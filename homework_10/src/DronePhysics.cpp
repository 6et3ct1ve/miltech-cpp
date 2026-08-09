#include "DronePhysics.h"

#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>

bool DronePhysics::init(const DroneConfig& config)
{
  config_ = config;
  acceleration_ = powf(config.attackSpeed, 2.0f) / (2.0f * config.accelPath);
  if (fabsf(acceleration_) < 1e-9f) {
    std::cerr << "Input format error\n";
    return false;
  }
  timeSecSinceStart_ = 0.0f;
  pos_ = config.startPos;
  dir_ = config.initialDir;
  speed_ = config.attackSpeed;
  mode_ = DroneMode::MOVING;
  return true;
}

void DronePhysics::submit(const DroneCommand& command)
{
  commands_.push(command);
}

void DronePhysics::step()
{
  DroneCommand cmd{};
  bool hasCommand = false;
  while (commands_.tryPop(cmd)) {
    hasCommand = true;
  }

  const std::lock_guard<std::mutex> lock(mutex_);

  if (hasCommand) {
    mode_ = cmd.mode;
    angleSpeed_ = cmd.angleSpeed;
  }

  switch (mode_) {
    case DroneMode::STOPPED:
      break;
    case DroneMode::TURNING:
      dir_ += angleSpeed_ * config_.physicsTimeStep;
      break;
    case DroneMode::ACCELERATING:
      speed_ += acceleration_ * config_.physicsTimeStep;
      if (speed_ > config_.attackSpeed) {
        speed_ = config_.attackSpeed;
      }
      break;
    case DroneMode::DECELERATING:
      speed_ -= acceleration_ * config_.physicsTimeStep;
      if (speed_ < 0.0f) {
        speed_ = 0.0f;
      }
      break;
    case DroneMode::MOVING:
      dir_ += angleSpeed_ * config_.physicsTimeStep;
      break;
  }

  pos_.x += cosf(dir_) * speed_ * config_.physicsTimeStep;
  pos_.y += sinf(dir_) * speed_ * config_.physicsTimeStep;
  timeSecSinceStart_ += config_.physicsTimeStep;
}

DroneTelemetry DronePhysics::getTelemetry() const
{
  const std::lock_guard<std::mutex> lock(mutex_);
  return DroneTelemetry{
    .pos = pos_, .speed = speed_, .direction = dir_, .mode = mode_, .timeSecSinceStart = timeSecSinceStart_, .acceleration = acceleration_};
}

void DronePhysics::run()
{
  ready_ = true;

  while (!started_ && running_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  while (running_) {
    step();
    std::this_thread::sleep_for(std::chrono::duration<float>(config_.physicsTimeStep / config_.timeScale));
  }
}

void DronePhysics::start()
{
  started_ = true;
}

void DronePhysics::stop()
{
  running_ = false;
}

bool DronePhysics::isThreadReady() const
{
  return ready_;
}