#pragma once

#include "Types.h"

class DroneController {
public:
  DroneController() = default;
  DroneController(const DroneController&) = delete;
  DroneController& operator=(const DroneController&) = delete;
  DroneController(DroneController&&) = delete;
  DroneController& operator=(DroneController&&) = delete;

  void configure(const DroneConfig& config);
  [[nodiscard]] ControlOutput toControl(const DroneCommand& cmd) const;  // NOLINT(modernize-use-trailing-return-type)

private:
  float angularSpeed_ = 0.0f;
};