#pragma once

#include "interfaces/IConfigLoader.h"
#include <string>

class FileConfigLoader : public IConfigLoader {
public:
  FileConfigLoader(std::string configPath, std::string ammoPath);

  bool load() override;                 // NOLINT(modernize-use-trailing-return-type)
  DroneConfig getConfig() override;     // NOLINT(modernize-use-trailing-return-type)
  AmmoParams getAmmoParams() override;  // NOLINT(modernize-use-trailing-return-type)

private:
  std::string configPath_;
  std::string ammoPath_;
  DroneConfig config_;
  AmmoParams ammoParams_;
};