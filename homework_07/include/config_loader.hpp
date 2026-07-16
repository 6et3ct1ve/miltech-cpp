#pragma once

#include "coord.hpp"

struct AmmoParams {
  char name[32];
  float mass, drag, lift;
};
struct DroneConfig {
  Coord startPos;
  float altitude;
  float initialDir;
  float attackSpeed;
  float accelPath;
  char ammoName[32];
  float arrayTimeStep;
  float simTimeStep;
  float hitRadius;
  float angularSpeed;
  float turnThreshold;
};