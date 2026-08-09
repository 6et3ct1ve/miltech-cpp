#include "config/ComponentFactory.h"
#include "DronePhysics.h"
#include "providers/ThreadSafeTargetProvider.h"
#include "MissionProcessor.h"
#include "Logging.h"

#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "json.hpp"

int main(int argc, char* argv[])  // NOLINT(modernize-use-trailing-return-type)
{
  const char* configPath = (argc > 1) ? argv[1] : "config.json";    // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* ammoPath = (argc > 2) ? argv[2] : "ammo.json";        // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* targetsPath = (argc > 3) ? argv[3] : "targets.json";  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  const char* solverType = (argc > 4) ? argv[4] : "analytical";
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  const char* tablePath = (argc > 5) ? argv[5] : "ballistic_table.txt";

  auto loader = createLoader(LoaderType::FILE, configPath, ammoPath);
  if (!loader->load()) {
    return 1;
  }

  DroneConfig config = loader->getConfig();

  auto provider = std::make_unique<ThreadSafeTargetProvider>(targetsPath, config.arrayTimeStep, config.timeScale);
  if (!provider->isValid()) {
    std::cerr << "Provider init failed\n";
    return 1;
  }

  std::unique_ptr<IBallisticSolver> solver;
  if (std::string(solverType) == "table") {
    solver = createSolver(SolverType::TABLE, tablePath);
  }
  else {
    solver = createSolver(SolverType::ANALYTICAL);
  }

  auto physics = std::make_unique<DronePhysics>();

  MissionProcessor mission(provider.get(), std::move(solver), std::move(loader), physics.get());
  if (!mission.init(configPath)) {
    std::cerr << "Mission init failed\n";
    return 1;
  }
  std::thread providerThread(&ThreadSafeTargetProvider::run, provider.get());
  std::thread physicsThread(&DronePhysics::run, physics.get());
  std::thread missionThread(&MissionProcessor::run, &mission);

  while (!provider->isThreadReady() || !physics->isThreadReady() || !mission.isThreadReady()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  provider->start();
  physics->start();
  mission.start();

  missionThread.join();

  physics->stop();
  provider->stop();
  physicsThread.join();
  providerThread.join();

  LOG("Simulation complete. Steps: " << mission.getStepCount());

  nlohmann::json out;
  out["totalSteps"] = mission.getStepCount();
  out["steps"] = nlohmann::json::array();

  for (const auto& s : mission.getSteps()) {
    nlohmann::json step;
    step["position"] = {{"x", s.pos.x}, {"y", s.pos.y}};
    step["direction"] = s.direction;
    step["state"] = s.state;
    step["targetIndex"] = s.targetIdx;
    step["dropPoint"] = {{"x", s.dropPoint.x}, {"y", s.dropPoint.y}};
    step["aimPoint"] = {{"x", s.aimPoint.x}, {"y", s.aimPoint.y}};
    step["predictedTarget"] = {{"x", s.predictedTarget.x}, {"y", s.predictedTarget.y}};
    step["timeSecSinceStart"] = s.timeSecSinceStart;
    out["steps"].push_back(step);
  }

  std::ofstream output("simulation.json");
  if (!output.is_open()) {
    std::cerr << "Unable to write output\n";
    return 1;
  }
  output << out.dump(2);
  output.close();

  std::cout << "Success\n";
  return 0;
}