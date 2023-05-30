/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-30 16:00:38 
 */
#include "trajectory.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void TRAJ_Init(TRAJ_Handle_t *handle, int32_t feedback_position)
{
    handle->start_position   = feedback_position;
    handle->current_position = feedback_position;
    handle->target_position  = feedback_position;
}

bool TRAJ_IsReachTargetPosition(TRAJ_Handle_t *handle, int32_t target_position)
{
    bool result = false;

    if (handle->current_position == target_position)
    {
        result = true;
    }

    return result;
}

bool TRAJ_IsNewPositionCurve(TRAJ_Handle_t *handle, int32_t target_position)
{
    bool result = false;

    if (handle->target_position != target_position)
    {
        result = true;
    }

    return result; 
}

void TRAJ_NewPositionCurve(TRAJ_Handle_t *handle, int32_t target_position, int32_t feedback_position)
{
    handle->planning_segment = TRAJ_ACC;
    handle->start_position = feedback_position;
    handle->current_position = feedback_position;
    handle->target_position = target_position;
    handle->current_velocity = 0;
    handle->acceleration_displacement = 0;

    // 目标位置大于当前位置反馈时，正向运动，反之反向运动，相等则不进行规划
    if (target_position > feedback_position)
    {
        handle->direction_cw = true;
        handle->direction_ccw = false;
    }
    else if (target_position < feedback_position)
    {
        handle->direction_cw = false;
        handle->direction_ccw = true;
    }

    handle->index = 0;
}

void TRAJ_SetTargetVelocity(TRAJ_Handle_t *handle, uint32_t target_velocity)
{
    handle->target_velocity = target_velocity;
}

int32_t TRAJ_StraightLine(TRAJ_Handle_t *handle)
{
    // 对于直线轨迹规划，当前轨迹速度直接等于目标速度
    if (true == handle->direction_cw)
    {
        handle->current_velocity = handle->target_velocity;
    }
    else if (true == handle->direction_ccw)
    {
        handle->current_velocity = -handle->target_velocity;
    }
    else
    {
        handle->current_velocity = 0;
    }

    handle->current_position += handle->current_velocity;

    // 减速段位移不一定能被整除，所以需要修正终点
    if (((true == handle->direction_cw) && (handle->current_position >= handle->target_position)) || 
        ((true == handle->direction_ccw) && (handle->current_position <= handle->target_position)))
    {
        handle->current_position = handle->target_position;
        handle->current_velocity = 0;

        handle->direction_cw = false;
        handle->direction_ccw = false;
    }    
    
    return handle->current_position;
}

/**
 * @brief T型曲线轨迹规划：通过对阶越速度进行插值得出梯形速度轨迹，
 *        再对位置进行累积得出位置轨迹
 * 
 * @param handle 
 * @return int32_t 当前周期的位置目标值
 */
int32_t TRAJ_TCurve(TRAJ_Handle_t *handle)
{
    if (true == handle->direction_cw)
    {
        // 状态跳转
        switch (handle->planning_segment)
        {
        case TRAJ_ACC:
            // 计算加速段位移，减掉current_velocity是为了使得规划的减速段位移始终大于加速段位移，以确保肯定能到达目标位置
            handle->acceleration_displacement = handle->current_position - handle->start_position - handle->current_velocity;

            if (handle->acceleration_displacement >= (handle->target_position - handle->current_position))
            {
                handle->index = 0;

                handle->planning_segment = TRAJ_DEC;
            }
            else if (handle->current_velocity == handle->target_velocity)
            {
                handle->index = 0;

                handle->planning_segment = TRAJ_UNIFORM;
            }
            break;
        
        case TRAJ_UNIFORM:
            if (handle->acceleration_displacement >= (handle->target_position - handle->current_position))
            {
                handle->index = 0;

                handle->planning_segment = TRAJ_DEC;
            }
            break;

        case TRAJ_DEC:
            break;

        default:
            break;
        }

        // 根据状态确定加速度
        switch (handle->planning_segment)
        {
        case TRAJ_ACC:
            handle->target_acceleration = TCURVE_ACCELERATION;
            break;

        case TRAJ_UNIFORM:
            handle->target_acceleration = 0;            
            break;

        case TRAJ_DEC:
            handle->target_acceleration = -TCURVE_ACCELERATION;
            break;

        default:
            break;
        }
    }
    else if (true == handle->direction_ccw)
    {
        // 状态跳转
        switch (handle->planning_segment)
        {
        case TRAJ_ACC:
            // 计算加速段位移
            handle->acceleration_displacement = handle->start_position - handle->current_position + handle->current_velocity;

            if (handle->acceleration_displacement >= (handle->current_position - handle->target_position))
            {
                handle->index = 0;

                handle->planning_segment = TRAJ_DEC;
            }
            else if (handle->current_velocity == -handle->target_velocity)
            {
                handle->index = 0;

                handle->planning_segment = TRAJ_UNIFORM;
            }
            break;
        
        case TRAJ_UNIFORM:
            if (handle->acceleration_displacement >= (handle->current_position - handle->target_position))
            {
                handle->index = 0;

                handle->planning_segment = TRAJ_DEC;
            }
            break;

        case TRAJ_DEC:
            break;

        default:
            break;
        }

        // 根据状态确定加速度
        switch (handle->planning_segment)
        {
        case TRAJ_ACC:
            handle->target_acceleration = -TCURVE_ACCELERATION;
            break;

        case TRAJ_UNIFORM:
            handle->target_acceleration = 0;            
            break;

        case TRAJ_DEC:
            handle->target_acceleration = TCURVE_ACCELERATION;
            break;

        default:
            break;
        }
    }
    else
    {
        handle->target_acceleration = 0;
    }
    
    // 由加速度计算得出速度
    if (handle->index == 0)
    {
        handle->current_velocity += handle->target_acceleration;

        // 速度限制：最大值为target值
        // 这里不进行强制类型转化的话运行结果会出错
        if ((int32_t)handle->current_velocity < (int32_t)(-handle->target_velocity) ||
            (int32_t)handle->current_velocity > (int32_t)handle->target_velocity)
        {
            handle->current_velocity = handle->target_velocity;
        }
    }

    // 实际上加速度为1时，速度变化可能仍然太快，所以需要再做细分
    handle->index++;
    if (handle->index >= TCURVE_ACCELERATION_DIV)
    {
        handle->index = 0;
    }

    // 由速度计算得出位置
    handle->current_position += handle->current_velocity;

    // 减速段位移不一定能被整除，所以需要修正终点
    if (((true == handle->direction_cw) && (handle->current_position >= handle->target_position)) || 
        ((true == handle->direction_ccw) && (handle->current_position <= handle->target_position)))
    {
        handle->current_position = handle->target_position;
        handle->current_velocity = 0;

        handle->direction_cw = false;
        handle->direction_ccw = false;

        handle->index = 0;
    }

    return handle->current_position;
}
