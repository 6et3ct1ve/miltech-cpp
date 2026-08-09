#pragma once

#include "interfaces/IThreadedComponent.h"
#include "ThreadSafeQueue.h"
#include "Types.h"

#include <mutex>
#include <atomic>

class DronePhysics : public IThreadedComponent {
public:
  DronePhysics() = default;

  DronePhysics(const DronePhysics&) = delete;
  DronePhysics& operator=(const DronePhysics&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  DronePhysics(DronePhysics&&) = delete;
  DronePhysics& operator=(DronePhysics&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

  bool init(const DroneConfig& config);  // NOLINT(modernize-use-trailing-return-type)
  void submit(const DroneCommand& command);
  [[nodiscard]] DroneTelemetry getTelemetry() const;  // NOLINT(modernize-use-trailing-return-type)
  void step();

  void run() override;
  void start() override;
  void stop() override;
  [[nodiscard]] bool isThreadReady() const override;

private:
  Coord pos_{};
  float dir_ = 0.0f;
  float speed_ = 0.0f;
  float acceleration_ = 0.0f;
  float timeSecSinceStart_ = 0.0f;
  float angleSpeed_ = 0.0f;
  DroneMode mode_ = DroneMode::STOPPED;
  DroneConfig config_{};
  ThreadSafeQueue<DroneCommand> commands_;
  std::atomic<bool> ready_{false};
  std::atomic<bool> started_{false};
  std::atomic<bool> running_{true};
  mutable std::mutex mutex_;
};