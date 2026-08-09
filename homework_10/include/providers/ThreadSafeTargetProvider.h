#pragma once

#include "interfaces/ITargetProvider.h"
#include "interfaces/IThreadedComponent.h"
#include <cstddef>
#include <string>
#include <vector>
#include <mutex>
#include <array>
#include <atomic>

class ThreadSafeTargetProvider : public ITargetProvider, public IThreadedComponent {
public:
  ThreadSafeTargetProvider(const std::string& path, float arrayTimeStep, float timeScale);
  ThreadSafeTargetProvider(const ThreadSafeTargetProvider&) = delete;
  ThreadSafeTargetProvider& operator=(const ThreadSafeTargetProvider&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  ThreadSafeTargetProvider(ThreadSafeTargetProvider&&) = delete;
  ThreadSafeTargetProvider& operator=(ThreadSafeTargetProvider&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

  [[nodiscard]] int getTargetCount() const override;         // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] Target getTarget(int index) const override;  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] bool isValid() const;                        // NOLINT(modernize-use-trailing-return-type)
  void step(float dt);
  void run() override;
  void start() override;
  void stop() override;
  [[nodiscard]] bool isThreadReady() const override;

private:
  void updateCurrent();
  std::vector<std::vector<Coord>> targets_;
  std::array<std::vector<Target>, 2> buffers_;
  size_t activeBuffer_ = 0;
  size_t timeSteps_ = 0;
  size_t nodeIndex_ = 0;
  float arrayTimeStep_ = 0.0f;
  float accumulator_ = 0.0f;
  float timeScale_ = 1.0f;
  bool ok_ = false;
  mutable std::mutex mutex_;
  std::atomic<bool> ready_{false};
  std::atomic<bool> started_{false};
  std::atomic<bool> running_{true};
};