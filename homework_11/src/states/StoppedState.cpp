#include "states/StoppedState.h"
#include "states/TurningState.h"

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> StoppedState::execute(const DroneTelemetry& /*tlm*/, DroneContext& /*ctx*/, DroneCommand& cmd)
{
  cmd.mode = DroneMode::STOPPED;
  return std::make_unique<TurningState>();
}

float StoppedState::estimateTimeToStop(const DroneTelemetry& /*tlm*/, const DroneContext& /*ctx*/) const
{
  return 0.0f;
}

// NOLINTEND(modernize-use-trailing-return-type)