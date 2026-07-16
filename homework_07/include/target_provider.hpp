#pragma once

#include "coord.hpp"

struct Target {
  Coord* positions;
  int timeSteps;
  float arrayTimeStep;
};

class ITargetProvider {
public:
  virtual int getTargetCount() = 0;
  virtual Target getTarget(int index) = 0;
  virtual ~ITargetProvider() {}
};

class JsonTargetProvider : public ITargetProvider {
public:
  JsonTargetProvider(const char* path, float arrayTimeStep);
  ~JsonTargetProvider() override;

  int getTargetCount() override;
  Target getTarget(int index) override;

private:
  Coord** targets_;
  int targetCount_;
  int timeSteps_;
  float arrayTimeStep_;
};