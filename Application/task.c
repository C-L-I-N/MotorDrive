/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-29 14:33:05 
 */
#include "task.h"

#include <stdint.h>

#include "com_slave.h"
#include "sys_cfg.h"
#include "state_machine.h"

void TSK_Init(void)
{

}

void TSK_MainLoop(void)
{

}

void TSK_ControlLoop(void)
{

}

void TSK_SafetyLoopHF(void)
{

}

void TSK_SafetyLoopLF(void)
{

}

uint8_t TSK_CommunicationResponse(COMS_MOTOR_t* pMotorData)
{
    uint8_t StatusCode = STATUS_CODE_OK;

    // 指令功能码检查
    if ((pMotorData->FuncCode != FUN_CODE_READ && 
         pMotorData->FuncCode != FUN_CODE_WRITE) ||
       ((pMotorData->FuncCode == FUN_CODE_WRITE) &&
        (pMotorData->ResisterAddr == REG_ERROR_CODES ||
         pMotorData->ResisterAddr == REG_FW_VERSION ||
         pMotorData->ResisterAddr == REG_ACTUAL_POS ||
         pMotorData->ResisterAddr == REG_POS_ERR)))
    {
        pMotorData->ReadValue.uint8[0] = pMotorData->FuncCode;
        pMotorData->ReadValue.uint8[1] = pMotorData->ResisterAddr;

        StatusCode = STATUS_CODE_UNKNOWN;
        goto Out;
    }
    
    // 执行命令对应操作
    switch (pMotorData->ResisterAddr)
    {
    case REG_OP_MODE:
        switch (pMotorData->WriteValue.uint32)
        {
        case OP_MODE_HOMING:

            break;

        case OP_MODE_PRO_POS:

            break;

        case OP_MODE_STO:
        case OP_MODE_CAL:
            // 暂不支持
            break;
        
        default:
            break;
        }

        break;

    case REG_ERROR_CODES:
        pMotorData->ReadValue.uint32 = STM_GetOccurredFaults(pStateMachine);
        StatusCode = STATUS_CODE_OK;
        break;

    case REG_FW_VERSION:
        pMotorData->ReadValue.uint32 = (FW_MAJOR_VERSION << 16) | 
                                       (FW_MINOR_VERSION << 8) | 
                                       (FW_PATCH_VERSION);
        StatusCode = STATUS_CODE_OK;
        break;

    case REG_TARGET_VEL:
        switch (pMotorData->FuncCode)
        {
        case FUN_CODE_READ:
            break;

        case FUN_CODE_WRITE:

            break;

        default:
            break;
        }
        break;

    case REG_TARGET_POS:
        switch (pMotorData->FuncCode)
        {
        case FUN_CODE_READ:

            break;

        case FUN_CODE_WRITE:

            break;
        
        default:
            break;
        }
        break;

    case REG_ACTUAL_POS:

        break;

    case REG_POS_ERR:

        break;

    default:
        pMotorData->ReadValue.uint8[0] = pMotorData->FuncCode;
        pMotorData->ReadValue.uint8[1] = pMotorData->ResisterAddr;
        StatusCode = STATUS_CODE_UNKNOWN;
        break;
    }

Out:
    if (STATUS_CODE_UNKNOWN == StatusCode)
    {
        STM_FaultProcessing(pStateMachine, ERR_CMD_INVALID);
    }
    else if (STATUS_CODE_NOK == StatusCode)
    {
        pMotorData->ReadValue.uint32 = STM_GetOccurredFaults(pStateMachine);
    }

    return StatusCode;
}
