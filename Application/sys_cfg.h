/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-12 12:06:32 
 */

#ifndef SYS_CFG_H_
#define SYS_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "com_slave.h"
#include "state_machine.h"

#define FW_MAJOR_VERSION (2U)
#define FW_MINOR_VERSION (0U)
#define FW_PATCH_VERSION (0U)

extern COMS_Handle_t* pCommunication;
extern STM_Handle_t* pStateMachine;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CFG_H_ */
