#pragma once

#include "interfaces/ITargetProvider.h"
#include <string>
#include <vector>

class JsonTargetProvider : public ITargetProvider {
public:
  JsonTargetProvider(const std::string& path, float arrayTimeStep);
  JsonTargetProvider(const JsonTargetProvider&) = delete;
  JsonTargetProvider& operator=(const JsonTargetProvider&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  JsonTargetProvider(JsonTargetProvider&&) = delete;
  JsonTargetProvider& operator=(JsonTargetProvider&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

  int getTargetCount() override;         // NOLINT(modernize-use-trailing-return-type)
  Target getTarget(int index) override;  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] bool isValid() const;    // NOLINT(modernize-use-trailing-return-type)

private:
  std::vector<std::vector<Coord>> targets_;
  float arrayTimeStep_;
  bool ok_;
};