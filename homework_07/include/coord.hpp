#pragma once

#include <iostream>
#include <cmath>

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