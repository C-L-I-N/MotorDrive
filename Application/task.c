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
#include "trajectory.h"
#include "controller.h"
#include "homing.h"

static bool closed_loop_control = false;
static uint32_t target_velocity = 0;
static int32_t current_position = 0;
static int32_t target_position = 0;
static int32_t actual_position = 0;
static AlphaBeta_t output_voltage = {0};

void TSK_Init(void)
{
    // MCU初始化

    // 编码器初始化

    // 读电机起始位置反馈

    // 轨迹规划初始化：设置轨迹规划起始位置
    TRAJ_Init(pTrajectory, actual_position);

    // 控制器初始化
    CTRL_Init(pController, actual_position);
}

void TSK_MainLoop(void)
{

}

void TSK_ControlLoop(void)
{
    // 读编码值（位置反馈）

    // 解析命令帧
    COM_Slave_Cycle(pCommunication);

    // 高频安全检查任务

    // 系统状态机
    switch (STM_GetState(pStateMachine))
    {
    case STM_IDLE:
        // 由闭环控制把电机Hold在原处
        closed_loop_control = true;
        break;

    case STM_HOME:
        current_position = HOME_TargetPositionPlanning(pHoming);
        closed_loop_control = true;
        break;

    case STM_RUN:
        // 暂时只支持位置控制
        current_position = TRAJ_StraightLine(pTrajectory); // TODO 等直线轨迹测试通过后，再试T型轨迹
        closed_loop_control = true;
        break;

    case STM_STOP:
        // 暂不支持
        break;

    case STM_FAULT_NOW:
        
        break;

    case STM_FAULT_OVER:
        
        break;

    case STM_ENCODER_CAL:
        // 暂不支持
        break;

    default:
        break;
    }

    // 闭环控制
    if (true == closed_loop_control)
    {
        output_voltage = CTRL_PositionController(pController, current_position, actual_position, 0/* 暂不使用速度前馈 */);
    }
    else
    {
        // 开环控制时，初始化位置控制器，以便后续能够正常切换到闭环控制
        CTRL_Init(pController, actual_position);
    }

    // 功率驱动PWM输出
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
        if (FUN_CODE_WRITE == pMotorData->FuncCode)
        {
            switch (pMotorData->WriteValue.uint32)
            {
            case OP_MODE_HOMING:
                // 系统状态机跳转到回零状态
                if (true == STM_NextState(pStateMachine, STM_HOME))
                {
                    // 回零成功后才返回OK
                    if (true == HOME_IsDone(pHoming))
                    {
                        // 清除回零成功状态，以便进行二次回零
                        HOME_Clear(pHoming);
                        StatusCode = STATUS_CODE_OK;
                    }
                    // 当刚开始回零时，将当前位置反馈实际值作为回零位置轨迹的起点
                    else if (true == HOME_IsRestart(pHoming))
                    {
                        HOME_Init(pHoming, actual_position);
                        StatusCode = STATUS_CODE_NOK;
                    }
                    else
                    {
                        StatusCode = STATUS_CODE_NOK;
                    }
                }
                else
                {
                    StatusCode = STATUS_CODE_NOK;
                }
                break;

            case OP_MODE_PRO_POS:
                // 系统状态机跳转到正常运行状态
                if (true == STM_NextState(pStateMachine, STM_RUN))
                {
                    StatusCode = STATUS_CODE_OK;
                }
                else
                {
                    StatusCode = STATUS_CODE_NOK;
                }
                break;

            case OP_MODE_STO:
            case OP_MODE_CAL:
                // 暂不支持
                break;
            
            default:
                break;
            }
        }
        else
        {    
            switch (STM_GetState(pStateMachine))
            {
            case STM_IDLE:
            case STM_STOP:
            case STM_FAULT_NOW:
            case STM_FAULT_OVER:
                pMotorData->ReadValue.uint8[0] = OP_MODE_STO;
                break;
            case STM_RUN:
                pMotorData->ReadValue.uint8[0] = OP_MODE_PRO_POS;
                break;
            case STM_HOME:
                pMotorData->ReadValue.uint8[0] = OP_MODE_HOMING;
                break;
            case STM_ENCODER_CAL:
                pMotorData->ReadValue.uint8[0] = OP_MODE_CAL;
                break;
            default:
                break;
            }
            StatusCode = STATUS_CODE_OK;
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
            pMotorData->ReadValue.uint32 = target_velocity;
            break;

        case FUN_CODE_WRITE:
            target_velocity = pMotorData->WriteValue.uint32;
            TRAJ_SetTargetVelocity(pTrajectory, target_velocity);
            break;

        default:
            break;
        }
        StatusCode = STATUS_CODE_OK;
        break;

    case REG_TARGET_POS:
        switch (pMotorData->FuncCode)
        {
        case FUN_CODE_READ:
            pMotorData->ReadValue.int32 = target_position;
            StatusCode = STATUS_CODE_OK;
            break;

        case FUN_CODE_WRITE:
            // 特别注意：前后两次target position不同时，轨迹可以被打断
            target_position = pMotorData->WriteValue.int32;
            // 当位置轨迹当前位置与目标位置相同，则应答OK
            if (true == TRAJ_IsReachTargetPosition(pTrajectory, target_position))
            {
                StatusCode = STATUS_CODE_OK;
            }
            // 当位置轨迹末端位置与目标位置不同，则可以需新增位置轨迹
            else if (true == TRAJ_IsNewPositionCurve(pTrajectory, target_position))
            {
                TRAJ_NewPositionCurve(pTrajectory, target_position, actual_position);
                StatusCode = STATUS_CODE_NOK;
            }
            else
            {
                StatusCode = STATUS_CODE_NOK;
            }
            break;
        
        default:
            break;
        }
        break;

    case REG_ACTUAL_POS:
        pMotorData->ReadValue.int32 = actual_position;
        StatusCode = STATUS_CODE_OK;
        break;

    case REG_POS_ERR:
        pMotorData->ReadValue.int32 = current_position - actual_position;
        StatusCode = STATUS_CODE_OK;
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
