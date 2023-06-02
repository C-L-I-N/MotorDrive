/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-12 12:33:38 
 */

#ifndef MCU_PORT_H_
#define MCU_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

void COM_Slave_UART_Init(void *pHW_Handle);

void COM_Slave_SetTimerISR(void *pHW_Handle, uint32_t Us);

bool HOME_GetPhotoelectricSwitchIoLevel(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MCU_PORT_H_ */
