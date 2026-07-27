#pragma once

#include "interfaces/IBallisticSolver.h"
#include "interfaces/ITargetProvider.h"
#include "interfaces/IConfigLoader.h"
#include <memory>
#include <string>

enum class SolverType { ANALYTICAL, TABLE };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

// NOLINTBEGIN(modernize-use-trailing-return-type)
std::unique_ptr<IBallisticSolver> createSolver(SolverType type);
std::unique_ptr<ITargetProvider> createProvider(ProviderType type, const std::string& path, float arrayTimeStep);
std::unique_ptr<IConfigLoader> createLoader(LoaderType type, const std::string& configPath, const std::string& ammoPath);
std::unique_ptr<IBallisticSolver> createSolver(SolverType type, const std::string& tablePath = "");
// NOLINTEND(modernize-use-trailing-return-type)