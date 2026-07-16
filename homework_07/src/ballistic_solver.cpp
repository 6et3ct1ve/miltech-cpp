#include "ballistic_solver.hpp"

#include <numbers>
#include <cmath>
#include <iostream>

namespace {
constexpr float kG = 9.81f;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type) - physics formula
// coefficients, not arbitrary constants; trailing return type is stylistic only
float cardano(const AmmoParams& ammo, float v0, float z0)
{
  float a = ammo.drag * kG * ammo.mass - 2 * powf(ammo.drag, 2) * ammo.lift * v0;
  if (fabsf(a) < 1e-9f) {
    return -1;
  }
  float b = -3 * kG * powf(ammo.mass, 2) + 3 * ammo.drag * ammo.lift * ammo.mass * v0;
  float c = 6 * powf(ammo.mass, 2) * z0;

  float p = -powf(b, 2) / (3 * powf(a, 2));
  float q = 2 * powf(b, 3) / (27 * powf(a, 3)) + c / a;

  float arg = 3 * q / (2 * p) * sqrtf(static_cast<float>(-3) / p);
  if (arg < -1.0f || arg > 1.0f) {
    return -1;
  }

  float phi = acosf(arg);

  return 2 * sqrtf(-p / 3) * cosf((phi + 4 * std::numbers::pi_v<float>) / 3) - b / (3 * a);
}

float horizon(const AmmoParams& ammo, float t, float v0)
{
  return v0 * t - powf(t, 2) * ammo.drag * v0 / (2 * ammo.mass) +
         powf(t, 3) * (6 * ammo.drag * kG * ammo.lift * ammo.mass - 6 * powf(ammo.drag, 2) * (powf(ammo.lift, 2) - 1) * v0) /
           (36 * powf(ammo.mass, 2)) +
         powf(t, 4) *
           (-6 * powf(ammo.drag, 2) * kG * ammo.lift * (1 + powf(ammo.lift, 2) + powf(ammo.lift, 4)) * ammo.mass +
            3 * powf(ammo.drag, 3) * powf(ammo.lift, 2) * (1 + powf(ammo.lift, 2)) * v0 +
            6 * powf(ammo.drag, 3) * powf(ammo.lift, 4) * (1 + powf(ammo.lift, 2)) * v0) /
           (36 * powf((1 + powf(ammo.lift, 2)), 2) * powf(ammo.mass, 3)) +
         powf(t, 5) *
           (3 * powf(ammo.drag, 3) * kG * powf(ammo.lift, 3) * ammo.mass -
            3 * powf(ammo.drag, 4) * powf(ammo.lift, 2) * (1 + powf(ammo.lift, 2)) * v0) /
           (36 * (1 + powf(ammo.lift, 2)) * powf(ammo.mass, 4));
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type)

}  // namespace

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type) - epsilon comparisons;
// trailing return type is stylistic only
Coord AnalyticalSolver::solve(
  Coord dronePos, Coord targetPos, float altitude, float accelPath, float attackSpeed, const AmmoParams& ammo, float& outH, bool& ok)
{
  float t = cardano(ammo, attackSpeed, altitude);
  if (t < 0) {
    ok = false;
    std::cerr << "Time error\n";
    return {};
  }

  float h = horizon(ammo, t, attackSpeed);
  if (h < 0) {
    ok = false;
    std::cerr << "Horizon error\n";
    return {};
  }
  outH = h;

  float D = sqrtf(powf(targetPos.x - dronePos.x, 2) + powf(targetPos.y - dronePos.y, 2));
  if (fabsf(D) < 1e-9f) {
    ok = false;
    std::cerr << "Calculation error\n";
    return {};
  }

  Coord tempPos = dronePos;
  if (h + accelPath > D) {
    tempPos.x = targetPos.x - (targetPos.x - dronePos.x) * (h + accelPath) / D;
    tempPos.y = targetPos.y - (targetPos.y - dronePos.y) * (h + accelPath) / D;
    D = sqrtf(powf(targetPos.x - tempPos.x, 2) + powf(targetPos.y - tempPos.y, 2));
    if (fabsf(D) < 1e-9f) {
      ok = false;
      std::cerr << "Calculation error\n";
      return {};
    }
  }

  float ratio = (D - h) / D;
  return tempPos + (targetPos - tempPos) * ratio;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type)