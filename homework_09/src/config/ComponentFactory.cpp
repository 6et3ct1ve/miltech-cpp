#include "config/ComponentFactory.h"
#include "solvers/AnalyticalSolver.h"
#include "providers/JsonTargetProvider.h"
#include "config/FileConfigLoader.h"
#include "solvers/TableSolver.h"

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
std::unique_ptr<IBallisticSolver> createSolver(SolverType type, const std::string& tablePath)
{
  switch (type) {
    case SolverType::ANALYTICAL:
      return std::make_unique<AnalyticalSolver>();
    case SolverType::TABLE:
      return std::make_unique<TableSolver>(tablePath);
    default:
      return nullptr;
  }
}

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
std::unique_ptr<ITargetProvider> createProvider(ProviderType type, const std::string& path, float arrayTimeStep)
{
  switch (type) {
    case ProviderType::JSON:
      return std::make_unique<JsonTargetProvider>(path, arrayTimeStep);
    default:
      return nullptr;
  }
}

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
std::unique_ptr<IConfigLoader> createLoader(LoaderType type, const std::string& configPath, const std::string& ammoPath)
{
  switch (type) {
    case LoaderType::FILE:
      return std::make_unique<FileConfigLoader>(configPath, ammoPath);
    default:
      return nullptr;
  }
}