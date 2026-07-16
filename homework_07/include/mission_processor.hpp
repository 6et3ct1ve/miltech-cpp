#pragma once

#include "coord.hpp"
#include "config_loader.hpp"
#include "target_provider.hpp"
#include "ballistic_solver.hpp"

constexpr int kMaxSteps = 10000;

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

class MissionProcessor {
public:
  MissionProcessor(ITargetProvider* provider, IBallisticSolver* solver, IConfigLoader* loader);
  MissionProcessor(const MissionProcessor&) = delete;
  MissionProcessor& operator=(const MissionProcessor&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  MissionProcessor(MissionProcessor&&) = delete;
  MissionProcessor& operator=(MissionProcessor&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  ~MissionProcessor();

  bool init(const char* configSource);
  bool hasNext() const;
  void step();
  void reset();
  void changeSolver(IBallisticSolver* solver);
  [[nodiscard]] int getStepCount() const;         // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] const SimStep* getSteps() const;  // NOLINT(modernize-use-trailing-return-type)

private:
  ITargetProvider* provider_;
  IBallisticSolver* solver_;
  IConfigLoader* loader_;

  DroneConfig config_{};
  AmmoParams ammo_{};

  Coord dronePos_{};
  float dir_ = 0.0f;
  float speed_ = 0.0f;
  float acceleration_ = 0.0f;
  DroneState droneState_ = DroneState::STOPPED;
  int prevTarget_ = -1;
  float currentTime_ = 0.0f;
  int steps_ = 0;
  bool ok_ = true;
  bool finished_ = false;
  int targetCount_ = 0;

  Coord* firePoint_ = nullptr;
  float* totalTime_ = nullptr;
  Coord* predictedAll_ = nullptr;

  SimStep* simSteps_ = nullptr;
};