#include "factory.hpp"

IBallisticSolver* createSolver(SolverType type)
{
  switch (type) {
    case SolverType::ANALYTICAL:
      return new AnalyticalSolver();
    default:
      return nullptr;
  }
}

ITargetProvider* createProvider(ProviderType type, const char* path, float arrayTimeStep)
{
  switch (type) {
    case ProviderType::JSON:
      return new JsonTargetProvider(path, arrayTimeStep);
    default:
      return nullptr;
  }
}

IConfigLoader* createLoader(LoaderType type, const char* configPath, const char* ammoPath)
{
  switch (type) {
    case LoaderType::FILE:
      return new FileConfigLoader(configPath, ammoPath);
    default:
      return nullptr;
  }
}