/**
 * @file bsp_imu.h
 * @brief IMU传感器板级支持包（BSP）头文件 (适用于TI MCU)
 * @details
 * - 声明了与IMU底层硬件操作相关的函数。
 * - 供上层应用（如AHRS算法）调用。
 * @version 2.0
 * @date 2025-07-25
 */

#ifndef __BSP_IMU_H
#define __BSP_IMU_H

// 包含上层的IMU核心库头文件
#include "imu_ahrs_core.h"

/**
 * @brief 获取为核心库实现的硬件函数集合
 * @details
 * 这个函数返回一个结构体指针，该结构体包含了所有底层硬件操作函数的地址，
 * 上层库通过这个接口与底层硬件解耦。
 * @return const imu_hal_functions_t* 指向硬件抽象层函数集的指针
 */
const imu_hal_functions_t *bsp_imu_get_hal(void);

/**
 * @brief 初始化所有与IMU相关的底层硬件
 * @details
 * 此函数应在主函数进入主循环前调用。
 * 它会启动用于计时的Timer。
 * 注意：TI的SysConfig通常会在 SYSCFG_DL_init() 中完成大部分初始化。
 * 这个函数可以用来执行一些额外的、应用特定的启动步骤。
 */
void bsp_imu_hardware_init(void);
uint32_t get_time_us(void);
 void sleep_us(uint32_t us);

#endif // __BSP_IMU_H
