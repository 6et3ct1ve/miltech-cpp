#pragma once

#include "Types.h"

class ILinkHandler {
 public:
 ILinkHandler() = default;
 ILinkHandler(const ILinkHandler&) = delete;
 ILinkHandler& operator=(const ILinkHandler&) = delete;
 ILinkHandler(ILinkHandler&&) = delete;
 ILinkHandler& operator=(ILinkHandler&&) = delete;
 virtual ~ILinkHandler() = default;

  virtual void onTelemetry(const DroneTelemetry& tlm) = 0;
  virtual void onTarget(int id, Coord pos) = 0;
  virtual void onAmmo(const AmmoParams& ammo, float hitRadius, int nTargets) = 0;
  virtual void onConfig(const DroneConfig& config) = 0;
};