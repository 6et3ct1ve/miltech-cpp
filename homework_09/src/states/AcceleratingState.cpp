#include "states/AcceleratingState.h"
#include "states/MovingState.h"

#include <cmath>

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> AcceleratingState::execute(DroneContext& ctx)
{
  ctx.speed += ctx.acceleration * ctx.config->simTimeStep;

  bool reachedTarget = false;
  if (ctx.speed >= ctx.config->attackSpeed) {
    ctx.speed = ctx.config->attackSpeed;
    reachedTarget = true;
  }

  ctx.pos.x += cosf(ctx.direction) * ctx.speed * ctx.config->simTimeStep;
  ctx.pos.y += sinf(ctx.direction) * ctx.speed * ctx.config->simTimeStep;

  if (reachedTarget) {
    return std::make_unique<MovingState>();
  }
  return nullptr;
}

const char* AcceleratingState::name() const
{
  return "Accelerating";
}

float AcceleratingState::estimateTimeToStop(const DroneContext& ctx) const
{
  return ctx.speed / ctx.acceleration;
}

// NOLINTEND(modernize-use-trailing-return-type)