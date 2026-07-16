#pragma once

#include "coord.hpp"

struct AmmoParams {  // NOLINT(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-magic-numbers,
                     // readability-magic-numbers)
  char name[32];     // NOLINT(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-magic-numbers,
                     // readability-magic-numbers)
  float mass, drag, lift;
};
struct DroneConfig {
  Coord startPos;
  float altitude;
  float initialDir;
  float attackSpeed;
  float accelPath;
  char ammoName[32];  // NOLINT(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-magic-numbers,
                      // readability-magic-numbers)
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
  IConfigLoader& operator=(const IConfigLoader&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  IConfigLoader(IConfigLoader&&) = delete;
  IConfigLoader& operator=(IConfigLoader&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  virtual bool load() = 0;                             // NOLINT(modernize-use-trailing-return-type)
  virtual DroneConfig getConfig() = 0;                 // NOLINT(modernize-use-trailing-return-type)
  virtual AmmoParams getAmmoParams() = 0;              // NOLINT(modernize-use-trailing-return-type)
  virtual ~IConfigLoader() = default;
};

class FileConfigLoader : public IConfigLoader {
public:
  FileConfigLoader(const char* configPath, const char* ammoPath);

  bool load() override;                 // NOLINT(modernize-use-trailing-return-type)
  DroneConfig getConfig() override;     // NOLINT(modernize-use-trailing-return-type)
  AmmoParams getAmmoParams() override;  // NOLINT(modernize-use-trailing-return-type)

private:
  const char* configPath_;
  const char* ammoPath_;
  DroneConfig config_;
  AmmoParams ammoParams_;
};