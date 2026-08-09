#include "providers/ThreadSafeTargetProvider.h"
#include "json.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

ThreadSafeTargetProvider::ThreadSafeTargetProvider(const std::string& path, float arrayTimeStep, float timeScale)
  : arrayTimeStep_(arrayTimeStep)
  , timeScale_(timeScale > 0.0f ? timeScale : 1.0f)
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

  const int targetCount = targetsCoords["targetCount"];
  const int timeSteps = targetsCoords["timeSteps"];
  if (targetCount == 0 || timeSteps == 0 || arrayTimeStep <= 0.0f) {
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
  timeSteps_ = static_cast<size_t>(timeSteps);
  buffers_[0].resize(targetCount);
  buffers_[1].resize(targetCount);
  ok_ = true;
  updateCurrent();
}

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
void ThreadSafeTargetProvider::updateCurrent()
{
  const size_t next = (nodeIndex_ + 1) % timeSteps_;
  const size_t back = 1 - activeBuffer_;

  for (size_t i = 0; i < buffers_[back].size(); i++) {
    buffers_[back][i].pos = targets_[i][nodeIndex_];
    buffers_[back][i].velocity = (targets_[i][next] - targets_[i][nodeIndex_]) / arrayTimeStep_;
  }

  const std::lock_guard<std::mutex> lock(mutex_);
  activeBuffer_ = back;
}

int ThreadSafeTargetProvider::getTargetCount() const  // NOLINT(modernize-use-trailing-return-type)
{
  return static_cast<int>(targets_.size());
}

Target ThreadSafeTargetProvider::getTarget(int index) const  // NOLINT(modernize-use-trailing-return-type)
{
  const std::lock_guard<std::mutex> lock(mutex_);
  return buffers_[activeBuffer_][static_cast<size_t>(index)];
}

// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)

bool ThreadSafeTargetProvider::isValid() const  // NOLINT(modernize-use-trailing-return-type)
{
  return ok_;
}

void ThreadSafeTargetProvider::step(float dt)
{
  if (!ok_) {
    return;
  }
  accumulator_ += dt;
  while (accumulator_ >= arrayTimeStep_) {
    nodeIndex_ = (nodeIndex_ + 1) % timeSteps_;
    accumulator_ -= arrayTimeStep_;
    updateCurrent();
  }
}

void ThreadSafeTargetProvider::run()
{
  ready_ = true;

  while (!started_ && running_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  while (running_) {
    step(arrayTimeStep_);
    std::this_thread::sleep_for(std::chrono::duration<float>(arrayTimeStep_ / timeScale_));
  }
}

void ThreadSafeTargetProvider::start()
{
  started_ = true;
}

void ThreadSafeTargetProvider::stop()
{
  running_ = false;
}

bool ThreadSafeTargetProvider::isThreadReady() const  // NOLINT(modernize-use-trailing-return-type)
{
  return ready_;
}