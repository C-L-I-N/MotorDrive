/*
 * @Author: NO3
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-31 17:42:47
 */
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

// 电压相量的超前角（步距角1.8°对应分辨率2^14的编码器的编码值为81.92）
#define VOLTAGE_ADVANCE_ANGLE 82U

typedef struct
{
    int16_t alpha;
    int16_t beta;
} AlphaBeta_t;

typedef struct
{
    uint8_t alpha;      // 采用q1.7格式
    int32_t output;     // 采用q25.7格式
} Filter_t;

typedef struct
{
    int16_t position_loop_kp;           // 采用q16.0格式
    int16_t position_loop_ki;           // 采用q11.5格式
    int16_t velocity_loop_kd;           // 采用q13.3格式
    int32_t controller_error;
    int32_t prev_position_feedback;
    int32_t position_differential;
    Filter_t filter;
    int32_t velocity_feedback;          // 采用q25.7格式
    int32_t integral_term;              // 采用q25.7格式
    uint32_t upper_output_limit;        // 采用q25.7格式
    bool output_limit_flag;
    int32_t controller_output;
    int32_t voltage_angle;
    AlphaBeta_t voltage_electrical_angle;
    AlphaBeta_t output_voltage;
} CTRL_Handle_t;

/**
 * @brief 控制器初始化，每次切换到闭环控制前都需要调用该接口
 * 
 * @param handle 控制器组件句柄
 * @param actual_position 电机转子的当前实际位置
 */
void CTRL_Init(CTRL_Handle_t *handle, int32_t actual_position);

/**
 * @brief 闭环位置控制器，输入位置和速度数据，输出电压值，每个控制周期调用一次
 * 
 * @param handle 控制器组件句柄
 * @param target_position 目标位置
 * @param actual_position 电机转子的当前实际位置
 * @param target_velocity 目标速度(可作为速度前馈，目前暂未使用)
 * @return AlphaBeta_t alpha-beta轴上的输出电压
 */
AlphaBeta_t CTRL_PositionController(CTRL_Handle_t *handle, int32_t target_position,
                                    int32_t actual_position, int32_t target_velocity);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONTROLLER_H_ */
