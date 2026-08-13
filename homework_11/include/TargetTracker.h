#pragma once

#include "Types.h"

#include <optional>
#include <span>
#include <vector>

class TargetTracker {
public:
  TargetTracker() = default;
  TargetTracker(const TargetTracker&) = delete;
  TargetTracker& operator=(const TargetTracker&) = delete;
  TargetTracker(TargetTracker&&) = delete;
  TargetTracker& operator=(TargetTracker&&) = delete;

  void resize(int count);
  void update(int id, Coord pos, float t);
  [[nodiscard]] std::span<const Target> targets() const;  // NOLINT(modernize-use-trailing-return-type)

private:
  std::vector<Target> targets_;
  std::vector<Coord> prevPos_;
  std::vector<std::optional<float>> prevTime_;
};