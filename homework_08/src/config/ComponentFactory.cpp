#include "config/ComponentFactory.h"
#include "solvers/AnalyticalSolver.h"
#include "providers/JsonTargetProvider.h"
#include "config/FileConfigLoader.h"

IBallisticSolver* createSolver(SolverType type)  // NOLINT(modernize-use-trailing-return-type)
{
  switch (type) {
    case SolverType::ANALYTICAL:
      return new AnalyticalSolver();  // NOLINT(cppcoreguidelines-owning-memory) - raw pointer ownership documented via factory contract,
                                      // caller deletes
    default:
      return nullptr;
  }
}

ITargetProvider* createProvider(ProviderType type, const char* path, float arrayTimeStep)  // NOLINT(modernize-use-trailing-return-type)
{
  switch (type) {
    case ProviderType::JSON:
      return new JsonTargetProvider(path, arrayTimeStep);  // NOLINT(cppcoreguidelines-owning-memory)
    default:
      return nullptr;
  }
}

IConfigLoader* createLoader(LoaderType type, const char* configPath, const char* ammoPath)  // NOLINT(modernize-use-trailing-return-type)
{
  switch (type) {
    case LoaderType::FILE:
      return new FileConfigLoader(configPath, ammoPath);  // NOLINT(cppcoreguidelines-owning-memory)
    default:
      return nullptr;
  }
}