#include "config/ComponentFactory.h"
#include "MissionProcessor.h"
#include "Logging.h"

#include <fstream>
#include <iostream>
#include "json.hpp"

int main(int argc, char* argv[])  // NOLINT(modernize-use-trailing-return-type)
{
  const char* configPath = (argc > 1) ? argv[1] : "config.json";    // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* ammoPath = (argc > 2) ? argv[2] : "ammo.json";        // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* targetsPath = (argc > 3) ? argv[3] : "targets.json";  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

  IConfigLoader* loader = createLoader(LoaderType::FILE, configPath, ammoPath);
  if (!loader->load()) {
    delete loader;  // NOLINT(cppcoreguidelines-owning-memory)
    return 1;
  }

  DroneConfig config = loader->getConfig();

  ITargetProvider* provider = createProvider(ProviderType::JSON, targetsPath, config.arrayTimeStep);
  IBallisticSolver* solver = createSolver(SolverType::ANALYTICAL);

  MissionProcessor mission(provider, solver, loader);
  if (!mission.init(configPath)) {
    std::cerr << "Mission init failed\n";
    delete provider;  // NOLINT(cppcoreguidelines-owning-memory)
    delete solver;    // NOLINT(cppcoreguidelines-owning-memory)
    delete loader;    // NOLINT(cppcoreguidelines-owning-memory)
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
    step["state"] = static_cast<int>(s.state);
    step["targetIndex"] = s.targetIdx;
    step["dropPoint"] = {{"x", s.dropPoint.x}, {"y", s.dropPoint.y}};
    step["aimPoint"] = {{"x", s.aimPoint.x}, {"y", s.aimPoint.y}};
    step["predictedTarget"] = {{"x", s.predictedTarget.x}, {"y", s.predictedTarget.y}};
    out["steps"].push_back(step);
  }

  std::ofstream output("simulation.json");
  if (!output.is_open()) {
    std::cerr << "Unable to write output\n";
    delete provider;  // NOLINT(cppcoreguidelines-owning-memory)
    delete solver;    // NOLINT(cppcoreguidelines-owning-memory)
    delete loader;    // NOLINT(cppcoreguidelines-owning-memory)
    return 1;
  }
  output << out.dump(2);
  output.close();

  delete provider;  // NOLINT(cppcoreguidelines-owning-memory)
  delete solver;    // NOLINT(cppcoreguidelines-owning-memory)
  delete loader;    // NOLINT(cppcoreguidelines-owning-memory)

  std::cout << "Success\n";
  return 0;
}