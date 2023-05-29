/*
 * @Author: NO3
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-29 14:44:46
 */
#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    STM_IDLE        = 0,
    STM_HOME        = 1,
    STM_RUN         = 2,
    STM_STOP        = 3,
    STM_FAULT_NOW   = 4,
    STM_FAULT_OVER  = 5,
    STM_ENCODER_CAL = 7,
} STM_State_t;

typedef struct
{
    STM_State_t state;
    uint8_t index;
    union
    {
        uint8_t uint8[4];
        uint32_t uint32;
    } error_codes;
} STM_Handle_t;

bool STM_NextState(STM_Handle_t *pstm, STM_State_t next_state);

STM_State_t STM_GetState(STM_Handle_t *pstm);

void STM_FaultProcessing(STM_Handle_t *pstm, uint8_t set_error);

void STM_FaultAcknowledged(STM_Handle_t *pstm);

uint32_t STM_GetOccurredFaults(STM_Handle_t *pstm);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STATE_MACHINE_H_ */
