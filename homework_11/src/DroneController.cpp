#include "DroneController.h"
#include "Types.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kEpsilon = 1e-9f;
}

void DroneController::configure(const DroneConfig& config)
{
  angularSpeed_ = config.angularSpeed;
}

ControlOutput DroneController::toControl(const DroneCommand& cmd) const  // NOLINT(modernize-use-trailing-return-type)
{
  ControlOutput out{};

  if (fabsf(angularSpeed_) > kEpsilon) {
    out.turnRate = std::clamp(cmd.angleSpeed / angularSpeed_, -1.0f, 1.0f);
  }

  switch (cmd.mode) {
    case DroneMode::ACCELERATING:
      out.accel = 1.0f;
      break;
    case DroneMode::DECELERATING:
      out.accel = -1.0f;
      break;
    case DroneMode::STOPPED:
    case DroneMode::TURNING:
    case DroneMode::MOVING:
      out.accel = 0.0f;
      break;
  }

  return out;
}