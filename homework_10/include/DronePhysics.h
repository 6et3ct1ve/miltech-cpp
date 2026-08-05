#pragma once

#include "ThreadSafeQueue.h"
#include "Types.h"

#include <mutex>

class DronePhysics {
public:
    DronePhysics() = default;
    ~DronePhysics() = default;

    DronePhysics(const DronePhysics&) = delete;
    DronePhysics& operator=(const DronePhysics&) = delete;  // NOLINT(modernize-use-trailing-return-type)
    DronePhysics(DronePhysics&&) = delete;
    DronePhysics& operator=(DronePhysics&&) = delete;  // NOLINT(modernize-use-trailing-return-type)

    bool init(const DroneConfig& config);  // NOLINT(modernize-use-trailing-return-type)
    void submit(const DroneCommand& command);
    [[nodiscard]] DroneTelemetry getTelemetry() const;  // NOLINT(modernize-use-trailing-return-type)
    void step();

private:
    Coord pos_{};
    float dir_ = 0.0f;
    float speed_ = 0.0f;
    float acceleration_ = 0.0f;
    DroneMode mode_ = DroneMode::STOPPED;
    float timeSecSinceStart_ = 0.0f;
    DroneConfig config_{};
    ThreadSafeQueue<DroneCommand> commands_;
    mutable std::mutex mutex_;
};