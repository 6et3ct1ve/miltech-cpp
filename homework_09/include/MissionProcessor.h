#pragma once

#include "Types.h"
#include "interfaces/IConfigLoader.h"
#include "interfaces/ITargetProvider.h"
#include "interfaces/IBallisticSolver.h"
#include "interfaces/IDroneState.h"
#include <memory>
#include <string>
#include <vector>

constexpr int kMaxSteps = 10000;

class MissionProcessor {
public:
  MissionProcessor(std::unique_ptr<ITargetProvider> provider,
                   std::unique_ptr<IBallisticSolver> solver,
                   std::unique_ptr<IConfigLoader> loader);
  MissionProcessor(const MissionProcessor&) = delete;
  MissionProcessor& operator=(const MissionProcessor&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  MissionProcessor(MissionProcessor&&) = delete;
  MissionProcessor& operator=(MissionProcessor&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

  bool init(const std::string& configSource);  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] bool hasNext() const;          // NOLINT(modernize-use-trailing-return-type)
  void step();
  void reset();
  void changeSolver(std::unique_ptr<IBallisticSolver> solver);
  [[nodiscard]] int getStepCount() const;                      // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] const std::vector<SimStep>& getSteps() const;  // NOLINT(modernize-use-trailing-return-type)

private:
  std::unique_ptr<ITargetProvider> provider_;
  std::unique_ptr<IBallisticSolver> solver_;
  std::unique_ptr<IConfigLoader> loader_;
  std::unique_ptr<IDroneState> currentState_;

  DroneConfig config_{};
  AmmoParams ammo_{};

  Coord dronePos_{};
  float dir_ = 0.0f;
  float speed_ = 0.0f;
  float acceleration_ = 0.0f;
  int prevTarget_ = -1;
  float currentTime_ = 0.0f;
  int steps_ = 0;
  bool ok_ = true;
  bool finished_ = false;
  int targetCount_ = 0;

  std::vector<Coord> firePoint_;
  std::vector<float> totalTime_;
  std::vector<Coord> predictedAll_;

  std::vector<SimStep> simSteps_;
};