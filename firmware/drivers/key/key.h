/**
 * @file    key.h
 * @brief   Key state machine type and function declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef __KEY_H_
#define __KEY_H_

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

// =================================================================================
// 1. 定义按键的状态枚举
// =================================================================================
typedef enum
{
    KEY_STATE_IDLE = 0,   // 0: 空闲状态
    KEY_STATE_PRESSED,    // 1: 按下状态 (消抖完成)
    KEY_STATE_HELD,       // 2: 按住状态 (持续按下)
    KEY_STATE_LONG_PRESS, // 3: 长按状态 (按住超过阈值)
    KEY_STATE_RELEASED,   // 4: 释放状态 (刚释放的瞬间)
} Key_State_e;

// =================================================================================
// 2. 定义按键“对象”的结构体
// =================================================================================
typedef struct
{
    /* -- 需要用户配置的参数 -- */
    GPIO_Regs *port;      // 按键连接的GPIO端口, 例如: GPIOA
    uint32_t pin;         // 按键连接的GPIO引脚, 例如: DL_GPIO_PIN_13
    uint8_t active_level; // 按键按下的有效电平 (0: 低电平有效, 1: 高电平有效)

    /* -- 内部状态变量，无需用户关心 -- */
    Key_State_e state;           // 当前按键状态
    uint16_t debounce_counter;   // 消抖计数器
    uint16_t long_press_counter; // 长按计数器
    uint8_t prev_level;          // 上一次读取的引脚电平

} Key_t;

// =================================================================================
// 3. 声明外部函数
// =================================================================================

#define KEY_COUNT 5

/**
 * @brief 初始化一个按键对象
 * @param key               指向要初始化的按键对象的指针
 * @param port              按键所用的GPIO端口 (例如 GPIOA)
 * @param pin               按键所用的GPIO引脚 (例如 DL_GPIO_PIN_13)
 * @param active_level      按键按下的有效电平, 0=低电平, 1=高电平
 */
void key_init(Key_t *key, GPIO_Regs *port, uint32_t pin, uint8_t active_level);

/**
 * @brief 获取按键的当前状态
 * @param key               指向按键对象的指针
 * @return Key_State_e      返回当前按键的状态枚举值
 */
Key_State_e key_get_state(Key_t *key);

/**
 * @brief 按键状态处理函数，需要在SysTick中周期性调用
 * @note  这是实现非阻塞的核心，它驱动所有按键的状态机。
 * @param key               指向按键对象的指针
 */
void key_tick_handler(Key_t *key);

#endif
