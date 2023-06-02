/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-06-02 18:10:21 
 */
#ifndef HOMING_H_
#define HOMING_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#define IO_LEVEL_HIGH true
#define IO_LEVEL_LOW false

// 光栅为低电平触发
#define SWITCH_TRIGGERED_LEVEL IO_LEVEL_LOW
// 光栅触发电平计数的阈值，当高/低电平计数大于这个值才认为光栅的触发有效
#define COUNTER_THRESHOLD 3U

typedef enum
{
    HOME_INIT         = 0,   // 回零初始状态
    HOME_POSITIVE_RUN = 1,   // 正向运动状态
    HOME_NEGATIVE_RUN = 2,   // 负向运动状态
    HOME_DONE         = 3,   // 回零完成状态
} HOME_State_t;

/**
 * @brief 读光栅IO输入电平接口
 * 
 * @return true: 高电平; false: 低电平
 */
typedef bool (*HOME_GetIoLevel_t)(void);

typedef struct
{
    HOME_State_t state;
    uint8_t counter;
    uint8_t wait_counter;
    int32_t target_position;
    uint32_t homing_velocity;
    HOME_GetIoLevel_t fGetPhotoelectricSwitchIoLevel;
} HOME_Handle_t;

void HOME_Init(HOME_Handle_t *handle, int32_t start_position);

/**
 * @brief 回零控制接口，其实就是根据光栅的触发状态，规划出一条运动轨迹。
 *        若回零起始位置负限位光栅已触发，需要先向正方向运动直至光栅不触发，再往负方向运动直至光栅触发，
 *        这样才能保证每次回零的位置大致是相同的。反之，直接向负方向运动直至光栅触发。
 * 
 * @param handle 回零控制组件句柄
 * @return int32_t 当前周期回零轨迹的位置目标值
 */
int32_t HOME_TargetPositionPlanning(HOME_Handle_t *handle);

bool HOME_IsDone(HOME_Handle_t *handle);

bool HOME_IsRestart(HOME_Handle_t *handle);

void HOME_Clear(HOME_Handle_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HOMING_H_ */
