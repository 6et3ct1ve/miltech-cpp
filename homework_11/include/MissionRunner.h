#pragma once

#include "DroneController.h"
#include "MissionProcessor.h"
#include "TargetTracker.h"
#include "Types.h"
#include "interfaces/IGpioOutput.h"
#include "interfaces/ILinkHandler.h"
#include "interfaces/IUartLink.h"

class MissionRunner : public ILinkHandler {
public:
  MissionRunner(MissionProcessor& mission, DroneController& controller, TargetTracker& tracker, IUartLink& link, IGpioOutput& gpio);
  MissionRunner(const MissionRunner&) = delete;
  MissionRunner& operator=(const MissionRunner&) = delete;
  MissionRunner(MissionRunner&&) = delete;
  MissionRunner& operator=(MissionRunner&&) = delete;
  ~MissionRunner() override = default;

  void onTelemetry(const DroneTelemetry& tlm) override;
  void onTarget(int id, Coord pos) override;
  void onAmmo(const AmmoParams& ammo, float hitRadius, int nTargets) override;
  void onConfig(const DroneConfig& config) override;

  [[nodiscard]] bool isDropped() const;  // NOLINT(modernize-use-trailing-return-type)

private:
  void tryConfigure();

  MissionProcessor& mission_;
  DroneController& controller_;
  TargetTracker& tracker_;
  IUartLink& link_;
  IGpioOutput& gpio_;

  DroneConfig config_{};
  AmmoParams ammo_{};
  int nTargets_ = 0;
  float lastTime_ = 0.0f;
  bool gotAmmo_ = false;
  bool gotConfig_ = false;
  bool dropped_ = false;
};