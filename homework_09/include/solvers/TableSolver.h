#pragma once

#include "interfaces/IBallisticSolver.h"
#include "solvers/BallisticTable.h"
#include <string>

class TableSolver : public IBallisticSolver {
public:
    explicit TableSolver(const std::string& tablePath);

    Coord solve(Coord dronePos,
                Coord targetPos,
                float altitude,
                float accelPath,
                float attackSpeed,
                const AmmoParams& ammo,
                float& outH,
                bool& ok) override;

    [[nodiscard]] bool isValid() const;

private:
    BallisticTable table_;
    bool loaded_ = false;
};