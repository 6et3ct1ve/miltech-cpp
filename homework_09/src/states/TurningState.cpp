#include "states/TurningState.h"
#include "states/AcceleratingState.h"

#include <cmath>

namespace {

float sign(float delta)
{
  return (fabsf(delta) < 1e-9f) ? 0.0f : (delta > 0) ? 1.0f : -1.0f;
}

}  // namespace

std::unique_ptr<IDroneState> TurningState::execute(DroneContext& ctx)
{
  ctx.direction += sign(ctx.deltaAngle) * ctx.config->angularSpeed * ctx.config->simTimeStep;

  if (fabsf(ctx.deltaAngle) <= ctx.config->turnThreshold) {
    return std::make_unique<AcceleratingState>();
  }
  return nullptr;
}

const char* TurningState::name() const
{
  return "Turning";
}

float TurningState::estimateTimeToStop(const DroneContext& ctx) const
{
  return fabsf(ctx.deltaAngle) / ctx.config->angularSpeed;
}