#include "states/MovingState.h"
#include "states/DeceleratingState.h"

#include <cmath>

// NOLINTBEGIN(modernize-use-trailing-return-type)

namespace {
constexpr float kDeadbandFactor = 3.0f;
}  // namespace

std::unique_ptr<IDroneState> MovingState::execute(const DroneTelemetry& /*tlm*/, DroneContext& ctx, DroneCommand& cmd)
{
  if (fabsf(ctx.deltaAngle) > ctx.config->turnThreshold) {
    return std::make_unique<DeceleratingState>();
  }

  const float deadband = ctx.config->angularSpeed * ctx.config->physicsTimeStep * kDeadbandFactor;

  cmd.mode = DroneMode::MOVING;
  cmd.angleSpeed = (fabsf(ctx.deltaAngle) > deadband) ? std::copysign(ctx.config->angularSpeed, ctx.deltaAngle) : 0.0f;
  return nullptr;
}

float MovingState::estimateTimeToStop(const DroneTelemetry& tlm, const DroneContext& /*ctx*/) const
{
  return tlm.speed / tlm.acceleration;
}

// NOLINTEND(modernize-use-trailing-return-type)