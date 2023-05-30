/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-12 12:07:01 
 */
#include "sys_cfg.h"

#include "com_slave.h"
#include "mcu_port.h"
#include "state_machine.h"
#include "task.h"

static COMSTL_Handle_t SlaveTransportLayer = 
{
    .pHW              = NULL,       //不使用，用宏定义更直接高效
    .WaitSend         = false,
    .ReceiveAvailable = false,
    .fHWInit          = COM_Slave_UART_Init,
    .fSetTimerISR     = COM_Slave_SetTimerISR,
    .fResponse        = TSK_CommunicationResponse,
};
static COMS_Handle_t CommunicationSlave =
{
    .CmdFrameBuff    = {0},
    .ResFrameBuff    = {0},
    .ResFrameSize    = 0,
    .MotorID         = 0,
    .MotorNum        = 0,
    .StatusCode      = 0,
    .MotorData       = {0},
    .pTransportLayer = (void *)&SlaveTransportLayer,
};
COMS_Handle_t* pCommunication = &CommunicationSlave;

static STM_Handle_t state_machine =
{
    .state = STM_IDLE,
    .index = 0,
    .error_codes.uint32 = 0,
};
STM_Handle_t* pStateMachine = &state_machine;
