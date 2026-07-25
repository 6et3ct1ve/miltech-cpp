#pragma once

#include <iostream>
#include <cmath>
#include <string>
#include <span>

// NOLINTBEGIN(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers) - epsilon comparisons;
// trailing return type is stylistic only
struct Coord {
  float x;
  float y;

  Coord operator+(const Coord& other) const
  {
    Coord result{};
    result.x = x + other.x;
    result.y = y + other.y;
    return result;
  }

  Coord operator-(const Coord& other) const
  {
    Coord result{};
    result.x = x - other.x;
    result.y = y - other.y;
    return result;
  }

  Coord operator*(float s) const
  {
    Coord result{};
    result.x = x * s;
    result.y = y * s;
    return result;
  }

  Coord operator/(float s) const
  {
    Coord result{};
    if (fabsf(s) < 1e-9f) {
      std::cerr << "Div error\n";
      return {0.0f, 0.0f};
    }
    result.x = x / s;
    result.y = y / s;
    return result;
  }

  bool operator==(const Coord& other) const { return fabsf(x - other.x) < 1e-9f && fabsf(y - other.y) < 1e-9f; }
};
// NOLINTEND(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

struct Target {
  std::span<const Coord> positions;
  float arrayTimeStep;
};

struct AmmoParams {
  std::string name;
  float mass, drag, lift;
};

struct DroneConfig {
  Coord startPos;
  float altitude;
  float initialDir;
  float attackSpeed;
  float accelPath;
  std::string ammoName;
  float arrayTimeStep;
  float simTimeStep;
  float hitRadius;
  float angularSpeed;
  float turnThreshold;
};

enum class DroneState { STOPPED, ACCELERATING, DECELERATING, TURNING, MOVING };

struct SimStep {
  Coord pos;
  float direction;
  DroneState state;
  int targetIdx;
  Coord dropPoint;
  Coord aimPoint;
  Coord predictedTarget;
};