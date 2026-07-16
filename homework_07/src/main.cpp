#include "factory.hpp"
#include "mission_processor.hpp"
#include "logging.hpp"

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

  const SimStep* steps = mission.getSteps();
  for (int i = 0; i < mission.getStepCount(); i++) {
    nlohmann::json step;
    step["position"] = {{"x", steps[i].pos.x}, {"y", steps[i].pos.y}};  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    step["direction"] = steps[i].direction;                             // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    step["state"] = static_cast<int>(steps[i].state);                   // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    step["targetIndex"] = steps[i].targetIdx;                           // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    step["dropPoint"] = {{"x", steps[i].dropPoint.x}, {"y", steps[i].dropPoint.y}};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    step["aimPoint"] = {{"x", steps[i].aimPoint.x}, {"y", steps[i].aimPoint.y}};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    step["predictedTarget"] = {{"x", steps[i].predictedTarget.x}, {"y", steps[i].predictedTarget.y}};
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