#include "states/AcceleratingState.h"
#include "states/MovingState.h"

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> AcceleratingState::execute(const DroneTelemetry& tlm, DroneContext& ctx, DroneCommand& cmd)
{
  cmd.mode = DroneMode::ACCELERATING;

  if (tlm.speed >= ctx.config->attackSpeed) {
    return std::make_unique<MovingState>();
  }

  return nullptr;
}

float AcceleratingState::estimateTimeToStop(const DroneTelemetry& tlm, const DroneContext& /*ctx*/) const
{
  return tlm.speed / tlm.acceleration;
}

// NOLINTEND(modernize-use-trailing-return-type)