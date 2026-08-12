#pragma once

#include "interfaces/IDroneState.h"

class AcceleratingState final : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(const DroneTelemetry& tlm, DroneContext& ctx, DroneCommand& cmd) override;
  [[nodiscard]] float estimateTimeToStop(const DroneTelemetry& tlm, const DroneContext& ctx) const override;
};