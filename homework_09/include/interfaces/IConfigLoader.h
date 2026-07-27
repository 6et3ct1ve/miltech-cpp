#pragma once

#include "Types.h"

class IConfigLoader {
public:
  IConfigLoader() = default;
  IConfigLoader(const IConfigLoader&) = delete;
  IConfigLoader& operator=(const IConfigLoader&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  IConfigLoader(IConfigLoader&&) = delete;
  IConfigLoader& operator=(IConfigLoader&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  virtual bool load() = 0;                             // NOLINT(modernize-use-trailing-return-type)
  virtual DroneConfig getConfig() = 0;                 // NOLINT(modernize-use-trailing-return-type)
  virtual AmmoParams getAmmoParams() = 0;              // NOLINT(modernize-use-trailing-return-type)
  virtual ~IConfigLoader() = default;
};