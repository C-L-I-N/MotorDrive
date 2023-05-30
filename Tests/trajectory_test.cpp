/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-30 19:39:27 
 */
#include <gtest/gtest.h>

#include "trajectory.h"
#include "sys_cfg.h"

TEST(trajectory_test, StraightLinePlanning)
{
  bool result = true;
  int32_t expected_position[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 
                                 22, 24, 26, 28, 30, 32, 32, 32, 32, 32, 
                                 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 
                                 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 
                                 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 
                                 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 
                                 32, 32, 32, 32};

  TRAJ_Init(pTrajectory, 0);

  TRAJ_SetTargetVelocity(pTrajectory, 2);

  TRAJ_NewPositionCurve(pTrajectory, 1 << 5, 0);

  // for (size_t i = 0; i < 1 << 6; i++)
  // {
  //   printf("%d\t", TRAJ_StraightLine(pTrajectory));
  // }
  // printf("\n");
  
  for (size_t i = 0; i < 1 << 6; i++)
  {
    if (TRAJ_StraightLine(pTrajectory) != expected_position[i])
    {
      result = false;
    }
  }

  EXPECT_EQ(result, true);
}

// 该测试用例默认通过，实际规划是否正常要把打印出来的数据画出曲线，根据图形来判断
TEST(trajectory_test, TCurvePlanning)
{
  bool result = true;

  TRAJ_Init(pTrajectory, 0);

  TRAJ_SetTargetVelocity(pTrajectory, 5);

  TRAJ_NewPositionCurve(pTrajectory, 1 << 7, 0);
  for (size_t i = 0; i < 1 << 6; i++)
  {
    TRAJ_TCurve(pTrajectory);
    printf("%d\n", pTrajectory->current_position);
    // printf("%d\n", pTrajectory->current_velocity);
  }

  TRAJ_NewPositionCurve(pTrajectory, 0, 1 << 7);
  for (size_t i = 0; i < 1 << 6; i++)
  {
    TRAJ_TCurve(pTrajectory);
    printf("%d\n", pTrajectory->current_position);
    // printf("%d\n", pTrajectory->current_velocity);
  }

  TRAJ_NewPositionCurve(pTrajectory, 1 << 9, 0);
  for (size_t i = 0; i < 1 << 8; i++)
  {
    TRAJ_TCurve(pTrajectory);
    printf("%d\n", pTrajectory->current_position);
    // printf("%d\n", pTrajectory->current_velocity);
  }

  EXPECT_EQ(true, true);
}
