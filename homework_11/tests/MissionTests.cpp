#include "DroneController.h"
#include "TargetTracker.h"
#include "Types.h"
#include "solvers/AnalyticalSolver.h"
#include "solvers/TableSolver.h"
#include "states/AcceleratingState.h"
#include "states/DeceleratingState.h"
#include "states/MovingState.h"
#include "states/StoppedState.h"
#include "states/TurningState.h"

#include <gtest/gtest.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(Hw11Coord, Addition)
{
  Coord a{1.0f, 2.0f};
  Coord b{3.0f, 4.0f};
  Coord result = a + b;
  EXPECT_NEAR(result.x, 4.0f, 0.001f);
  EXPECT_NEAR(result.y, 6.0f, 0.001f);
}

TEST(Hw11Coord, Subtraction)
{
  Coord a{5.0f, 5.0f};
  Coord b{2.0f, 1.0f};
  Coord result = a - b;
  EXPECT_NEAR(result.x, 3.0f, 0.001f);
  EXPECT_NEAR(result.y, 4.0f, 0.001f);
}

TEST(Hw11Coord, ScalarMultiplication)
{
  Coord a{2.0f, 3.0f};
  Coord result = a * 2.0f;
  EXPECT_NEAR(result.x, 4.0f, 0.001f);
  EXPECT_NEAR(result.y, 6.0f, 0.001f);
}

TEST(Hw11Coord, ScalarDivision)
{
  Coord a{6.0f, 8.0f};
  Coord result = a / 2.0f;
  EXPECT_NEAR(result.x, 3.0f, 0.001f);
  EXPECT_NEAR(result.y, 4.0f, 0.001f);
}

TEST(Hw11Coord, DivisionByZeroReturnsZero)
{
  Coord a{6.0f, 8.0f};
  Coord result = a / 0.0f;
  EXPECT_NEAR(result.x, 0.0f, 0.001f);
  EXPECT_NEAR(result.y, 0.0f, 0.001f);
}

TEST(Hw11Coord, Equality)
{
  Coord a{1.0f, 2.0f};
  Coord b{1.0f, 2.0f};
  EXPECT_TRUE(a == b);
}

TEST(Hw11AnalyticalSolver, ComputesKnownDropPoint)
{
  AnalyticalSolver solver;
  Coord dronePos{0.0f, 0.0f};
  Coord targetPos{300.0f, 200.0f};
  float altitude = 100.0f;
  float accelPath = 50.0f;
  float attackSpeed = 15.0f;
  AmmoParams ammo{"VOG-17", 0.35f, 0.004f, 0.0f};
  float h = 0.0f;
  bool ok = true;

  Coord result = solver.solve(dronePos, targetPos, altitude, accelPath, attackSpeed, ammo, h, ok);

  EXPECT_TRUE(ok);
  EXPECT_NEAR(result.x, 244.605f, 0.1f);
  EXPECT_NEAR(result.y, 163.070f, 0.1f);
  EXPECT_NEAR(h, 66.576f, 0.1f);
}

TEST(Hw11AnalyticalSolver, SamePositionFails)
{
  AnalyticalSolver solver;
  Coord dronePos{0.0f, 0.0f};
  Coord targetPos{0.0f, 0.0f};
  AmmoParams ammo{"VOG-17", 0.35f, 0.004f, 0.0f};
  float h = 0.0f;
  bool ok = true;

  solver.solve(dronePos, targetPos, 100.0f, 50.0f, 15.0f, ammo, h, ok);

  EXPECT_FALSE(ok);
}

TEST(Hw11TableSolver, MissingFileIsInvalid)
{
  TableSolver solver("nonexistent_table.txt");
  EXPECT_FALSE(solver.isValid());
}

TEST(Hw11TableSolver, LoadsRealTable)
{
  TableSolver solver("../../../homework_10/data/ballistic_table.txt");
  EXPECT_TRUE(solver.isValid());
}

TEST(Hw11TableSolver, ComputesDropPointAtExactNode)
{
  TableSolver solver("../../../homework_10/data/ballistic_table.txt");
  ASSERT_TRUE(solver.isValid());

  Coord dronePos{0.0f, 0.0f};
  Coord targetPos{300.0f, 0.0f};
  AmmoParams ammo{"VOG-17", 0.35f, 0.004f, 0.0f};
  float h = 0.0f;
  bool ok = true;

  Coord result = solver.solve(dronePos, targetPos, 50.0f, 0.0f, 1.0f, ammo, h, ok);

  EXPECT_TRUE(ok);
  EXPECT_NEAR(h, 2.8938f, 0.01f);
  EXPECT_NEAR(result.x, 297.106f, 0.01f);
  EXPECT_NEAR(result.y, 0.0f, 0.01f);
}

TEST(Hw11States, AcceleratingSwitchesToMovingAtAttackSpeed)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;

  DroneTelemetry tlm{};
  tlm.speed = 10.0f;

  DroneContext ctx{};
  ctx.config = &config;

  DroneCommand cmd{};
  AcceleratingState state;
  auto next = state.execute(tlm, ctx, cmd);

  EXPECT_NE(next, nullptr);
  EXPECT_EQ(cmd.mode, DroneMode::ACCELERATING);
}

