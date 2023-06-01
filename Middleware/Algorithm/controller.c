/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-31 17:42:51 
 */
#include "controller.h"

#include <stdint.h>

#include "sin_table.h"

int32_t OneOrderLowpassFilter(Filter_t *handle, int32_t input);

void CTRL_Init(CTRL_Handle_t *handle, int32_t actual_position)
{
    handle->controller_error = 0;
    handle->prev_position_feedback = actual_position;
    handle->position_differential = 0;
    handle->integral_term = 0;
}

AlphaBeta_t CTRL_PositionController(CTRL_Handle_t *handle, int32_t target_position, 
                                    int32_t actual_position, int32_t target_velocity)
{
    // 计算位置反馈的微分
    handle->position_differential = actual_position - handle->prev_position_feedback;
    handle->prev_position_feedback = actual_position;

    // 计算位置误差
    handle->controller_error = target_position - actual_position;

    // 计算积分项并进行积分饱和限制
    handle->integral_term += (handle->position_loop_ki * handle->controller_error) >> 5;
    if (handle->integral_term > handle->upper_output_limit)
    {
        handle->integral_term = handle->upper_output_limit;
        handle->output_limit_flag = true;
    }
    else if (handle->integral_term < -((int32_t)handle->upper_output_limit))
    {
        handle->integral_term = -((int32_t)handle->upper_output_limit);
        handle->output_limit_flag = true;
    }
    else
    {
        handle->output_limit_flag = false;
    }

    // 位置环PI控制器
    handle->controller_output = (handle->position_loop_kp * handle->controller_error + handle->integral_term) >> 7;

    // 位置反馈的微分经过低通滤波后得到速度反馈
    handle->velocity_feedback = OneOrderLowpassFilter(&handle->filter, handle->position_differential);

    // 测速反馈控制器
    handle->controller_output = handle->controller_output - (((handle->velocity_loop_kd >> 3) * handle->velocity_feedback) >> 7);

    // 控制器输出限制
    if (handle->controller_output > handle->upper_output_limit)
    {
        handle->controller_output = handle->upper_output_limit;
        handle->output_limit_flag = true;
    }
    else if (handle->controller_output < -((int32_t)handle->upper_output_limit))
    {
        handle->controller_output = -((int32_t)handle->upper_output_limit);
        handle->output_limit_flag = true;
    }
    else
    {
        handle->output_limit_flag = false;
    }

    // 设置电压相量的角度，目的是为了尽可能使电机产生最大的转矩，
    // 因为转矩是由电流决定的，而对于没有电流采样的系统，该角度就只能通过实测得到。
    // 当转速越大时，该角度也需要增大。
    // 选位置变化角度的三倍作为基的目的可能是为了让电机在转向时角度变化不那么快，
    // 这样运动效果就能更为平缓；另外就是转速增大时，角度也相应的增大。
    handle->voltage_angle = actual_position + handle->position_differential * 3;
    if (handle->controller_output >= 0)
    {
        handle->voltage_angle += VOLTAGE_ADVANCE_ANGLE;
    }
    else
    {
        handle->voltage_angle -= VOLTAGE_ADVANCE_ANGLE;

        // 电压相量的模值为正值
        handle->controller_output = -handle->controller_output;
    }

    // 由机械角度换算电角度（正数）：在已经进行过编码对齐的前提下，由机械角度对一个电周期的编码值取模得到
    // 乘以系数12.5的目的是将电周期由327.68换算成4096
    // 4096 -> 360°，1024 -> 90°
    handle->voltage_electrical_angle.alpha = (int32_t)((float)handle->voltage_angle * 12.5f) % 4096;
    if (handle->voltage_electrical_angle.alpha < 0)
    {
        handle->voltage_electrical_angle.alpha += 4096;
    }
    handle->voltage_electrical_angle.beta = handle->voltage_electrical_angle.alpha + 1024;
    if (handle->voltage_electrical_angle.beta > 4096)
    {
        handle->voltage_electrical_angle.beta -= 4096;
    }

    // 通过查表计算正弦和余弦，从而得出电压相量在alpha-beta轴上的投影
    // 1024 -> 1: 正弦表格由整型数表示小数，放大1024倍
    handle->output_voltage.alpha = (handle->controller_output * sin_table[handle->voltage_electrical_angle.alpha]) >> 10;
    handle->output_voltage.beta = (handle->controller_output * sin_table[handle->voltage_electrical_angle.beta]) >> 10;

    return handle->output_voltage;
}

int32_t OneOrderLowpassFilter(Filter_t *handle, int32_t input)
{
    // 一阶低通滤波器参数alpha采用q1.7格式
    handle->output = handle->alpha * input + (((128 - handle->alpha) * handle->output) >> 7);

    return handle->output;
}
