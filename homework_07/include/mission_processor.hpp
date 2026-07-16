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
  ~MissionProcessor();

  bool init(const char* configSource);
  bool hasNext() const;
  void step();
  void reset();
  void changeSolver(IBallisticSolver* solver);
  int getStepCount() const;
  const SimStep* getSteps() const;

private:
  ITargetProvider* provider_;
  IBallisticSolver* solver_;
  IConfigLoader* loader_;

  DroneConfig config_;
  AmmoParams ammo_;

  Coord dronePos_;
  float dir_;
  float speed_;
  float acceleration_;
  DroneState droneState_;
  int prevTarget_;
  float currentTime_;
  int steps_;

  int targetCount_;
  bool ok_;
  bool finished_;

  Coord* firePoint_;
  float* totalTime_;
  Coord* predictedAll_;

  SimStep* simSteps_;
};