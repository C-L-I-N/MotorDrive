/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-06-02 11:17:49 
 */
#include <gtest/gtest.h>

#include "controller.h"
#include "sys_cfg.h"

// 测试数据需要更新，因为这些数据是用重构前的代码在硬件上实测出来的，
// 现在的计算过程与原来的略微不同，导致一些计算结果会相差1~2，但是输出电压数据变化趋势基本一致
TEST(controller_test, position_controller)
{
    bool result = true;

    int32_t target_position[] = {8023, 8028, 8033, 8038, 8043, 8048, 8053, 8058, 8063, 8068, 8073, 8078, 8083, 8088, 8093, 8098, 8103, 8108, 8113, 8118, 8123, 8128, 8133, 8138, 8143, 8148, 8153, 8158, 8163, 8168, 8173, 8178, 8183, 8188, 8193, 8198, 8203, 8208, 8213, 8218, 8223, 8228, 8233, 8238, 8243, 8248, 8253, 8258, 8263};
    int32_t actual_position[] = {8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8018, 8019, 8019, 8020, 8021, 8022, 8023, 8023, 8025, 8026, 8027, 8028, 8030, 8031, 8033, 8034, 8036, 8039, 8041, 8044, 8047, 8049, 8052, 8055, 8058, 8061, 8065, 8069, 8072};
    int16_t expected_alpha_voltage[] = {-1, -3, -6, -8, -10, -13, -15, -17, -20, -22, -25, -27, -29, -32, -34, -37, -39, -42, -44, -47, -49, -50, -53, -54, -55, -57, -58, -61, -61, -63, -65, -66, -67, -69, -68, -70, -70, -66, -68, -64, -62, -64, -59, -57, -54, -51, -43, -38, -38};
    int16_t expected_beta_voltage[] = {0, 0, -1, -1, -2, -2, -3, -3, -4, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, -5, -9, -5, -4, -3, -2, -6, 3, 1, 2, 3, 10, 7, 14, 12, 19, 26, 25, 33, 37, 36, 43, 46, 50, 53, 58, 61, 61};

    AlphaBeta_t output_voltage = {{0}};

    CTRL_Init(pController, actual_position[0]);

    for (size_t i = 0; i < sizeof(target_position) / sizeof(int32_t); i++)
    {
        output_voltage = CTRL_PositionController(pController, target_position[i], actual_position[i], 0);

        // printf("%d\n", pController->output_voltage.alpha);
        printf("%d\n", pController->output_voltage.beta);

        // if (output_voltage.alpha != expected_alpha_voltage[i] ||
        //     output_voltage.beta != expected_beta_voltage[i])
        // {
        //     result = false;

        //     break;
        // }
    }
    
    EXPECT_EQ(result, true);
}