TEST(Hw11States, DeceleratingSwitchesToStoppedAtZeroSpeed)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;

  DroneTelemetry tlm{};
  tlm.speed = 0.0f;

  DroneContext ctx{};
  ctx.config = &config;

  DroneCommand cmd{};
  DeceleratingState state;
  auto next = state.execute(tlm, ctx, cmd);

  EXPECT_NE(next, nullptr);
  EXPECT_EQ(cmd.mode, DroneMode::DECELERATING);
}

TEST(Hw11States, MovingSwitchesToDeceleratingOnLargeAngle)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.turnThreshold = 0.05f;
  config.angularSpeed = 0.5f;
  config.timeStep = 0.01f;

  DroneTelemetry tlm{};
  tlm.speed = 10.0f;

  DroneContext ctx{};
  ctx.config = &config;
  ctx.deltaAngle = 1.0f;

  DroneCommand cmd{};
  MovingState state;
  auto next = state.execute(tlm, ctx, cmd);

  EXPECT_NE(next, nullptr);
}

TEST(Hw11States, StoppedAlwaysSwitchesToTurning)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.timeStep = 0.01f;

  DroneTelemetry tlm{};
  tlm.speed = 0.0f;

  DroneContext ctx{};
  ctx.config = &config;

  DroneCommand cmd{};
  StoppedState state;
  auto next = state.execute(tlm, ctx, cmd);

  EXPECT_NE(next, nullptr);
  EXPECT_EQ(cmd.mode, DroneMode::STOPPED);
}

TEST(Hw11States, TurningSwitchesToAcceleratingOnSmallAngle)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.turnThreshold = 0.05f;
  config.angularSpeed = 0.5f;
  config.timeStep = 0.01f;

  DroneTelemetry tlm{};
  tlm.speed = 10.0f;

  DroneContext ctx{};
  ctx.config = &config;
  ctx.deltaAngle = 0.01f;

  DroneCommand cmd{};
  TurningState state;
  auto next = state.execute(tlm, ctx, cmd);

  EXPECT_NE(next, nullptr);
  EXPECT_EQ(cmd.mode, DroneMode::TURNING);
}

TEST(Hw11TargetTracker, FirstUpdateGivesZeroVelocity)
{
  TargetTracker tracker;
  tracker.resize(1);
  tracker.update(0, Coord{10.0f, 20.0f}, 1.0f);

  const auto targets = tracker.targets();
  ASSERT_EQ(targets.size(), 1u);
  EXPECT_NEAR(targets[0].pos.x, 10.0f, 0.001f);
  EXPECT_NEAR(targets[0].velocity.x, 0.0f, 0.001f);
  EXPECT_NEAR(targets[0].velocity.y, 0.0f, 0.001f);
}

TEST(Hw11TargetTracker, SecondUpdateComputesVelocity)
{
  TargetTracker tracker;
  tracker.resize(1);
  tracker.update(0, Coord{10.0f, 20.0f}, 1.0f);
  tracker.update(0, Coord{12.0f, 24.0f}, 2.0f);

  const auto targets = tracker.targets();
  ASSERT_EQ(targets.size(), 1u);
  EXPECT_NEAR(targets[0].velocity.x, 2.0f, 0.001f);
  EXPECT_NEAR(targets[0].velocity.y, 4.0f, 0.001f);
}

TEST(Hw11TargetTracker, OutOfRangeIdIsIgnored)
{
  TargetTracker tracker;
  tracker.resize(1);
  tracker.update(5, Coord{10.0f, 20.0f}, 1.0f);

  const auto targets = tracker.targets();
  ASSERT_EQ(targets.size(), 1u);
  EXPECT_NEAR(targets[0].pos.x, 0.0f, 0.001f);
}

TEST(Hw11DroneController, AcceleratingGivesFullThrottle)
{
  DroneConfig config{};
  config.angularSpeed = 0.5f;

  DroneController controller;
  controller.configure(config);

  const ControlOutput out = controller.toControl(DroneCommand{DroneMode::ACCELERATING, 0.0f});
  EXPECT_NEAR(out.accel, 1.0f, 0.001f);
}

TEST(Hw11DroneController, DeceleratingGivesBrake)
{
  DroneConfig config{};
  config.angularSpeed = 0.5f;

  DroneController controller;
  controller.configure(config);

  const ControlOutput out = controller.toControl(DroneCommand{DroneMode::DECELERATING, 0.0f});
  EXPECT_NEAR(out.accel, -1.0f, 0.001f);
}

TEST(Hw11DroneController, TurnRateIsNormalizedAndClamped)
{
  DroneConfig config{};
  config.angularSpeed = 0.5f;

  DroneController controller;
  controller.configure(config);

  EXPECT_NEAR(controller.toControl(DroneCommand{DroneMode::TURNING, 0.25f}).turnRate, 0.5f, 0.001f);
  EXPECT_NEAR(controller.toControl(DroneCommand{DroneMode::TURNING, 5.0f}).turnRate, 1.0f, 0.001f);
  EXPECT_NEAR(controller.toControl(DroneCommand{DroneMode::TURNING, -5.0f}).turnRate, -1.0f, 0.001f);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)