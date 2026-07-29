#include "states/StoppedState.h"
#include "states/TurningState.h"

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> StoppedState::execute(DroneContext& /*ctx*/)
{
  return std::make_unique<TurningState>();
}

const char* StoppedState::name() const
{
  return "Stopped";
}

float StoppedState::estimateTimeToStop(const DroneContext& /*ctx*/) const
{
  return 0.0f;
}

// NOLINTEND(modernize-use-trailing-return-type)