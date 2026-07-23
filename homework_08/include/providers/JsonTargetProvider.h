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

  int getTargetCount() override;         // NOLINT(modernize-use-trailing-return-type)
  Target getTarget(int index) override;  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] bool isValid() const;    // NOLINT(modernize-use-trailing-return-type)

private:
  Coord** targets_;
  int targetCount_;
  int timeSteps_;
  float arrayTimeStep_;
  bool ok_;
};