#pragma once

#include "coord.hpp"

struct Target {
  Coord* positions;
  int timeSteps;
  float arrayTimeStep;
};

class ITargetProvider {
public:
  virtual int getTargetCount() = 0;
  virtual Target getTarget(int index) = 0;
  virtual ~ITargetProvider() {}
};