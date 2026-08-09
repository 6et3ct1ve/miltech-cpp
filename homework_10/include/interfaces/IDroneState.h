#pragma once

#include "Types.h"

#include <memory>

class IDroneState {
public:
  IDroneState() = default;
  IDroneState(const IDroneState&) = delete;
  IDroneState& operator=(const IDroneState&) = delete;
  IDroneState(IDroneState&&) = delete;
  IDroneState& operator=(IDroneState&&) = delete;
  virtual ~IDroneState() = default;

  virtual std::unique_ptr<IDroneState> execute(const DroneTelemetry& tlm, DroneContext& ctx, DroneCommand& cmd) = 0;
  [[nodiscard]] virtual float estimateTimeToStop(const DroneTelemetry& tlm, const DroneContext& ctx) const = 0;
};