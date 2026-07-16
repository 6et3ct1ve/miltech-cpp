#pragma once

#include "coord.hpp"

enum class DroneState { STOPPED, ACCELERATING, DECELERATING, TURNING, MOVING };

struct SimStep {
  Coord pos;
  float direction;
  DroneState state;
  int targetIdx;
  Coord dropPoint;
  Coord aimPoint;
  Coord predictedTarget;
};