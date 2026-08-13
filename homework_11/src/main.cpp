#include "DroneController.h"
#include "Logging.h"
#include "MissionProcessor.h"
#include "MissionRunner.h"
#include "TargetTracker.h"
#include "config/ComponentFactory.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {

constexpr int kDefaultStartLine = 24;
constexpr int kDefaultDropLine = 23;
constexpr int kPollIntervalMs = 2;

}  // namespace

int main(int argc, char* argv[])  // NOLINT(modernize-use-trailing-return-type)
{
  std::string device = "/tmp/ttyA";
  std::string chipName = "gpiochip1";
  std::string tablePath = "ballistic_table.txt";
  bool useTable = false;
  int startLine = kDefaultStartLine;
  int dropLine = kDefaultDropLine;

  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  for (int i = 1; i + 1 < argc; i += 2) {
    const std::string key = argv[i];
    const std::string value = argv[i + 1];
    if (key == "--uart") {
      device = value;
    }
    else if (key == "--gpiochip") {
      chipName = value;
    }
    else if (key == "--start-line") {
      startLine = std::stoi(value);
    }
    else if (key == "--drop-line") {
      dropLine = std::stoi(value);
    }
    else if (key == "--solver") {
      useTable = (value == "table");
    }
    else if (key == "--table") {
      tablePath = value;
    }
  }
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

  auto link = createUart(device);
  if (!link->isValid()) {
    std::cerr << "UART init failed\n";
    return 1;
  }

  auto gpio = createGpio(chipName, startLine, dropLine);
  if (!gpio->isValid()) {
    std::cerr << "GPIO init failed\n";
    return 1;
  }

  auto solver = useTable ? createSolver(SolverType::TABLE, tablePath) : createSolver(SolverType::ANALYTICAL);

  MissionProcessor mission(std::move(solver));
  DroneController controller;
  TargetTracker tracker;
  MissionRunner runner(mission, controller, tracker, *link, *gpio);

  gpio->raiseStart();
  LOG("Started, waiting for telemetry");

  while (!runner.isDropped()) {
    link->poll(runner);
    std::this_thread::sleep_for(std::chrono::milliseconds(kPollIntervalMs));
  }

  LOG("Drop signal sent");
  return 0;
}