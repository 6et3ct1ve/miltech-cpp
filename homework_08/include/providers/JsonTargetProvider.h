#pragma once

#include "interfaces/ITargetProvider.h"

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