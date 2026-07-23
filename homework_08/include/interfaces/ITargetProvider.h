#pragma once

#include "coord.hpp"

struct Target {
  Coord* positions;
  int timeSteps;
  float arrayTimeStep;
};

class ITargetProvider {
public:
  ITargetProvider() = default;
  ITargetProvider(const ITargetProvider&) = delete;
  ITargetProvider& operator=(const ITargetProvider&) = delete;
  ITargetProvider(ITargetProvider&&) = delete;
  ITargetProvider& operator=(ITargetProvider&&) = delete;
  virtual int getTargetCount() = 0;
  virtual Target getTarget(int index) = 0;
  virtual ~ITargetProvider() = default;
};

class JsonTargetProvider : public ITargetProvider {
public:
  JsonTargetProvider(const char* path, float arrayTimeStep);
  JsonTargetProvider(const JsonTargetProvider&) = delete;
  JsonTargetProvider& operator=(const JsonTargetProvider&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  JsonTargetProvider(JsonTargetProvider&&) = delete;
  JsonTargetProvider& operator=(JsonTargetProvider&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  ~JsonTargetProvider() override;

  int getTargetCount() override;
  Target getTarget(int index) override;
  bool isValid() const;

private:
  Coord** targets_;
  int targetCount_;
  int timeSteps_;
  float arrayTimeStep_;
  bool ok_;
};