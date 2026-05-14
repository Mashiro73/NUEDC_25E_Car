/**
 * @file    main.h
 * @brief   Shared macros, state machine definitions, and global declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef __MAIN_H
#define __MAIN_H

#include "ti_msp_dl_config.h"

/*
 * =================================================================================
 * 2. 包含项目中所有用到的模块头文件
 * 把所有需要的 .h 文件都在这里统一管理，让 main.c 保持整洁。
 * =================================================================================
 */
#include <stdio.h>
#include "lcd.h"
#include "key.h"
#include "motor.h"
#include "pid.h"
#include "line_sensor.h"
#include "lcd_init.h"
#include "clock.h"


// =================================================================================
// 1. 宏定义 (常量)
// =================================================================================

// 【重要】请根据实际测试来标定这两个编码器计数值
#define ENCODER_COUNT_FORWARD (350) // 遇到直角后，向前直行的编码器目标计数值
#define ENCODER_COUNT_TURN    (260) // 原地左转90度的编码器目标计数值

#define BASE_SPEED (18) //15        // 小车循迹和直行时的基础速度
#define TURN_SPEED (11)         // 小车原地转弯时的速度
#define LINE_KP    (0.2f)        // 循迹时的比例系数

// 定义传感器检测到直角的条件 (例如，最左、最右和中间的传感器同时检测到黑线)
#define CORNER_DETECT_MASK1 (0b11100000) 
#define CORNER_DETECT_MASK2 (0b11110000) 
#define CORNER_DETECT_MASK3 (0b11111000)
#define CORNER_DETECT_MASK6 (0b11111100)
#define CORNER_DETECT_MASK4 (0b00011000)
#define CORNER_DETECT_MASK5 (0b0001000)
#define NUM_KEYS 3
// =================================================================================
// 2. 状态机枚举定义
// =================================================================================
typedef enum {
	S_CALIBRATE,
    S0_SELECT_LAPS,         // 【新增】S0.1: 选择圈数
    S0_WAIT_FOR_START,      // S0.2: 等待开始
	S1_START_TO_GO,          //启动特调加速度
    S1_LINE_FOLLOWING,      // S1:   沿直线循迹
    S2_PREPARE_TURN_FORWARD,// S2.1: 准备转弯-直行
    S2_TURNING_LEFT,        // S2.2: 准备转弯-旋转
    S3_STOPPED              // S3:   完成任务，停车
	} PathState_e;

// =================================================================================
// 3. 外部变量和函数声明
// =================================================================================
extern LineSensor_t myTracker;
extern volatile int64_t g_encoder_l_total; // 用于计数的左轮总编码器值
extern volatile int64_t g_encoder_r_total; // 用于计数的右轮总编码器值

// 【新增】按键别名
extern Key_t my_keys[NUM_KEYS];
#define KEY_RESET  my_keys[0] // S1
#define KEY_CONFIRM my_keys[1] // S2
#define KEY_SELECT  my_keys[0] // S1: 在选择模式下为选择键
#endif // __MAIN_H