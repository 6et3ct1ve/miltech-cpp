#include "MissionProcessor.h"
#include "interfaces/IBallisticSolver.h"
#include "interfaces/IConfigLoader.h"
#include "Logging.h"
#include "interfaces/ITargetProvider.h"
#include "states/StoppedState.h"
#include "states/MovingState.h"

#include <cmath>
#include <iostream>
#include <string>
#include <span>
#include <vector>

// NOLINTBEGIN(modernize-use-trailing-return-type,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,cppcoreguidelines-pro-bounds-pointer-arithmetic)

namespace {

Coord interpolateTarget(std::span<const Coord> target, float arrayTimeStep, float currentTime)
{
  int timeSteps = static_cast<int>(target.size());
  int idx = static_cast<int>(floor(currentTime / arrayTimeStep)) % timeSteps;
  int next = (idx + 1) % timeSteps;
  float frac = (currentTime - static_cast<float>(idx) * arrayTimeStep) / arrayTimeStep;
  return target[idx] + (target[next] - target[idx]) * frac;
}

Coord velocity(std::span<const Coord> target, float arrayTimeStep, float currentTime, float simTimeStep)
{
  Coord d = interpolateTarget(target, arrayTimeStep, currentTime + simTimeStep) - interpolateTarget(target, arrayTimeStep, currentTime);
  return d / simTimeStep;
}

}  // namespace

MissionProcessor::MissionProcessor(std::unique_ptr<ITargetProvider> provider,
                                   std::unique_ptr<IBallisticSolver> solver,
                                   std::unique_ptr<IConfigLoader> loader)
  : provider_(std::move(provider))
  , solver_(std::move(solver))
  , loader_(std::move(loader))
  , currentState_(std::make_unique<StoppedState>())
{
}

bool MissionProcessor::init(const std::string& configSource)
{
  if (configSource.empty()) {
    return false;
  }

  config_ = loader_->getConfig();
  ammo_ = loader_->getAmmoParams();

  acceleration_ = (config_.attackSpeed * config_.attackSpeed) / (2 * config_.accelPath);
  if (fabsf(acceleration_) < 1e-9f) {
    std::cerr << "Calculation error\n";
    return false;
  }

  targetCount_ = provider_->getTargetCount();
  if (targetCount_ == 0) {
    std::cerr << "No targets available\n";
    return false;
  }

  dronePos_ = config_.startPos;
  dir_ = config_.initialDir;
  speed_ = config_.attackSpeed;
  currentState_ = std::make_unique<MovingState>();
  prevTarget_ = -1;
  currentTime_ = 0.0f;
  steps_ = 0;

  firePoint_.resize(targetCount_);
  totalTime_.resize(targetCount_);
  predictedAll_.resize(targetCount_);
  simSteps_.reserve(kMaxSteps);

  Target firstTarget = provider_->getTarget(0);
  Coord firstPos = interpolateTarget(firstTarget.positions, firstTarget.arrayTimeStep, 0.0f);
  bool solverOk = true;
  float dummyH = 0.0f;
  solver_->solve(dronePos_, firstPos, config_.altitude, config_.accelPath, config_.attackSpeed, ammo_, dummyH, solverOk);
  if (!solverOk) {
    return false;
  }

  LOG("Config loaded, speed = " << config_.attackSpeed);

  return true;
}

bool MissionProcessor::hasNext() const
{
  return steps_ < kMaxSteps && ok_ && !finished_;
}

