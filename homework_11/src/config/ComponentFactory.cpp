#include "config/ComponentFactory.h"
#include "io/GpioOutput.h"
#include "io/UartLink.h"
#include "solvers/AnalyticalSolver.h"
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
std::unique_ptr<IUartLink> createUart(const std::string& device)
{
  return std::make_unique<UartLink>(device);
}

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
std::unique_ptr<IGpioOutput> createGpio(const std::string& chipName, int startLine, int dropLine)
{
  return std::make_unique<GpioOutput>(chipName, startLine, dropLine);
}