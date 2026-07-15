#include "ballistics.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(Ballistics, ComputesKnownDropPoint)
{
  const float kDroneX = 100.0f;
  const float kDroneY = 100.0f;
  const float kDroneZ = 100.0f;
  const float kTargetX = 200.0f;
  const float kTargetY = 200.0f;
  const float kAttackSpeed = 10.0f;
  const float kAccelerationPath = 10.0f;
  const std::string kAmmoName = "VOG-17";
  DroneState state{kDroneX, kDroneY, kDroneZ, kTargetX, kTargetY, kAttackSpeed, kAccelerationPath, kAmmoName};
  bool ok = true;

  DropPoint result = compute_drop_point(state, ok);

  EXPECT_TRUE(ok);
  EXPECT_NEAR(result.fireX, 173.759, 0.01);
  EXPECT_NEAR(result.fireY, 173.759, 0.01);
}

TEST(Ballistics, UnknownAmmoFails)
{
  const float kDroneX = 100.0f;
  const float kDroneY = 100.0f;
  const float kDroneZ = 100.0f;
  const float kTargetX = 200.0f;
  const float kTargetY = 200.0f;
  const float kAttackSpeed = 10.0f;
  const float kAccelerationPath = 10.0f;
  const std::string kAmmoName = "Dafasfafkkjaf";
  DroneState state{kDroneX, kDroneY, kDroneZ, kTargetX, kTargetY, kAttackSpeed, kAccelerationPath, kAmmoName};
  bool ok = true;

  compute_drop_point(state, ok);

  EXPECT_FALSE(ok);
}