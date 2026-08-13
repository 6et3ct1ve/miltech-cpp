#pragma once

#include "interfaces/IBallisticSolver.h"
#include "interfaces/IGpioOutput.h"
#include "interfaces/IUartLink.h"

#include <memory>
#include <string>

enum class SolverType { ANALYTICAL, TABLE };

// NOLINTBEGIN(modernize-use-trailing-return-type)
std::unique_ptr<IBallisticSolver> createSolver(SolverType type, const std::string& tablePath = "");
std::unique_ptr<IUartLink> createUart(const std::string& device);
std::unique_ptr<IGpioOutput> createGpio(const std::string& chipName, int startLine, int dropLine);
// NOLINTEND(modernize-use-trailing-return-type)