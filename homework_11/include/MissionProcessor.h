#pragma once

#include "Types.h"
#include "interfaces/IBallisticSolver.h"
#include "interfaces/IDroneState.h"

#include <memory>
#include <span>
#include <vector>

class MissionProcessor {
public:
  explicit MissionProcessor(std::unique_ptr<IBallisticSolver> solver);
  MissionProcessor(const MissionProcessor&) = delete;
  MissionProcessor& operator=(const MissionProcessor&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  MissionProcessor(MissionProcessor&&) = delete;
  MissionProcessor& operator=(MissionProcessor&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

  void configure(const DroneConfig& config, const AmmoParams& ammo, int targetCount);
  [[nodiscard]] bool isReady() const;  // NOLINT(modernize-use-trailing-return-type)

  DroneCommand step(const DroneTelemetry& tlm, std::span<const Target> targets);  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] bool shouldDrop() const;                                          // NOLINT(modernize-use-trailing-return-type)

private:
  std::unique_ptr<IBallisticSolver> solver_;
  std::unique_ptr<IDroneState> currentState_;

  DroneConfig config_{};
  AmmoParams ammo_{};

  int prevTarget_ = -1;
  int targetCount_ = 0;
  bool ok_ = false;
  bool shouldDrop_ = false;

  std::vector<Coord> firePoint_;
  std::vector<float> totalTime_;
  std::vector<Coord> predictedAll_;
};