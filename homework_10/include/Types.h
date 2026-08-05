#pragma once

#include <iostream>
#include <cmath>
#include <string>

// NOLINTBEGIN(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers) - epsilon comparisons;
// trailing return type is stylistic only
struct Coord {
  float x = 0.0f;
  float y = 0.0f;

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
  Coord pos{};
  Coord velocity{};
};

struct AmmoParams {
  std::string name;
  float mass = 0.0f, drag = 0.0f, lift = 0.0f;
};

enum class DroneMode { STOPPED, ACCELERATING, DECELERATING, TURNING, MOVING };

struct DroneConfig {
  Coord startPos{};
  float altitude = 0.0f;
  float initialDir = 0.0f;
  float attackSpeed = 0.0f;
  float accelPath = 0.0f;
  std::string ammoName;
  float arrayTimeStep = 0.0f;
  float simTimeStep = 0.0f;
  float hitRadius = 0.0f;
  float angularSpeed = 0.0f;
  float turnThreshold = 0.0f;
  float physicsTimeStep = 0.01f;
  float timeScale = 1.0f;
};

struct DroneContext {
  float newDir = 0.0f;
  float deltaAngle = 0.0f;
  const DroneConfig* config = nullptr;
};

struct DroneCommand {
  DroneMode mode = DroneMode::STOPPED;
  float angleSpeed = 0.0f;
};

struct DroneTelemetry {
  Coord pos{};
  float speed = 0.0f;
  float direction = 0.0f;
  DroneMode mode = DroneMode::STOPPED;
  float timeSecSinceStart = 0.0f;
};

struct SimStep {
  Coord pos{};
  float direction = 0.0f;
  std::string state;
  int targetIdx = 0;
  Coord dropPoint{};
  Coord aimPoint{};
  Coord predictedTarget{};
  float timeSecSinceStart = 0.0f;
};