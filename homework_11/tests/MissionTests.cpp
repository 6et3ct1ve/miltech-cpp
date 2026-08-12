#include "DronePhysics.h"
#include "Types.h"
#include "solvers/AnalyticalSolver.h"
#include "solvers/TableSolver.h"
#include "config/FileConfigLoader.h"
#include "providers/ThreadSafeTargetProvider.h"
#include "ThreadSafeQueue.h"
#include "states/AcceleratingState.h"
#include "states/DeceleratingState.h"
#include "states/MovingState.h"
#include "states/StoppedState.h"
#include "states/TurningState.h"

#include <thread>
#include <chrono>
#include <gtest/gtest.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(Hw10Coord, Addition)
{
  Coord a{1.0f, 2.0f};
  Coord b{3.0f, 4.0f};
  Coord result = a + b;
  EXPECT_NEAR(result.x, 4.0f, 0.001f);
  EXPECT_NEAR(result.y, 6.0f, 0.001f);
}

TEST(Hw10Coord, Subtraction)
{
  Coord a{5.0f, 5.0f};
  Coord b{2.0f, 1.0f};
  Coord result = a - b;
  EXPECT_NEAR(result.x, 3.0f, 0.001f);
  EXPECT_NEAR(result.y, 4.0f, 0.001f);
}

TEST(Hw10Coord, ScalarMultiplication)
{
  Coord a{2.0f, 3.0f};
  Coord result = a * 2.0f;
  EXPECT_NEAR(result.x, 4.0f, 0.001f);
  EXPECT_NEAR(result.y, 6.0f, 0.001f);
}

TEST(Hw10Coord, ScalarDivision)
{
  Coord a{6.0f, 8.0f};
  Coord result = a / 2.0f;
  EXPECT_NEAR(result.x, 3.0f, 0.001f);
  EXPECT_NEAR(result.y, 4.0f, 0.001f);
}

TEST(Hw10Coord, DivisionByZeroReturnsZero)
{
  Coord a{6.0f, 8.0f};
  Coord result = a / 0.0f;
  EXPECT_NEAR(result.x, 0.0f, 0.001f);
  EXPECT_NEAR(result.y, 0.0f, 0.001f);
}

TEST(Hw10Coord, Equality)
{
  Coord a{1.0f, 2.0f};
  Coord b{1.0f, 2.0f};
  EXPECT_TRUE(a == b);
}

TEST(Hw10AnalyticalSolver, ComputesKnownDropPoint)
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

TEST(Hw10AnalyticalSolver, SamePositionFails)
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

TEST(Hw10FileConfigLoader, MissingFileFails)
{
  FileConfigLoader loader("nonexistent_config.json", "nonexistent_ammo.json");
  EXPECT_FALSE(loader.load());
}

TEST(Hw10ThreadSafeTargetProvider, MissingFileIsInvalid)
{
  ThreadSafeTargetProvider provider("nonexistent_targets.json", 1.0f, 1.0f);
  EXPECT_FALSE(provider.isValid());
}

TEST(Hw10TableSolver, MissingFileIsInvalid)
{
  TableSolver solver("nonexistent_table.txt");
  EXPECT_FALSE(solver.isValid());
}

TEST(Hw10TableSolver, LoadsRealTable)
{
  TableSolver solver("../../../homework_10/data/ballistic_table.txt");
  EXPECT_TRUE(solver.isValid());
}

TEST(Hw10TableSolver, ComputesDropPointAtExactNode)
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

TEST(Hw10ThreadSafeQueue, PopOnEmptyReturnsFalse)
{
  ThreadSafeQueue<DroneCommand> queue;
  DroneCommand out{};
  EXPECT_FALSE(queue.tryPop(out));
}

TEST(Hw10ThreadSafeQueue, PushThenPopReturnsSameValue)
{
  ThreadSafeQueue<DroneCommand> queue;
  DroneCommand cmd{DroneMode::TURNING, 0.5f};
  queue.push(cmd);

  DroneCommand out{};
  EXPECT_TRUE(queue.tryPop(out));
  EXPECT_EQ(out.mode, DroneMode::TURNING);
  EXPECT_NEAR(out.angleSpeed, 0.5f, 0.001f);
}

