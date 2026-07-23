#pragma once

#include "interfaces/IConfigLoader.h"

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