#pragma once

#include "interfaces/IDroneState.h"

class TurningState : public IDroneState {
public:
  std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
  [[nodiscard]] const char* name() const override;
  [[nodiscard]] float estimateTimeToStop(const DroneContext& ctx) const override;
};