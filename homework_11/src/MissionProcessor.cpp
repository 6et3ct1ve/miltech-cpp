#include "MissionProcessor.h"
#include "Logging.h"
#include "Types.h"
#include "states/MovingState.h"
#include "interfaces/IBallisticSolver.h"
#include "interfaces/IDroneState.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <span>

// NOLINTBEGIN(modernize-use-trailing-return-type,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

MissionProcessor::MissionProcessor(std::unique_ptr<IBallisticSolver> solver)
  : solver_(std::move(solver))
  , currentState_(std::make_unique<MovingState>())
{
}

void MissionProcessor::configure(const DroneConfig& config, const AmmoParams& ammo, int targetCount)
{
  config_ = config;
  ammo_ = ammo;
  targetCount_ = targetCount;

  if (targetCount_ <= 0 || solver_ == nullptr) {
    std::cerr << "Invalid mission configuration\n";
    return;
  }

  firePoint_.resize(targetCount_);
  totalTime_.resize(targetCount_);
  predictedAll_.resize(targetCount_);

  prevTarget_ = -1;
  shouldDrop_ = false;
  ok_ = true;

  LOG("Mission configured, speed = " << config_.attackSpeed);
}

bool MissionProcessor::isReady() const
{
  return ok_;
}

bool MissionProcessor::shouldDrop() const
{
  return shouldDrop_;
}

DroneCommand MissionProcessor::step(const DroneTelemetry& tlm, std::span<const Target> targets)
{
  if (!ok_) {
    std::cerr << "MissionProcessor is not ready\n";
    return DroneCommand{};
  }

  if (targets.size() < static_cast<size_t>(targetCount_)) {
    std::cerr << "Not enough targets provided\n";
    return DroneCommand{};
  }

  float h = 0.0f;

  for (int i = 0; i < targetCount_; i++) {
    Target tgt = targets[i];
    Coord target = tgt.pos;
    Coord vel = tgt.velocity;

    float D = sqrtf(powf(target.x - tlm.pos.x, 2) + powf(target.y - tlm.pos.y, 2));
    if (fabsf(D) < 1e-9f) {
      std::cerr << "Calculation error\n";
      ok_ = false;
      return DroneCommand{};
    }
    totalTime_[i] = D / config_.attackSpeed;

    Coord predicted = target + vel * totalTime_[i];
    predictedAll_[i] = predicted;

    D = sqrtf(powf(predicted.x - tlm.pos.x, 2) + powf(predicted.y - tlm.pos.y, 2));
    if (fabsf(D) < 1e-9f) {
      std::cerr << "Calculation error\n";
      ok_ = false;
      return DroneCommand{};
    }
    totalTime_[i] = D / config_.attackSpeed;

    bool solverOk = true;
    firePoint_[i] = solver_->solve(tlm.pos, predicted, tlm.altitude, config_.accelPath, config_.attackSpeed, ammo_, h, solverOk);
    if (!solverOk) {
      ok_ = false;
      return DroneCommand{};
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

  if (sqrtf(powf(tlm.pos.x - firePoint_[currentTarget].x, 2) + powf(tlm.pos.y - firePoint_[currentTarget].y, 2)) < config_.hitRadius) {
    shouldDrop_ = true;
  }

  DEBUG("pos = (" << tlm.pos.x << "," << tlm.pos.y << ") target = " << currentTarget << " t = " << tlm.timeSecSinceStart);
  return cmd;
}
// NOLINTEND(modernize-use-trailing-return-type,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)