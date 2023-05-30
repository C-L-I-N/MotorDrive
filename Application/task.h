/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-29 14:33:47 
 */
#ifndef TASK_H_
#define TASK_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#include "com_slave.h"

void TSK_Init(void);

void TSK_MainLoop(void);

void TSK_ControlLoop(void);

void TSK_SafetyLoopHF(void);

void TSK_SafetyLoopLF(void);

uint8_t TSK_CommunicationResponse(COMS_MOTOR_t* pMotorData);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TASK_H_ */
