#pragma once

#include "coord.hpp"

struct AmmoParams {
  char name[32];
  float mass, drag, lift;
};
struct DroneConfig {
  Coord startPos;
  float altitude;
  float initialDir;
  float attackSpeed;
  float accelPath;
  char ammoName[32];
  float arrayTimeStep;
  float simTimeStep;
  float hitRadius;
  float angularSpeed;
  float turnThreshold;
};

class IConfigLoader {
public:
  IConfigLoader() = default;
  IConfigLoader(const IConfigLoader&) = delete;
  IConfigLoader& operator=(const IConfigLoader&) = delete;
  virtual bool load() = 0;
  virtual DroneConfig getConfig() = 0;
  virtual AmmoParams getAmmoParams() = 0;
  virtual ~IConfigLoader() = default;
};

class FileConfigLoader : public IConfigLoader {
public:
  FileConfigLoader(const char* configPath, const char* ammoPath);

  bool load() override;
  DroneConfig getConfig() override;
  AmmoParams getAmmoParams() override;

private:
  const char* configPath_;
  const char* ammoPath_;
  DroneConfig config_;
  AmmoParams ammoParams_;
};