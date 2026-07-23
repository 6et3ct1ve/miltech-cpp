#pragma once

#include "interfaces/IBallisticSolver.h"
#include "interfaces/ITargetProvider.h"
#include "interfaces/IConfigLoader.h"

enum class SolverType { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

IBallisticSolver* createSolver(SolverType type);                                             // NOLINT(modernize-use-trailing-return-type)
ITargetProvider* createProvider(ProviderType type, const char* path, float arrayTimeStep);   // NOLINT(modernize-use-trailing-return-type)
IConfigLoader* createLoader(LoaderType type, const char* configPath, const char* ammoPath);  // NOLINT(modernize-use-trailing-return-type)