/**
 * @file    motor.h
 * @brief   Motor driver: physical constants, type definitions, function declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include "ti_msp_dl_config.h"
#include "pid.h"

// =========================================================
// === 在这里定义小车的物理参数 (请根据您的实际硬件修改!) ===
// =========================================================

// 假设轮子直径为 6.8cm，半径即为 3.4cm
#define Wheel_radius (3.3f)

// 编码器转一圈的总脉冲数 (例如，电机减速组输出轴 的脉冲数)
// 这个值需要您查阅编码器手册或者自己实测得到
#define Full_turnPulse (390.0f)

// 速度采样时间，与SysTick中调用Motor_GetSpeed的周期一致 (单位：毫秒)
// 在我们的设计中，这个周期是 5ms
#define Sampling_time (20.0f)

// 在 motor.h 中添加
extern int Speed_L, Speed_R;
extern volatile int32_t g_encoder_total_L;
extern volatile int32_t g_encoder_total_R;
// =========================================================
// === 函数声明 (Public Function Prototypes) ===
// =========================================================

// --- 新增的函数声明，以匹配 motor.c ---
void Motor_Output(void);
void Motor_GetSpeed(void);
void Motor_Stop(void);
void motor_speed_pid_update(void);

// 您原有的函数声明可以保留，以备将来使用
// 如果您确定用不上，也可以删除它们
typedef enum
{
    MOTOR_A = 0,
    MOTOR_B = 1
} Motor_ID;
void motor_init(void);
//void motor_set_speed(Motor_ID motor, int16_t speed);
void motor_set_pulse(Motor_ID motor, int16_t speed);

void motor_brake(Motor_ID motor);

#endif // MOTOR_H_