/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-06-03 17:27:48 
 */
#include <gtest/gtest.h>

#include "homing.h"
#include "sys_cfg.h"

// 模拟光栅IO电平变化周期是20，那为了使每个测试用例间不耦合，
// 必须保证每个测试用例调用HOME_TargetPositionPlanning()接口的次数为20的整数倍

// 回零起始位置未触发光栅测试用例
TEST(homing_test, nontrigger_switch)
{
    bool result = true;

    int32_t expected_position[20] = {0, 0, 0, -1, -2, -3, -4, -5, -6, -7, 
                                     -8, -9, -10, -10, -10, -10, -10, -10, -10, -10};

    HOME_Init(pHoming, 0);

    // 用来造测试数据
    // for (size_t i = 0; i < 20; i++)
    // {
    //     if (false == HOME_IsDone(pHoming))
    //     {
    //         printf("%d, ", HOME_TargetPositionPlanning(pHoming));
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }
    // printf("\n");

    for (size_t i = 0; i < sizeof(expected_position) / sizeof(int32_t); i++)
    {
        if (HOME_TargetPositionPlanning(pHoming) != expected_position[i])
        {
            result = false;
        }
    }

    HOME_Clear(pHoming);

    EXPECT_EQ(result, true);
}

// 回零起始位置触发光栅测试用例
TEST(homing_test, trigger_switch)
{
    bool result = true;

    int32_t expected_position[50] = {0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 
                                     8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 
                                     2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // 跳过初始十次光栅IO输出，以确保回零起始位置是处于光栅触发的位置
    for (size_t i = 0; i < 10; i++)
    {
        HOME_TargetPositionPlanning(pHoming);
    }
    HOME_Clear(pHoming);

    HOME_Init(pHoming, 0);

    // 用来造测试数据
    // for (size_t i = 0; i < 50; i++)
    // {
    //     if (false == HOME_IsDone(pHoming))
    //     {
    //         printf("%d, ", HOME_TargetPositionPlanning(pHoming));
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }
    // printf("\n");

    for (size_t i = 0; i < sizeof(expected_position) / sizeof(int32_t); i++)
    {
        if (HOME_TargetPositionPlanning(pHoming) != expected_position[i])
        {
            result = false;
        }
    }

    HOME_Clear(pHoming);

    EXPECT_EQ(result, true);
}

// 二次回零测试用例
TEST(homing_test, secondary_homing)
{
    bool result = true;

    int32_t expected_position[2][20] = {{0, 0, 0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -10, -10, -10, -10, -10, -10, -10},
                                        {0, 0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -11, -11, -11, -11, -11, -11, -11}};

    for (size_t i = 0; i < 2; i++)
    {
        // 用来造测试数据
        // for (size_t j = 0; j < 20; j++)
        // {
        //     if (true == HOME_IsDone(pHoming))
        //     {
        //         // 等跑到第二次再清楚回零状态重新开始，以确保二次回零的初始位置和与上一次一样
        //         if (j == 19)
        //         {
        //             HOME_Clear(pHoming);
        //         }
        //     }
        //     else if (true == HOME_IsRestart(pHoming))
        //     {
        //         HOME_Init(pHoming, 0);
        //     }

        //     printf("%d, ", HOME_TargetPositionPlanning(pHoming));
        // }
        // printf("\n");

        for (size_t j = 0; j < 20; j++)
        {
            if (true == HOME_IsDone(pHoming))
            {
                // 等跑到第二次再清楚回零状态重新开始，以确保二次回零的初始位置和与上一次一样
                if (j == 19)
                {
                    HOME_Clear(pHoming);
                }
            }
            else if (true == HOME_IsRestart(pHoming))
            {
                HOME_Init(pHoming, 0);
            }

            if (HOME_TargetPositionPlanning(pHoming) != expected_position[i][j])
            {
                result = false;
            }
        }
    }
    
    HOME_Clear(pHoming);

    EXPECT_EQ(result, true);
}
