#include "TargetTracker.h"

#include <iostream>
#include <cmath>

namespace {
constexpr float kEpsilon = 1e-9f;
}

void TargetTracker::resize(int count)
{
  if (count < 0) {
    std::cerr << "Count cannot be less zero\n";
    return;
  }
  targets_.resize(static_cast<size_t>(count));
  prevPos_.resize(static_cast<size_t>(count));
  prevTime_.resize(static_cast<size_t>(count));
}

void TargetTracker::update(int id, Coord pos, float t)
{
  if (id < 0 || static_cast<size_t>(id) >= targets_.size()) {
    return;
  }
  const auto i = static_cast<size_t>(id);

  if (prevTime_[i].has_value()) {
    const float dt = t - *prevTime_[i];
    if (fabsf(dt) > kEpsilon) {
      targets_[i].velocity = (pos - prevPos_[i]) / dt;
    }
  }

  targets_[i].pos = pos;
  prevPos_[i] = pos;
  prevTime_[i] = t;
}

std::span<const Target> TargetTracker::targets() const  // NOLINT(modernize-use-trailing-return-type)
{
  return targets_;
}