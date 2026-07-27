#pragma once

#include "interfaces/IBallisticSolver.h"

class AnalyticalSolver : public IBallisticSolver {
public:
  Coord solve(Coord dronePos,  // NOLINT(modernize-use-trailing-return-type)
              Coord targetPos,
              float altitude,
              float accelPath,
              float attackSpeed,
              const AmmoParams& ammo,
              float& outH,
              bool& ok) override;
};