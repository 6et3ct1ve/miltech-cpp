#include "config/FileConfigLoader.h"
#include "json.hpp"
#include "Logging.h"

#include <fstream>
#include <cmath>
#include <iostream>

FileConfigLoader::FileConfigLoader(std::string configPath, std::string ammoPath)
  : configPath_(std::move(configPath))
  , ammoPath_(std::move(ammoPath))
  , config_{}
  , ammoParams_{}
{
}

bool FileConfigLoader::load()  // NOLINT(modernize-use-trailing-return-type)
{
  std::ifstream configFile(configPath_);
  if (!configFile.is_open()) {
    std::cerr << "Unable to read drone config\n";
    return false;
  }

  nlohmann::json droneConfig;
  configFile >> droneConfig;
  if (configFile.fail()) {
    std::cerr << "Input format error\n";
    return false;
  }
  configFile.close();

  config_.startPos.x = droneConfig["drone"]["position"]["x"];
  config_.startPos.y = droneConfig["drone"]["position"]["y"];
  config_.altitude = droneConfig["drone"]["altitude"];
  config_.initialDir = droneConfig["drone"]["initialDirection"];
  config_.attackSpeed = droneConfig["drone"]["attackSpeed"];
  config_.accelPath = droneConfig["drone"]["accelerationPath"];
  config_.angularSpeed = droneConfig["drone"]["angularSpeed"];
  config_.turnThreshold = droneConfig["drone"]["turnThreshold"];
  config_.ammoName = droneConfig["ammo"].get<std::string>();
  config_.simTimeStep = droneConfig["simulation"]["timeStep"];
  config_.hitRadius = droneConfig["simulation"]["hitRadius"];
  config_.arrayTimeStep = droneConfig["targetArrayTimeStep"];

  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  if (fabsf(config_.arrayTimeStep) < 1e-9f || fabsf(config_.attackSpeed) < 1e-9f || fabsf(config_.simTimeStep) < 1e-9f ||
      fabsf(config_.accelPath) < 1e-9f || fabsf(config_.angularSpeed) < 1e-9f) {
    std::cerr << "Input format error\n";
    return false;
  }
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  std::ifstream ammoFile(ammoPath_);
  if (!ammoFile.is_open()) {
    std::cerr << "Unable to read ammo species\n";
    return false;
  }
  nlohmann::json ammoParams;
  ammoFile >> ammoParams;
  if (ammoFile.fail()) {
    std::cerr << "Ammo format error\n";
    return false;
  }
  ammoFile.close();

  int ammoCount = static_cast<int>(ammoParams.size());

  if (ammoCount == 0) {
    std::cerr << "Empty ammo list\n";
    return false;
  }

  bool found = false;
  for (int i = 0; i < ammoCount; i++) {
    std::string name = ammoParams[i]["name"].get<std::string>();
    if (config_.ammoName == name) {
      ammoParams_.name = name;
      ammoParams_.mass = ammoParams[i]["mass"];
      ammoParams_.drag = ammoParams[i]["drag"];
      ammoParams_.lift = ammoParams[i]["lift"];
      found = true;
      LOG("Ammo found>" << ammoParams_.name);
      break;
    }
  }

  if (!found) {
    std::cerr << "Wrong ammo name\n";
    return false;
  }

  return true;
}

DroneConfig FileConfigLoader::getConfig()  // NOLINT(modernize-use-trailing-return-type)
{
  return config_;
}

AmmoParams FileConfigLoader::getAmmoParams()  // NOLINT(modernize-use-trailing-return-type)
{
  return ammoParams_;
}