/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-29 14:44:36 
 */
#include "state_machine.h"

#include <stdbool.h>
#include <stdint.h>

bool STM_NextState(STM_Handle_t *pstm, STM_State_t next_state)
{
    bool result = false;

    // 当出现错误时，无论当前处于何种状态都立即切换到出错状态
    if (STM_FAULT_NOW == next_state)
    {
        result = true;

        goto out;
    }

    switch (pstm->state)
    {
    case STM_IDLE:
        if ((STM_HOME == next_state) || (STM_RUN == next_state) ||
            (STM_ENCODER_CAL == next_state))
        {
            result = true;
        }
        break;
    
    case STM_HOME:
        if ((STM_RUN == next_state) || (STM_STOP == next_state))
        {
            result = true;
        }
        break;

    case STM_RUN:
        if (STM_STOP == next_state)
        {
            result = true;
        }
        break;

    case STM_STOP:
    case STM_FAULT_OVER:
    case STM_ENCODER_CAL:
        if (STM_IDLE == next_state)
        {
            result = true;
        }
        break;

    case STM_FAULT_NOW:
        if (STM_FAULT_OVER == next_state)
        {
            result = true;
        }        
        break;

    default:
        break;
    }

out:
    // 切换成新状态
    if (true == result)
    {
        pstm->state = next_state;
    }

    return result;
}

STM_State_t STM_GetState(STM_Handle_t *pstm)
{
    return pstm->state;
}

void STM_FaultProcessing(STM_Handle_t *pstm, uint8_t set_error)
{
    // 只保存最开始的四次出错的错误码
    if (pstm->index < 4)
    {
        uint8_t i = 0;

        // 已经保存过的错误码就不再保存
        for (i = 0; i < pstm->index; i++)
        {
            if (pstm->error_codes.uint8[i] == set_error)
            {
                break;
            }
        }

        // 保存没保存过的错误码
        if (i >= pstm->index)
        {
            pstm->error_codes.uint8[i] = set_error;

            pstm->index++;
        }
    }
}

void STM_FaultAcknowledged(STM_Handle_t *pstm)
{
    if (STM_FAULT_OVER == pstm->state)
    {
        pstm->index = 0;
        pstm->error_codes.uint32 = 0;
        pstm->state = STM_IDLE;
    }
}

uint32_t STM_GetOccurredFaults(STM_Handle_t *pstm)
{
    return (pstm->error_codes.uint32);
}
