#pragma once

#include "interfaces/IBallisticSolver.h"
#include "interfaces/ITargetProvider.h"
#include "interfaces/IConfigLoader.h"
#include <string>

enum class SolverType { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

// NOLINTBEGIN(modernize-use-trailing-return-type)
IBallisticSolver* createSolver(SolverType type);
ITargetProvider* createProvider(ProviderType type, const std::string& path, float arrayTimeStep);
IConfigLoader* createLoader(LoaderType type, const std::string& configPath, const std::string& ammoPath);
// NOLINTEND(modernize-use-trailing-return-type)