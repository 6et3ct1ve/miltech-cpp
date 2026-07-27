#include "solvers/TableSolver.h"

#include <cmath>

TableSolver::TableSolver(const std::string& tablePath)
  : loaded_(table_.load(tablePath))
{
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type) - epsilon comparisons;
// trailing return type is stylistic only
Coord TableSolver::solve(
  Coord dronePos, Coord targetPos, float altitude, float accelPath, float attackSpeed, const AmmoParams& ammo, float& outH, bool& ok)
{
  if (!loaded_) {
    ok = false;
    return {};
  }

  BallisticTable::Result result = table_.lookup(altitude, attackSpeed, ammo.mass, ammo.drag, ammo.lift);

  if (result.t <= 0.0f) {
    ok = false;
    return {};
  }

  outH = result.hDist;

  float D = sqrtf(powf(targetPos.x - dronePos.x, 2) + powf(targetPos.y - dronePos.y, 2));
  if (fabsf(D) < 1e-9f) {
    ok = false;
    return {};
  }

  Coord tempPos = dronePos;
  if (result.hDist + accelPath > D) {
    tempPos.x = targetPos.x - (targetPos.x - dronePos.x) * (result.hDist + accelPath) / D;
    tempPos.y = targetPos.y - (targetPos.y - dronePos.y) * (result.hDist + accelPath) / D;
    D = sqrtf(powf(targetPos.x - tempPos.x, 2) + powf(targetPos.y - tempPos.y, 2));
    if (fabsf(D) < 1e-9f) {
      ok = false;
      return {};
    }
  }

  float ratio = (D - result.hDist) / D;
  return tempPos + (targetPos - tempPos) * ratio;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type)

bool TableSolver::isValid() const  // NOLINT(modernize-use-trailing-return-type)
{
  return loaded_;
}