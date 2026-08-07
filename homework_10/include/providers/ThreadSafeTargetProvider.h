#pragma once

#include "interfaces/ITargetProvider.h"
#include <cstddef>
#include <string>
#include <vector>
#include <mutex>
#include <array>

class ThreadSafeTargetProvider : public ITargetProvider {
public:
  ThreadSafeTargetProvider(const std::string& path, float arrayTimeStep);
  ThreadSafeTargetProvider(const ThreadSafeTargetProvider&) = delete;
  ThreadSafeTargetProvider& operator=(const ThreadSafeTargetProvider&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  ThreadSafeTargetProvider(ThreadSafeTargetProvider&&) = delete;
  ThreadSafeTargetProvider& operator=(ThreadSafeTargetProvider&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

  [[nodiscard]] int getTargetCount() const override;         // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] Target getTarget(int index) const override;  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] bool isValid() const;                        // NOLINT(modernize-use-trailing-return-type)
  void step(float dt);

private:
  std::vector<std::vector<Coord>> targets_;
  std::array<std::vector<Target>, 2> buffers_;
  size_t activeBuffer_ = 0;
  size_t timeSteps_ = 0;
  float arrayTimeStep_ = 0.0f;
  float accumulator_ = 0.0f;
  size_t nodeIndex_ = 0;
  bool ok_ = false;
  mutable std::mutex mutex_;
  void updateCurrent();
};