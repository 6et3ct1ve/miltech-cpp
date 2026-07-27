#include "states/MovingState.h"
#include "states/DeceleratingState.h"

#include <cmath>

// NOLINTBEGIN(modernize-use-trailing-return-type)

std::unique_ptr<IDroneState> MovingState::execute(DroneContext& ctx)
{
  if (fabsf(ctx.deltaAngle) > ctx.config->turnThreshold) {
    return std::make_unique<DeceleratingState>();
  }

  ctx.direction = ctx.newDir;
  ctx.pos.x += cosf(ctx.direction) * ctx.speed * ctx.config->simTimeStep;
  ctx.pos.y += sinf(ctx.direction) * ctx.speed * ctx.config->simTimeStep;

  return nullptr;
}

const char* MovingState::name() const
{
  return "Moving";
}

float MovingState::estimateTimeToStop(const DroneContext& ctx) const
{
  return ctx.config->attackSpeed / ctx.acceleration;
}

// NOLINTEND(modernize-use-trailing-return-type)