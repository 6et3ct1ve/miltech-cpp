#include "states/TurningState.h"
#include "states/AcceleratingState.h"

#include <cmath>

// NOLINTBEGIN(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers) - epsilon comparison

std::unique_ptr<IDroneState> TurningState::execute(const DroneTelemetry& /*tlm*/, DroneContext& ctx, DroneCommand& cmd)
{
  cmd.mode = DroneMode::TURNING;
  cmd.angleSpeed = std::copysign(ctx.config->angularSpeed, ctx.deltaAngle);

  if (fabsf(ctx.deltaAngle) <= ctx.config->turnThreshold) {
    return std::make_unique<AcceleratingState>();
  }
  return nullptr;
}

float TurningState::estimateTimeToStop(const DroneTelemetry& /*tlm*/, const DroneContext& ctx) const
{
  return fabsf(ctx.deltaAngle) / ctx.config->angularSpeed;
}

// NOLINTEND(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)