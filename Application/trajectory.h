/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-30 16:00:34 
 */
#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

// T型轨迹的加速度
#define TCURVE_ACCELERATION 1
// T型轨迹的加速度细分数，因为即便加速度为1，速度变化可能仍然太快，所以需要再做细分
#define TCURVE_ACCELERATION_DIV 10U

// T型轨迹分段
typedef enum
{
    TRAJ_ACC     = 0,       //匀加速段
    TRAJ_UNIFORM = 1,       //匀速段
    TRAJ_DEC     = 2,       //匀减速段
} TRAJ_PlanningSegment_t;

typedef struct
{
    TRAJ_PlanningSegment_t planning_segment;
    int32_t start_position;
    int32_t current_position;
    int32_t target_position;
    int32_t current_velocity;
    uint32_t target_velocity;
    int8_t target_acceleration;
    uint32_t acceleration_displacement;
    bool direction_cw;
    bool direction_ccw;
    uint8_t index;
} TRAJ_Handle_t;

/**
 * @brief 轨迹规划组件初始化，需要设置位置起点
 * 
 * @param handle 轨迹规划组件句柄
 * @param feedback_position 当前位置反馈
 */
void TRAJ_Init(TRAJ_Handle_t *handle, int32_t feedback_position);

/**
 * @brief 判断位置轨迹末端点是否与目标位置一致
 * 
 * @param handle 轨迹规划组件句柄
 * @param target_position 目标位置
 * @return true 位置轨迹末端点与目标位置一致
 * @return false 位置轨迹末端点与目标位置不一致
 */
bool TRAJ_IsReachTargetPosition(TRAJ_Handle_t *handle, int32_t target_position);

/**
 * @brief 判断是否新增位置轨迹：当target position前后两次不一致时，则需新增位置轨迹，
 *        反之则表示当前目标位置的轨迹正在规划中，无需再新增位置轨迹
 * 
 * @param handle 轨迹规划组件句柄
 * @param target_position 目标位置
 * @return true 可以新增位置轨迹
 * @return false 不可以新增位置轨迹
 */
bool TRAJ_IsNewPositionCurve(TRAJ_Handle_t *handle, int32_t target_position);

/**
 * @brief 新增一条位置轨迹
 * 
 * @param handle 轨迹规划组件句柄
 * @param target_position 目标位置，也就是轨迹末端点
 * @param feedback_position 当前位置反馈，也就是轨迹起点
 */
void TRAJ_NewPositionCurve(TRAJ_Handle_t *handle, int32_t target_position, int32_t feedback_position);

/**
 * @brief 设置轨迹的最大速度
 * 
 * @param handle 轨迹规划组件句柄
 * @param target_velocity 最大目标速度
 */
void TRAJ_SetTargetVelocity(TRAJ_Handle_t *handle, uint32_t target_velocity);

/**
 * @brief 直线轨迹规划，需要轮询执行，每执行一次输出一个位置点
 * 
 * @param handle 轨迹规划组件句柄
 * @return int32_t 当前周期的目标位置
 */
int32_t TRAJ_StraightLine(TRAJ_Handle_t *handle);

/**
 * @brief T型曲线轨迹规划，需要轮询执行，每执行一次输出一个位置点
 * 
 * @param handle 轨迹规划组件句柄
 * @return int32_t 当前周期的目标位置
 */
int32_t TRAJ_TCurve(TRAJ_Handle_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRAJECTORY_H_ */
