#pragma once
#include <string>

struct DropPoint {
  double fireX, fireY;
};

struct DroneState {
  float xd, yd, zd, targetX, targetY, attackSpeed, accelerationPath;
  std::string ammo_name;
};

auto compute_drop_point(DroneState dronState, bool& ok) -> DropPoint;