TEST(Hw10ThreadSafeQueue, PreservesFifoOrder)
{
  ThreadSafeQueue<DroneCommand> queue;
  DroneCommand cmd1{DroneMode::TURNING, 0.5f};
  DroneCommand cmd2{DroneMode::MOVING, 1.0f};
  DroneCommand cmd3{DroneMode::ACCELERATING, 0.0f};
  queue.push(cmd1);
  queue.push(cmd2);
  queue.push(cmd3);

  DroneCommand out1{};
  DroneCommand out2{};
  DroneCommand out3{};
  EXPECT_TRUE(queue.tryPop(out1));
  EXPECT_TRUE(queue.tryPop(out2));
  EXPECT_TRUE(queue.tryPop(out3));
  EXPECT_EQ(out1.mode, DroneMode::TURNING);
  EXPECT_NEAR(out1.angleSpeed, 0.5f, 0.001f);
  EXPECT_EQ(out2.mode, DroneMode::MOVING);
  EXPECT_NEAR(out2.angleSpeed, 1.0f, 0.001f);
  EXPECT_EQ(out3.mode, DroneMode::ACCELERATING);
  EXPECT_NEAR(out3.angleSpeed, 0.0f, 0.001f);
}

TEST(Hw10ThreadSafeQueue, SurvivesConcurrentAccess)
{
  constexpr int kCount = 1000;
  ThreadSafeQueue<DroneCommand> queue;

  std::thread producer([&queue]() {
    for (int i = 0; i < kCount; i++) {
      queue.push(DroneCommand{DroneMode::MOVING, static_cast<float>(i)});
    }
  });

  int received = 0;
  DroneCommand out{};
  while (received < kCount) {
    if (queue.tryPop(out)) {
      received++;
    }
  }

  producer.join();
  EXPECT_EQ(received, kCount);
}

TEST(Hw10DronePhysics, InitFailsOnZeroAccelPath)
{
  DroneConfig config{};
  config.attackSpeed = 15.0f;
  config.accelPath = 0.0f;
  DronePhysics physics;
  EXPECT_FALSE(physics.init(config));
}

TEST(Hw10DronePhysics, MovingIntegratesPosition)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.accelPath = 50.0f;
  config.physicsTimeStep = 0.1f;
  config.initialDir = 0.0f;
  DronePhysics physics;
  ASSERT_TRUE(physics.init(config));
  physics.submit(DroneCommand{DroneMode::MOVING, 0.0f});
  physics.step();
  DroneTelemetry tlm = physics.getTelemetry();
  EXPECT_NEAR(tlm.pos.x, 1.0f, 0.001f);
  EXPECT_NEAR(tlm.pos.y, 0.0f, 0.001f);
  EXPECT_NEAR(tlm.timeSecSinceStart, 0.1f, 0.001f);
}

TEST(Hw10DronePhysics, TurningChangesDirection)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.accelPath = 50.0f;
  config.physicsTimeStep = 0.1f;
  config.initialDir = 0.0f;
  DronePhysics physics;
  ASSERT_TRUE(physics.init(config));
  physics.submit(DroneCommand{DroneMode::TURNING, 0.5f});
  physics.step();
  DroneTelemetry tlm = physics.getTelemetry();
  EXPECT_NEAR(tlm.direction, 0.05f, 0.001f);
}

TEST(Hw10DronePhysics, AcceleratingClampsAtAttackSpeed)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.accelPath = 50.0f;
  config.physicsTimeStep = 0.1f;
  config.initialDir = 0.0f;
  DronePhysics physics;
  ASSERT_TRUE(physics.init(config));
  physics.submit(DroneCommand{DroneMode::DECELERATING, 0.0f});
  for (int i = 0; i < 50; i++) {
    physics.step();
  }
  physics.submit(DroneCommand{DroneMode::ACCELERATING, 0.0f});
  for (int i = 0; i < 100; i++) {
    physics.step();
  }
  DroneTelemetry tlm = physics.getTelemetry();
  EXPECT_NEAR(tlm.speed, 10.0f, 0.001f);
}

