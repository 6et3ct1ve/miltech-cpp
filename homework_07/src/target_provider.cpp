#include "target_provider.hpp"
#include "coord.hpp"
#include "json.hpp"

#include <fstream>
#include <iostream>

JsonTargetProvider::JsonTargetProvider(const char* path, float arrayTimeStep)
  : targets_(nullptr)
  , targetCount_(0)
  , timeSteps_(0)
  , arrayTimeStep_(arrayTimeStep)
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

  targetCount_ = targetsCoords["targetCount"];
  timeSteps_ = targetsCoords["timeSteps"];
  if (targetCount_ == 0 || timeSteps_ == 0) {
    std::cerr << "Coord target format error\n";
    return;
  }

  targets_ = new Coord*[targetCount_];
  for (int i = 0; i < targetCount_; i++) {
    targets_[i] = new Coord[timeSteps_];
    for (int j = 0; j < timeSteps_; j++) {
      targets_[i][j].x = targetsCoords["targets"][i]["positions"][j]["x"];
      targets_[i][j].y = targetsCoords["targets"][i]["positions"][j]["y"];
    }
  }
  ok_ = true;
}

JsonTargetProvider::~JsonTargetProvider()
{
  if (targets_ != nullptr) {
    for (int i = 0; i < targetCount_; i++) {
      delete[] targets_[i];
      targets_[i] = nullptr;
    }
    delete[] targets_;
    targets_ = nullptr;
  }
}

int JsonTargetProvider::getTargetCount()
{
  return targetCount_;
}

Target JsonTargetProvider::getTarget(int index)
{
  return Target{targets_[index], timeSteps_, arrayTimeStep_};
}

bool JsonTargetProvider::isValid() const
{
  return ok_;
}