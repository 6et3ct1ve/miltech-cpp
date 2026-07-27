#include "states/DeceleratingState.h"
#include "states/StoppedState.h"

#include <cmath>

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> DeceleratingState::execute(DroneContext& ctx)
{
  ctx.speed -= ctx.acceleration * ctx.config->simTimeStep;

  bool stopped = false;
  if (ctx.speed <= 0) {
    ctx.speed = 0;
    stopped = true;
  }

  ctx.pos.x += cosf(ctx.direction) * ctx.speed * ctx.config->simTimeStep;
  ctx.pos.y += sinf(ctx.direction) * ctx.speed * ctx.config->simTimeStep;

  if (stopped) {
    return std::make_unique<StoppedState>();
  }
  return nullptr;
}

const char* DeceleratingState::name() const
{
  return "Decelerating";
}

float DeceleratingState::estimateTimeToStop(const DroneContext& ctx) const
{
  return ctx.speed / ctx.acceleration;
}

// NOLINTEND(modernize-use-trailing-return-type)