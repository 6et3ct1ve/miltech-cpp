#pragma once

#include "coord.hpp"
#include "config_loader.hpp"

class IBallisticSolver {
public:
  virtual Coord solve(Coord dronePos, Coord targetPos, float altitude, float accelPath, float attackSpeed, const AmmoParams& ammo, bool& ok) = 0;
  virtual ~IBallisticSolver() {}
};

class AnalyticalSolver : public IBallisticSolver {
public:
  Coord solve(Coord dronePos, Coord targetPos, float altitude, float accelPath, float attackSpeed, const AmmoParams& ammo,  bool& ok) override;
};