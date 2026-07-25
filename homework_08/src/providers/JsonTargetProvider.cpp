#include "providers/JsonTargetProvider.h"
#include "Types.h"
#include "json.hpp"

#include <fstream>
#include <iostream>

JsonTargetProvider::JsonTargetProvider(const std::string& path, float arrayTimeStep)
  : arrayTimeStep_(arrayTimeStep)
  , ok_(false)
{
  std::ifstream targetsFile(path);
  if (!targetsFile.is_open()) {
    std::cerr << "Unable to read targets\n";
    return;
  }

  nlohmann::json targetsCoords;
  targetsFile >> targetsCoords;
  if (targetsFile.fail()) {
    std::cerr << "Targets format error\n";
    return;
  }
  targetsFile.close();

  int targetCount = targetsCoords["targetCount"];
  int timeSteps = targetsCoords["timeSteps"];
  if (targetCount == 0 || timeSteps == 0) {
    std::cerr << "Coord target format error\n";
    return;
  }

  targets_.resize(targetCount);
  for (int i = 0; i < targetCount; i++) {
    targets_[i].resize(timeSteps);
    for (int j = 0; j < timeSteps; j++) {
      targets_[i][j].x = targetsCoords["targets"][i]["positions"][j]["x"];
      targets_[i][j].y = targetsCoords["targets"][i]["positions"][j]["y"];
    }
  }
  ok_ = true;
}

int JsonTargetProvider::getTargetCount()  // NOLINT(modernize-use-trailing-return-type)
{
  return static_cast<int>(targets_.size());
}

Target JsonTargetProvider::getTarget(int index)  // NOLINT(modernize-use-trailing-return-type)
{
  return Target{targets_[index], arrayTimeStep_};
}

bool JsonTargetProvider::isValid() const  // NOLINT(modernize-use-trailing-return-type)
{
  return ok_;
}