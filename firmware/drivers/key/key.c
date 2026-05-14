/**
 * @file    key.c
 * @brief   Non-blocking key state machine with debounce and long-press detection
 * @author  Dinglei Xu
 * @date    2025
 */

#include "key.h"

// =================================================================================
// 定义常量
// =================================================================================
#define DEBOUNCE_TICKS 25     // 消抖时间 (单位: SysTick周期数), 例如 SysTick为5ms, 则为 5*5=25ms
#define LONG_PRESS_TICKS 500 // 长按阈值 (单位: SysTick周期数), 例如 SysTick为5ms, 则为 100*5=500ms

// =================================================================================
// 函数实现
// =================================================================================

/**
 * @brief 初始化按键对象
 */
void key_init(Key_t *key, GPIO_Regs *port, uint32_t pin, uint8_t active_level)
{
    // 确保指针有效
    if (key == NULL)
        return;

    // 1. 关联GPIO硬件信息
    key->port = port;
    key->pin = pin;
    key->active_level = active_level;

    // 2. 初始化内部状态变量
    key->state = KEY_STATE_IDLE;
    key->debounce_counter = 0;
    key->long_press_counter = 0;

    // 3. 读取初始电平作为上一次的电平
    // DL_GPIO_readPins 是TI官方库函数，用于读取一组引脚的电平
    key->prev_level = (DL_GPIO_readPins(key->port, key->pin) & key->pin) ? 1 : 0;
}

/**
 * @brief 获取按键状态
 * @note  这个函数是非阻塞的，它只返回当前的状态，不做任何计算。
 * 并且为了防止状态被错过，在读取后会将一些瞬时状态（如按下、释放）清除。
 */
Key_State_e key_get_state(Key_t *key)
{
    // 确保指针有效
    if (key == NULL)
        return KEY_STATE_IDLE;

    Key_State_e current_state = key->state;

    // 如果是“按下”或“释放”这种一次性状态，读取后就清除，防止重复触发
    if (current_state == KEY_STATE_PRESSED || current_state == KEY_STATE_RELEASED)
    {
        // 如果之前是按下了，现在就进入按住状态
        if (current_state == KEY_STATE_PRESSED)
        {
            key->state = KEY_STATE_HELD;
        }
        else
        { // 如果是释放了，就进入空闲状态
            key->state = KEY_STATE_IDLE;
        }
    }

    return current_state;
}

/**
 * @brief 按键状态机处理核心函数
 * @note  **必须在SysTick中断中被周期性调用**
 */
void key_tick_handler(Key_t *key)
{
    // 确保指针有效
    if (key == NULL)
        return;

    // 1. 读取当前GPIO的电平
    uint8_t current_level = (DL_GPIO_readPins(key->port, key->pin) & key->pin) ? 1 : 0;

    // 2. 状态机核心逻辑
    // 如果当前电平与上一次记录的电平不同 (说明发生了电平跳变)
    if (current_level != key->prev_level)
    {
        key->debounce_counter++; // 消抖计数器累加
        // 如果消抖计数器超过阈值，说明不是干扰信号，是有效的按键动作
        if (key->debounce_counter >= DEBOUNCE_TICKS)
        {
            // 更新上一次电平记录
            key->prev_level = current_level;
            // 清空消抖计数器，为下次做准备
            key->debounce_counter = 0;

            // 判断是按下还是释放
            if (current_level == key->active_level)
            {
                // 如果当前电平是有效电平，说明按键被按下
                key->state = KEY_STATE_PRESSED;
                key->long_press_counter = 0; // 按下时，重置长按计数器
            }
            else
            {
                // 如果当前电平是无效电平，说明按键被释放
                // 只有在按下或长按状态后才响应释放
                if (key->state == KEY_STATE_HELD || key->state == KEY_STATE_LONG_PRESS)
                {
                    key->state = KEY_STATE_RELEASED;
                }
                else
                {
                    key->state = KEY_STATE_IDLE;
                }
            }
        }
    }
    else
    {
        // 如果电平没有变化，则清零消抖计数器
        key->debounce_counter = 0;
    }

    // 3. 长按逻辑处理
    // 如果按键正处于“按住”状态
    if (key->state == KEY_STATE_HELD)
    {
        key->long_press_counter++; // 长按计数器累加
        // 如果计数器超过长按阈值
        if (key->long_press_counter >= LONG_PRESS_TICKS)
        {
            key->state = KEY_STATE_LONG_PRESS; // 切换到长按状态
            // （可选）可以只触发一次长按，或者不清零让它一直处于长按状态
            // key->long_press_counter = 0;
        }
    }
}
