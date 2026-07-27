#include "config/ComponentFactory.h"
#include "MissionProcessor.h"
#include "Logging.h"

#include <fstream>
#include <iostream>
#include <string>
#include "json.hpp"

int main(int argc, char* argv[])  // NOLINT(modernize-use-trailing-return-type)
{
  const char* configPath = (argc > 1) ? argv[1] : "config.json";         // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* ammoPath = (argc > 2) ? argv[2] : "ammo.json";             // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* targetsPath = (argc > 3) ? argv[3] : "targets.json";       // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* solverType = (argc > 4) ? argv[4] : "analytical";          // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* tablePath = (argc > 5) ? argv[5] : "ballistic_table.txt";  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

  auto loader = createLoader(LoaderType::FILE, configPath, ammoPath);
  if (!loader->load()) {
    return 1;
  }

  DroneConfig config = loader->getConfig();

  auto provider = createProvider(ProviderType::JSON, targetsPath, config.arrayTimeStep);

  std::unique_ptr<IBallisticSolver> solver;
  if (std::string(solverType) == "table") {
    solver = createSolver(SolverType::TABLE, tablePath);
  }
  else {
    solver = createSolver(SolverType::ANALYTICAL);
  }

  MissionProcessor mission(std::move(provider), std::move(solver), std::move(loader));
  if (!mission.init(configPath)) {
    std::cerr << "Mission init failed\n";
    return 1;
  }

  while (mission.hasNext()) {
    mission.step();
  }

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