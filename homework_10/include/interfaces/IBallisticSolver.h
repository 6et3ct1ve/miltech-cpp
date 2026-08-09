#pragma once

#include "Types.h"

class IBallisticSolver {
public:
  IBallisticSolver() = default;
  IBallisticSolver(const IBallisticSolver&) = delete;
  IBallisticSolver& operator=(const IBallisticSolver&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  IBallisticSolver(IBallisticSolver&&) = delete;
  IBallisticSolver& operator=(IBallisticSolver&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  virtual Coord solve(                                       // NOLINT(modernize-use-trailing-return-type)
    Coord dronePos,
    Coord targetPos,
    float altitude,
    float accelPath,
    float attackSpeed,
    const AmmoParams& ammo,
    float& outH,
    bool& ok) = 0;
  virtual ~IBallisticSolver() = default;
};