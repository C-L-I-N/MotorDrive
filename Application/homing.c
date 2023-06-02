/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-06-02 18:10:25 
 */
#include "homing.h"

void HOME_Init(HOME_Handle_t *handle, int32_t start_position)
{
    handle->target_position = start_position;
}

int32_t HOME_TargetPositionPlanning(HOME_Handle_t *handle)
{
    // 光栅触发次数计数，累计满3次才认为是触发光栅，为了避免由于信号干扰导致误触发
    if (SWITCH_TRIGGERED_LEVEL == handle->fGetPhotoelectricSwitchIoLevel())
    {
        handle->counter = (handle->counter >= COUNTER_THRESHOLD) ? COUNTER_THRESHOLD : handle->counter + 1;
    }
    else
    {
        handle->counter = (handle->counter == 0) ? 0 : handle->counter - 1;
    }

    // 回零状态跳转，根据所处状态和回零速度更新Position target
    switch (handle->state)
    {
    case HOME_INIT:
        // 光栅电平采样计数
        handle->wait_counter++;

        // 光栅电平采样次数要大于触发电平计数的阈值才能开始判断
        if (handle->wait_counter >= COUNTER_THRESHOLD)
        {
            // 若回零起始位置负限位光栅已触发，则要先往正方向运动，反之往负方向运动
            if (handle->counter >= COUNTER_THRESHOLD)
            {
                handle->state = HOME_POSITIVE_RUN;
            }
            else
            {
                handle->state = HOME_NEGATIVE_RUN;
            }
        }
        break;

    case HOME_POSITIVE_RUN:
        handle->target_position += handle->homing_velocity;

        // 向正方向运动到光栅不触发时，再跳到往负方向运动的状态
        if (handle->counter == 0)
        {
            handle->state = HOME_NEGATIVE_RUN;
        }
        break;

    case HOME_NEGATIVE_RUN:
        handle->target_position -= handle->homing_velocity;

        // 向负方向运动到光栅触发时，此时回零完成
        if (handle->counter >= COUNTER_THRESHOLD)
        {
            handle->state = HOME_DONE;
        }
        break;

    case HOME_DONE:
        // 等待下次调用HOME_Clear()接口，状态重新跳到INIT，以便进行二次回零
        break;

    default:
        break;
    }

    return handle->target_position;
}

bool HOME_IsDone(HOME_Handle_t *handle)
{
    return (HOME_DONE == handle->state);
}

bool HOME_IsRestart(HOME_Handle_t *handle)
{
    return (HOME_INIT == handle->state);
}

void HOME_Clear(HOME_Handle_t *handle)
{
    handle->state = HOME_INIT;
    handle->counter = 0;
    handle->wait_counter = 0;
}