TEST(Hw10DronePhysics, DeceleratingClampsAtZero)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.accelPath = 50.0f;
  config.physicsTimeStep = 0.1f;
  config.initialDir = 0.0f;
  DronePhysics physics;
  ASSERT_TRUE(physics.init(config));
  physics.submit(DroneCommand{DroneMode::DECELERATING, 0.0f});
  for (int i = 0; i < 150; i++) {
    physics.step();
  }
  DroneTelemetry tlm = physics.getTelemetry();
  EXPECT_NEAR(tlm.speed, 0.0f, 0.001f);
}

TEST(Hw10ThreadSafeTargetProvider, SmallStepDoesNotAdvanceNode)
{
  ThreadSafeTargetProvider provider("../../../homework_10/data/targets.json", 1.0f, 1.0f);
  ASSERT_TRUE(provider.isValid());

  const Target before = provider.getTarget(0);
  provider.step(0.1f);
  const Target after = provider.getTarget(0);

  EXPECT_TRUE(after.pos == before.pos);
}

TEST(Hw10ThreadSafeTargetProvider, AccumulatedStepsAdvanceNode)
{
  ThreadSafeTargetProvider provider("../../../homework_10/data/targets.json", 1.0f, 1.0f);
  ASSERT_TRUE(provider.isValid());

  const Target before = provider.getTarget(0);
  for (int i = 0; i < 10; i++) {
    provider.step(0.1f);
  }
  const Target after = provider.getTarget(0);

  EXPECT_FALSE(after.pos == before.pos);
}

TEST(Hw10ThreadSafeTargetProvider, WrapsAroundTrajectory)
{
  constexpr int kTimeSteps = 60;
  ThreadSafeTargetProvider provider("../../../homework_10/data/targets.json", 1.0f, 1.0f);
  ASSERT_TRUE(provider.isValid());

  const Target before = provider.getTarget(0);
  for (int i = 0; i < kTimeSteps; i++) {
    provider.step(1.0f);
  }
  const Target after = provider.getTarget(0);

  EXPECT_TRUE(after.pos == before.pos);
}

TEST(Hw10States, AcceleratingSwitchesToMovingAtAttackSpeed)
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

TEST(Hw10States, DeceleratingSwitchesToStoppedAtZeroSpeed)
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

TEST(Hw10States, MovingSwitchesToDeceleratingOnLargeAngle)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.turnThreshold = 0.05f;
  config.angularSpeed = 0.5f;
  config.physicsTimeStep = 0.01f;

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

TEST(Hw10States, StoppedAlwaysSwitchesToTurning)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.physicsTimeStep = 0.01f;

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

TEST(Hw10States, TurningSwitchesToAcceleratingOnSmallAngle)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.turnThreshold = 0.05f;
  config.angularSpeed = 0.5f;
  config.physicsTimeStep = 0.01f;

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

TEST(Hw10Lifecycle, ThreadBecomesReadyAfterRun)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.accelPath = 50.0f;
  config.physicsTimeStep = 0.01f;
  config.timeScale = 100.0f;

  DronePhysics physics;
  ASSERT_TRUE(physics.init(config));
  EXPECT_FALSE(physics.isThreadReady());

  std::thread t(&DronePhysics::run, &physics);

  while (!physics.isThreadReady()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  EXPECT_TRUE(physics.isThreadReady());

  physics.stop();
  t.join();
}

TEST(Hw10Lifecycle, StopTerminatesRun)
{
  DroneConfig config{};
  config.attackSpeed = 10.0f;
  config.accelPath = 50.0f;
  config.physicsTimeStep = 0.01f;
  config.timeScale = 100.0f;

  DronePhysics physics;
  ASSERT_TRUE(physics.init(config));

  std::thread t(&DronePhysics::run, &physics);

  while (!physics.isThreadReady()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  EXPECT_TRUE(physics.isThreadReady());

  physics.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  const DroneTelemetry tlm = physics.getTelemetry();
  EXPECT_GT(tlm.timeSecSinceStart, 0.0f);

  physics.stop();
  t.join();
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)