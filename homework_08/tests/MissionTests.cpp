#include "Types.h"
#include "solvers/AnalyticalSolver.h"
#include "config/FileConfigLoader.h"
#include "providers/JsonTargetProvider.h"
#include <gtest/gtest.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(Hw08Coord, Addition)
{
  Coord a{1.0f, 2.0f};
  Coord b{3.0f, 4.0f};
  Coord result = a + b;
  EXPECT_NEAR(result.x, 4.0f, 0.001f);
  EXPECT_NEAR(result.y, 6.0f, 0.001f);
}

TEST(Hw08Coord, Subtraction)
{
  Coord a{5.0f, 5.0f};
  Coord b{2.0f, 1.0f};
  Coord result = a - b;
  EXPECT_NEAR(result.x, 3.0f, 0.001f);
  EXPECT_NEAR(result.y, 4.0f, 0.001f);
}

TEST(Hw08Coord, ScalarMultiplication)
{
  Coord a{2.0f, 3.0f};
  Coord result = a * 2.0f;
  EXPECT_NEAR(result.x, 4.0f, 0.001f);
  EXPECT_NEAR(result.y, 6.0f, 0.001f);
}

TEST(Hw08Coord, ScalarDivision)
{
  Coord a{6.0f, 8.0f};
  Coord result = a / 2.0f;
  EXPECT_NEAR(result.x, 3.0f, 0.001f);
  EXPECT_NEAR(result.y, 4.0f, 0.001f);
}

TEST(Hw08Coord, DivisionByZeroReturnsZero)
{
  Coord a{6.0f, 8.0f};
  Coord result = a / 0.0f;
  EXPECT_NEAR(result.x, 0.0f, 0.001f);
  EXPECT_NEAR(result.y, 0.0f, 0.001f);
}

TEST(Hw08Coord, Equality)
{
  Coord a{1.0f, 2.0f};
  Coord b{1.0f, 2.0f};
  EXPECT_TRUE(a == b);
}

TEST(Hw08AnalyticalSolver, ComputesKnownDropPoint)
{
  AnalyticalSolver solver;
  Coord dronePos{0.0f, 0.0f};
  Coord targetPos{300.0f, 200.0f};
  float altitude = 100.0f;
  float accelPath = 50.0f;
  float attackSpeed = 15.0f;
  AmmoParams ammo{"VOG-17", 0.35f, 0.07f, 0.0f};
  float h = 0.0f;
  bool ok = true;

  Coord result = solver.solve(dronePos, targetPos, altitude, accelPath, attackSpeed, ammo, h, ok);

  EXPECT_TRUE(ok);
  EXPECT_NEAR(result.x, 253.684f, 0.1f);
  EXPECT_NEAR(result.y, 169.122f, 0.1f);
  EXPECT_NEAR(h, 55.665f, 0.1f);
}

TEST(Hw08AnalyticalSolver, SamePositionFails)
{
  AnalyticalSolver solver;
  Coord dronePos{0.0f, 0.0f};
  Coord targetPos{0.0f, 0.0f};
  AmmoParams ammo{"VOG-17", 0.35f, 0.07f, 0.0f};
  float h = 0.0f;
  bool ok = true;

  solver.solve(dronePos, targetPos, 100.0f, 50.0f, 15.0f, ammo, h, ok);

  EXPECT_FALSE(ok);
}

TEST(Hw08FileConfigLoader, MissingFileFails)
{
  FileConfigLoader loader("nonexistent_config.json", "nonexistent_ammo.json");
  EXPECT_FALSE(loader.load());
}

TEST(Hw08JsonTargetProvider, MissingFileIsInvalid)
{
  JsonTargetProvider provider("nonexistent_targets.json", 1.0f);
  EXPECT_FALSE(provider.isValid());
}
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)