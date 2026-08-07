#include "MissionProcessor.h"
#include "Logging.h"
#include "states/StoppedState.h"
#include "states/MovingState.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// NOLINTBEGIN(modernize-use-trailing-return-type,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace {

const char* modeName(DroneMode mode)
{
  switch (mode) {
    case DroneMode::STOPPED:
      return "Stopped";
    case DroneMode::ACCELERATING:
      return "Accelerating";
    case DroneMode::DECELERATING:
      return "Decelerating";
    case DroneMode::TURNING:
      return "Turning";
    case DroneMode::MOVING:
      return "Moving";
  }
  return "Unknown";
}

}  // namespace

MissionProcessor::MissionProcessor(std::unique_ptr<ITargetProvider> provider,
                                   std::unique_ptr<IBallisticSolver> solver,
                                   std::unique_ptr<IConfigLoader> loader,
                                   DronePhysics* physics)
  : provider_(std::move(provider))
  , solver_(std::move(solver))
  , loader_(std::move(loader))
  , currentState_(std::make_unique<StoppedState>())
  , physics_(physics)
{
}

bool MissionProcessor::init(const std::string& configSource)
{
  if (configSource.empty()) {
    return false;
  }

  config_ = loader_->getConfig();
  ammo_ = loader_->getAmmoParams();

  if (physics_ == nullptr || !physics_->init(config_)) {
    std::cerr << "Physics init error\n";
    return false;
  }

  targetCount_ = provider_->getTargetCount();
  if (targetCount_ == 0) {
    std::cerr << "No targets available\n";
    return false;
  }

  currentState_ = std::make_unique<MovingState>();
  prevTarget_ = -1;
  steps_ = 0;

  firePoint_.resize(targetCount_);
  totalTime_.resize(targetCount_);
  predictedAll_.resize(targetCount_);
  simSteps_.reserve(kMaxSteps);

  const DroneTelemetry tlm = physics_->getTelemetry();
  bool solverOk = true;
  float dummyH = 0.0f;
  solver_->solve(tlm.pos, provider_->getTarget(0).pos, config_.altitude, config_.accelPath, config_.attackSpeed, ammo_, dummyH, solverOk);
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
  const DroneTelemetry tlm = physics_->getTelemetry();
  float h = 0.0f;

  for (int i = 0; i < targetCount_; i++) {
    Target tgt = provider_->getTarget(i);
    Coord target = tgt.pos;
    Coord vel = tgt.velocity;

    float D = sqrtf(powf(target.x - tlm.pos.x, 2) + powf(target.y - tlm.pos.y, 2));
    if (fabsf(D) < 1e-9f) {
      std::cerr << "Calculation error\n";
      ok_ = false;
      return;
    }
    totalTime_[i] = D / config_.attackSpeed;

    Coord predicted = target + vel * totalTime_[i];
    predictedAll_[i] = predicted;

    D = sqrtf(powf(predicted.x - tlm.pos.x, 2) + powf(predicted.y - tlm.pos.y, 2));
    if (fabsf(D) < 1e-9f) {
      std::cerr << "Calculation error\n";
      ok_ = false;
      return;
    }
    totalTime_[i] = D / config_.attackSpeed;

    bool solverOk = true;
    firePoint_[i] = solver_->solve(tlm.pos, predicted, config_.altitude, config_.accelPath, config_.attackSpeed, ammo_, h, solverOk);
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

  float newDir = atan2f(firePoint_[currentTarget].y - tlm.pos.y, firePoint_[currentTarget].x - tlm.pos.x);
  float deltaAngle = newDir - tlm.direction;
  while (deltaAngle > M_PI)
    deltaAngle -= 2 * M_PI;
  while (deltaAngle < -M_PI)
    deltaAngle += 2 * M_PI;

  DroneContext ctx;
  ctx.newDir = newDir;
  ctx.deltaAngle = deltaAngle;
  ctx.config = &config_;

  if (currentTarget != prevTarget_) {
    float timeToStop = currentState_->estimateTimeToStop(tlm, ctx);
    totalTime_[currentTarget] += timeToStop;
    prevTarget_ = currentTarget;
  }

  DroneCommand cmd{};
  constexpr int kMaxTransitionsPerStep = 5;
  for (int guard = 0; guard < kMaxTransitionsPerStep; guard++) {
    auto next = currentState_->execute(tlm, ctx, cmd);
    if (!next) {
      break;
    }
    currentState_ = std::move(next);
  }

  physics_->submit(cmd);

  SimStep newStep{};
  newStep.pos = tlm.pos;
  newStep.direction = tlm.direction;
  newStep.state = modeName(tlm.mode);
  newStep.targetIdx = currentTarget;
  newStep.dropPoint = firePoint_[currentTarget];
  newStep.aimPoint = tlm.pos + Coord{cosf(tlm.direction), sinf(tlm.direction)} * h;
  newStep.predictedTarget = predictedAll_[currentTarget];
  newStep.timeSecSinceStart = tlm.timeSecSinceStart;
  simSteps_.push_back(newStep);
  steps_++;

  if (sqrtf(powf(tlm.pos.x - firePoint_[currentTarget].x, 2) + powf(tlm.pos.y - firePoint_[currentTarget].y, 2)) < config_.hitRadius) {
    finished_ = true;
  }

  DEBUG("Step " << steps_ << " pos = (" << tlm.pos.x << "," << tlm.pos.y << ") target = " << currentTarget
                << " state = " << modeName(tlm.mode));
}

void MissionProcessor::reset()
{
  currentState_ = std::make_unique<MovingState>();
  prevTarget_ = -1;
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
// NOLINTEND(modernize-use-trailing-return-type,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)