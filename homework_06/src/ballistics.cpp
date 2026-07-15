#include "ballistics.hpp"

#include <iostream>
#include <cstring>
#include <cmath>
#define _USE_MATH_DEFINES

struct Ammo {
  float mass, drag, lift;
};

struct AmmoEntry {
  const char* name;
  Ammo ammo;
};

const double g = 9.81;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays) - fixed-size static internal data
const AmmoEntry kAmmoTable[] = {
  {"VOG-17", {0.35f, 0.07f, 0.0f}},
  {"M67", {0.6f, 0.10f, 0.0f}},
  {"RKG-3", {1.2f, 0.10f, 0.0f}},
  {"GLIDING-VOG", {0.45f, 0.10f, 1.0f}},
  {"GLIDING-RKG", {1.4f, 0.10f, 1.0f}},
};

bool find_ammo(const char* name, Ammo& out)  // NOLINT(modernize-use-trailing-return-type) - stylistic only
{
  for (const auto& entry : kAmmoTable) {
    if (strcmp(name, entry.name) == 0) {
      out = entry.ammo;
      return true;
    }
  }
  return false;
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type) - physics formula
// coefficients, not arbitrary constants; trailing return type is stylistic only
double cardano(Ammo ammo, float v0, float z0)
{
  double a = ammo.drag * g * ammo.mass - 2 * pow(ammo.drag, 2) * ammo.lift * v0;
  if (fabs(a) < 1e-9)
    return -1;
  double b = -3 * g * pow(ammo.mass, 2) + 3 * ammo.drag * ammo.lift * ammo.mass * v0;
  double c = 6 * pow(ammo.mass, 2) * z0;

  double p = -pow(b, 2) / (3 * pow(a, 2));
  double q = 2 * pow(b, 3) / (27 * pow(a, 3)) + c / a;

  double arg = 3 * q / (2 * p) * sqrt(static_cast<double>(-3) / p);
  if (arg < -1.0 || arg > 1.0)
    return -1;

  double phi = acos(arg);

  return 2 * sqrt(-p / 3) * cos((phi + 4 * M_PI) / 3) - b / (3 * a);
}

double horizon(double t, float v0, Ammo ammo)
{
  return v0 * t - pow(t, 2) * ammo.drag * v0 / (2 * ammo.mass) +
         pow(t, 3) * (6 * ammo.drag * g * ammo.lift * ammo.mass - 6 * pow(ammo.drag, 2) * (pow(ammo.lift, 2) - 1) * v0) /
           (36 * pow(ammo.mass, 2)) +
         pow(t, 4) *
           (-6 * pow(ammo.drag, 2) * g * ammo.lift * (1 + pow(ammo.lift, 2) + pow(ammo.lift, 4)) * ammo.mass +
            3 * pow(ammo.drag, 3) * pow(ammo.lift, 2) * (1 + pow(ammo.lift, 2)) * v0 +
            6 * pow(ammo.drag, 3) * pow(ammo.lift, 4) * (1 + pow(ammo.lift, 2)) * v0) /
           (36 * pow((1 + pow(ammo.lift, 2)), 2) * pow(ammo.mass, 3)) +
         pow(t, 5) *
           (3 * pow(ammo.drag, 3) * g * pow(ammo.lift, 3) * ammo.mass -
            3 * pow(ammo.drag, 4) * pow(ammo.lift, 2) * (1 + pow(ammo.lift, 2)) * v0) /
           (36 * (1 + pow(ammo.lift, 2)) * pow(ammo.mass, 4));
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers, modernize-use-trailing-return-type)

auto compute_drop_point(DroneState dronState, bool& ok) -> DropPoint
{
  Ammo ammo{};

  if (!find_ammo(dronState.ammo_name.c_str(), ammo)) {
    ok = false;
    std::cerr << "Unknown ammo\n";
    return DropPoint{};
  }

  double t = cardano(ammo, dronState.attackSpeed, dronState.zd);
  if (t < 0) {
    ok = false;
    std::cerr << "Time error\n";
    return DropPoint{};
  }

  double h = horizon(t, dronState.attackSpeed, ammo);
  if (h < 0) {
    ok = false;
    std::cerr << "Horizon error\n";
    return DropPoint{};
  }

  double D = sqrt(pow(dronState.targetX - dronState.xd, 2) + pow(dronState.targetY - dronState.yd, 2));
  if (fabs(D) < 1e-9) {  // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers) - floating-point comparison epsilon
    ok = false;
    std::cerr << "Calculation error\n";
    return DropPoint{};
  }

  if (h + dronState.accelerationPath > D) {
    dronState.xd = static_cast<float>(dronState.targetX - (dronState.targetX - dronState.xd) * (h + dronState.accelerationPath) / D);
    dronState.yd = static_cast<float>(dronState.targetY - (dronState.targetY - dronState.yd) * (h + dronState.accelerationPath) / D);
    D = sqrt(pow(dronState.targetX - dronState.xd, 2) + pow(dronState.targetY - dronState.yd, 2));
  }

  double ratio = (D - h) / D;

  DropPoint dropPoint{dronState.xd + (dronState.targetX - dronState.xd) * ratio, dronState.yd + (dronState.targetY - dronState.yd) * ratio};

  return dropPoint;
}