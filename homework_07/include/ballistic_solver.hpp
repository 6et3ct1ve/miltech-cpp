#pragma once

#include "coord.hpp"
#include "config_loader.hpp"

class IBallisticSolver {
public:
  IBallisticSolver() = default;
  IBallisticSolver(const IBallisticSolver&) = delete;
  IBallisticSolver& operator=(const IBallisticSolver&) = delete;
  virtual Coord solve(
    Coord dronePos, Coord targetPos, float altitude, float accelPath, float attackSpeed, const AmmoParams& ammo, float& outH, bool& ok) = 0;
  virtual ~IBallisticSolver() = default;
};

class AnalyticalSolver : public IBallisticSolver {
public:
  Coord solve(Coord dronePos,
              Coord targetPos,
              float altitude,
              float accelPath,
              float attackSpeed,
              const AmmoParams& ammo,
              float& outH,
              bool& ok) override;
};