#pragma once

#include "ballistic_solver.hpp"
#include "target_provider.hpp"
#include "config_loader.hpp"

enum class SolverType { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

IBallisticSolver* createSolver(SolverType type);
ITargetProvider* createProvider(ProviderType type, const char* path, float arrayTimeStep);
IConfigLoader* createLoader(LoaderType type, const char* configPath, const char* ammoPath);