#include "states/DeceleratingState.h"
#include "states/StoppedState.h"

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> DeceleratingState::execute(const DroneTelemetry& tlm, DroneContext& /*ctx*/, DroneCommand& cmd)
{
  cmd.mode = DroneMode::DECELERATING;

  if (tlm.speed <= 0.0f) {
    return std::make_unique<StoppedState>();
  }

  return nullptr;
}

float DeceleratingState::estimateTimeToStop(const DroneTelemetry& tlm, const DroneContext& /*ctx*/) const
{
  return tlm.speed / tlm.acceleration;
}

// NOLINTEND(modernize-use-trailing-return-type)