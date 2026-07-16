#include "ballistic_solver.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

namespace {
constexpr float kG = 9.81f;

float cardano(const AmmoParams& ammo, float v0, float z0)
{
  float a = ammo.drag * kG * ammo.mass - 2 * pow(ammo.drag, 2) * ammo.lift * v0;
  if (fabs(a) < 1e-9)
    return -1;
  float b = -3 * kG * pow(ammo.mass, 2) + 3 * ammo.drag * ammo.lift * ammo.mass * v0;
  float c = 6 * pow(ammo.mass, 2) * z0;

  float p = -pow(b, 2) / (3 * pow(a, 2));
  float q = 2 * pow(b, 3) / (27 * pow(a, 3)) + c / a;

  float arg = 3 * q / (2 * p) * sqrt(static_cast<float>(-3) / p);
  if (arg < -1.0 || arg > 1.0)
    return -1;

  float phi = acos(arg);

  return 2 * sqrt(-p / 3) * cos((phi + 4 * M_PI) / 3) - b / (3 * a);
}

float horizon(const AmmoParams& ammo, float t, float v0)
{
  return v0 * t - pow(t, 2) * ammo.drag * v0 / (2 * ammo.mass) +
         pow(t, 3) * (6 * ammo.drag * kG * ammo.lift * ammo.mass - 6 * pow(ammo.drag, 2) * (pow(ammo.lift, 2) - 1) * v0) /
           (36 * pow(ammo.mass, 2)) +
         pow(t, 4) *
           (-6 * pow(ammo.drag, 2) * kG * ammo.lift * (1 + pow(ammo.lift, 2) + pow(ammo.lift, 4)) * ammo.mass +
            3 * pow(ammo.drag, 3) * pow(ammo.lift, 2) * (1 + pow(ammo.lift, 2)) * v0 +
            6 * pow(ammo.drag, 3) * pow(ammo.lift, 4) * (1 + pow(ammo.lift, 2)) * v0) /
           (36 * pow((1 + pow(ammo.lift, 2)), 2) * pow(ammo.mass, 3)) +
         pow(t, 5) *
           (3 * pow(ammo.drag, 3) * kG * pow(ammo.lift, 3) * ammo.mass -
            3 * pow(ammo.drag, 4) * pow(ammo.lift, 2) * (1 + pow(ammo.lift, 2)) * v0) /
           (36 * (1 + pow(ammo.lift, 2)) * pow(ammo.mass, 4));
}

}  // namespace

Coord AnalyticalSolver::solve(
  Coord dronePos, Coord targetPos, float altitude, float accelPath, float attackSpeed, const AmmoParams& ammo, bool& ok)
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

  float D = sqrtf(pow(targetPos.x - dronePos.x, 2) + pow(targetPos.y - dronePos.y, 2));
  if (fabsf(D) < 1e-9f) {
    ok = false;
    std::cerr << "Calculation error\n";
    return {};
  }

  Coord tempPos = dronePos;
  if (h + accelPath > D) {
    tempPos.x = targetPos.x - (targetPos.x - dronePos.x) * (h + accelPath) / D;
    tempPos.y = targetPos.y - (targetPos.y - dronePos.y) * (h + accelPath) / D;
    D = sqrtf(pow(targetPos.x - tempPos.x, 2) + pow(targetPos.y - tempPos.y, 2));
    if (fabsf(D) < 1e-9f) {
      ok = false;
      std::cerr << "Calculation error\n";
      return {};
    }
  }

  float ratio = (D - h) / D;
  return tempPos + (targetPos - tempPos) * ratio;
}