void MissionProcessor::step()
{
  float h = 0.0f;

  for (int i = 0; i < targetCount_; i++) {
    Target tgt = provider_->getTarget(i);
    Coord target = interpolateTarget(tgt.positions, tgt.arrayTimeStep, currentTime_);
    Coord vel = velocity(tgt.positions, tgt.arrayTimeStep, currentTime_, config_.simTimeStep);

    float D = sqrtf(powf(target.x - dronePos_.x, 2) + powf(target.y - dronePos_.y, 2));
    if (fabsf(D) < 1e-9f) {
      std::cerr << "Calculation error\n";
      ok_ = false;
      return;
    }
    totalTime_[i] = D / config_.attackSpeed;

    Coord predicted = target + vel * totalTime_[i];
    predictedAll_[i] = predicted;

    D = sqrtf(powf(predicted.x - dronePos_.x, 2) + powf(predicted.y - dronePos_.y, 2));
    if (fabsf(D) < 1e-9f) {
      std::cerr << "Calculation error\n";
      ok_ = false;
      return;
    }
    totalTime_[i] = D / config_.attackSpeed;

    bool solverOk = true;
    firePoint_[i] = solver_->solve(dronePos_, predicted, config_.altitude, config_.accelPath, config_.attackSpeed, ammo_, h, solverOk);
    if (!solverOk) {
      ok_ = false;
      return;
    }
  }

  float currentTargetTime = totalTime_[0];
  int currentTarget = 0;
  for (int i = 1; i < targetCount_; i++) {
    if (totalTime_[i] < currentTargetTime) {
      currentTarget = i;
      currentTargetTime = totalTime_[i];
    }
  }

  float newDir = atan2f(firePoint_[currentTarget].y - dronePos_.y, firePoint_[currentTarget].x - dronePos_.x);
  float deltaAngle = newDir - dir_;
  while (deltaAngle > M_PI)
    deltaAngle -= 2 * M_PI;
  while (deltaAngle < -M_PI)
    deltaAngle += 2 * M_PI;

  DroneContext ctx;
  ctx.pos = dronePos_;
  ctx.direction = dir_;
  ctx.speed = speed_;
  ctx.acceleration = acceleration_;
  ctx.newDir = newDir;
  ctx.deltaAngle = deltaAngle;
  ctx.config = &config_;

  if (currentTarget != prevTarget_) {
    float timeToStop = currentState_->estimateTimeToStop(ctx);
    totalTime_[currentTarget] += timeToStop;
    prevTarget_ = currentTarget;
  }

  constexpr int kMaxTransitionsPerStep = 5;
  for (int guard = 0; guard < kMaxTransitionsPerStep; guard++) {
    auto next = currentState_->execute(ctx);
    if (!next) {
      break;
    }
    currentState_ = std::move(next);
  }

  dronePos_ = ctx.pos;
  dir_ = ctx.direction;
  speed_ = ctx.speed;

  SimStep newStep{};
  newStep.pos = dronePos_;
  newStep.direction = dir_;
  newStep.state = currentState_->name();
  newStep.targetIdx = currentTarget;
  newStep.dropPoint = firePoint_[currentTarget];
  newStep.aimPoint = dronePos_ + Coord{cosf(dir_), sinf(dir_)} * h;
  newStep.predictedTarget = predictedAll_[currentTarget];
  simSteps_.push_back(newStep);
  steps_++;

  if (sqrtf(powf(dronePos_.x - firePoint_[currentTarget].x, 2) + powf(dronePos_.y - firePoint_[currentTarget].y, 2)) < config_.hitRadius) {
    finished_ = true;
  }

  currentTime_ += config_.simTimeStep;

  DEBUG("Step " << steps_ << " pos = (" << dronePos_.x << "," << dronePos_.y << ") target = " << currentTarget
                << " state = " << currentState_->name());
}

void MissionProcessor::reset()
{
  dronePos_ = config_.startPos;
  dir_ = config_.initialDir;
  speed_ = config_.attackSpeed;
  currentState_ = std::make_unique<MovingState>();
  prevTarget_ = -1;
  currentTime_ = 0.0f;
  steps_ = 0;
  ok_ = true;
  finished_ = false;
}

void MissionProcessor::changeSolver(std::unique_ptr<IBallisticSolver> solver)
{
  solver_ = std::move(solver);
}
int MissionProcessor::getStepCount() const
{
  return steps_;
}

const std::vector<SimStep>& MissionProcessor::getSteps() const
{
  return simSteps_;
}
// NOLINTEND(modernize-use-trailing-return-type,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,cppcoreguidelines-pro-bounds-pointer-arithmetic)