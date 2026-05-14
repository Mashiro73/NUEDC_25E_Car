/**
 * @file bsp_imu.c
 * @brief IMU传感器板级支持包（BSP）实现文件 (适用于TI MCU)
 * @version 2.2 - 为SPI通信增加了超时保护，防止程序卡死
 * @date 2025-07-25
 */

#include "bsp_imu.h"
#include "ti_msp_dl_config.h"

//================================================================================================//
//                                                                                                //
//                                【移植前请务必定义此部分宏】                                     //
//                                                                                                //
//================================================================================================//

#define IMU_SPI_INSTANCE (SPI_IMU_INST)
#define US_TIMER_INSTANCE (TIMER_1_INST) // 用于延时和时间戳的定时器
//#define IMU_CS_PORT (IMU_CS_PIN_CS_PORT) // [修正] 请确保SysConfig中CS引脚的Name为IMU_CS_PIN_CS
#define IMU_CS_PIN (IMU_CS_PIN_CS_PIN)   // [修正] 确保宏定义正确

// [新增] 定义一个SPI通信的超时计数，防止在while中死循环
#define SPI_TIMEOUT_COUNT (100000)

/* ------------------ 底层硬件函数实现 (TI DriverLib) ------------------ */
extern volatile uint16_t g_timer_high_bits;

int spi_write_reg(uint8_t reg, const uint8_t *buf, uint32_t len)
{
    volatile uint32_t timeout;

    DL_GPIO_clearPins(IMU_CS_PORT, IMU_CS_PIN);

    // 等待SPI总线空闲，带超时
    timeout = SPI_TIMEOUT_COUNT;
    while (DL_SPI_isBusy(IMU_SPI_INSTANCE) && timeout--)
        ;
    if (timeout == 0)
        return -1; // 超时错误

    // 发送寄存器地址（写模式）
    DL_SPI_transmitData8(IMU_SPI_INSTANCE, reg & 0x7F);

    // 等待发送完成，带超时
    timeout = SPI_TIMEOUT_COUNT;
    while (DL_SPI_isBusy(IMU_SPI_INSTANCE) && timeout--)
        ;
    if (timeout == 0)
        return -1;

    // 循环发送数据
    for (uint32_t i = 0; i < len; i++)
    {
        DL_SPI_transmitData8(IMU_SPI_INSTANCE, buf[i]);
        timeout = SPI_TIMEOUT_COUNT;
        while (DL_SPI_isBusy(IMU_SPI_INSTANCE) && timeout--)
            ;
        if (timeout == 0)
            return -1;
    }

    DL_GPIO_setPins(IMU_CS_PORT, IMU_CS_PIN);
    return 0;
}

int spi_read_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
    volatile uint32_t timeout;

    DL_GPIO_clearPins(IMU_CS_PORT, IMU_CS_PIN);

    // 清空可能残留的接收FIFO
    while (DL_SPI_isRXFIFOEmpty(IMU_SPI_INSTANCE) == false)
    {
        DL_SPI_receiveData8(IMU_SPI_INSTANCE);
    }

    // 等待总线空闲
    timeout = SPI_TIMEOUT_COUNT;
    while (DL_SPI_isBusy(IMU_SPI_INSTANCE) && timeout--)
        ;
    if (timeout == 0)
        return -1;

    // 发送寄存器地址（读模式）
    DL_SPI_transmitData8(IMU_SPI_INSTANCE, reg | 0x80);

    // 等待接收到从机返回的无效字节
    timeout = SPI_TIMEOUT_COUNT;
    while (DL_SPI_isRXFIFOEmpty(IMU_SPI_INSTANCE) && timeout--)
        ;
    if (timeout == 0)
        return -1;
    DL_SPI_receiveData8(IMU_SPI_INSTANCE); // 读取并丢弃这个字节

    // 循环读取数据
    for (uint32_t i = 0; i < len; i++)
    {
        // 发送一个虚拟字节(0xFF)来产生时钟，以读取数据
        DL_SPI_transmitData8(IMU_SPI_INSTANCE, 0xFF);
        timeout = SPI_TIMEOUT_COUNT;
        while (DL_SPI_isRXFIFOEmpty(IMU_SPI_INSTANCE) && timeout--) // 等待数据接收
            ;
        if (timeout == 0)
            return -1;

        buf[i] = DL_SPI_receiveData8(IMU_SPI_INSTANCE);
    }

    // 等待总线最终空闲
    timeout = SPI_TIMEOUT_COUNT;
    while (DL_SPI_isBusy(IMU_SPI_INSTANCE) && timeout--)
        ;

    DL_GPIO_setPins(IMU_CS_PORT, IMU_CS_PIN);

    // 如果上面任何一步超时，timeout最终会是0
    return (timeout > 0) ? 0 : -1;
}

// 实现一个既精确又高效的 get_time_us 函数
uint32_t get_time_us(void)
{
    uint16_t high_before, low, high_after;

    // 防止在读取高低位时发生中断导致数据错乱
    do
    {
        high_before = g_timer_high_bits;
        low = DL_Timer_getTimerCount(TIMER_1_INST);
        high_after = g_timer_high_bits;
    } while (high_before != high_after);

    // 将高16位和低16位拼接成一个32位的时间戳
    return ((uint32_t)high_before << 16) | low;
}

// 延时函数 (可以用SysTick实现的delay_ms，或基于新get_time_us的忙等待)
void sleep_us_robust(uint32_t us)
{
    uint32_t start = get_time_us();
    while ((get_time_us() - start) < us)
        ;
}
/* ------------------ HAL 结构体封装 ------------------ */

const imu_hal_functions_t imu_instance = {
    .write_reg = spi_write_reg,
    .read_reg = spi_read_reg,
    .sleep_us = sleep_us_robust,
    .get_time_us = get_time_us};

const imu_hal_functions_t *bsp_imu_get_hal(void)
{
    return &imu_instance;
}

/* ------------------ BSP 初始化函数 ------------------ */

void bsp_imu_hardware_init(void)
{
    DL_Timer_startCounter(US_TIMER_INSTANCE);
}

// /**
// ?* @file main.c
// ?* @brief 极简主程序：循环读取并显示IMU的6轴原始数据
// ?* @details
// ?* - 移除了所有状态机和非阻塞调度逻辑。
// ?* - 专注于验证传感器原始数据的读取和显示。
// ?* @version 5.0 - Simplified for Raw Data Reading
// ?*/

// #include "ti_msp_dl_config.h"
// #include <stdio.h>

// // 包含我们自己的BSP模块和上层库
// #include "bsp_imu.h"
// #include "imu_ahrs_core.h"
// #include "lcd.h"
// #include "lcd_init.h"
// #include "key.h"
// #define NUM_KEYS 2
// Key_t my_keys[NUM_KEYS];
// #define ENCODER_SWITCH_KEY my_keys[0]
// #define MEASURE_KEY my_keys[1]

// /* ============================================================================
//  * @brief 重定义 fputc 函数，将 printf 的输出重定向到 UART0
//  * ============================================================================
//  */

// // Keil MDK 标准库重定向 printf 所需的结构体和全局变量
// // 如果您使用的是 Microlib，则不需要这部分，但为了兼容性，通常都写上。
// #if !defined(__MICROLIB)
// #if (__ARMCLIB_VERSION <= 6000000)
// struct __FILE
// {
//     int handle;
// };
// #endif
// FILE __stdout;
// void _sys_exit(int x) { x = x; }
// #endif

// // 核心函数：实现单个字符的发送
// int fputc(int ch, FILE *stream)
// {
//     // 等待，直到 UART0 的发送缓冲区有空间
//     // DL_UART_Main_isTXFIFOFull 比 DL_UART_isBusy 更适合用于连续发送
//     while (DL_UART_Main_isTXFIFOFull(UART_0_INST) == true)
//         ;

//     // 将字符发送出去
//     DL_UART_Main_transmitData(UART_0_INST, (uint8_t)ch);

//     // 对于 printf 来说，换行符 '\n' 需要我们手动补充一个回车符 '\r'
//     // 这样在串口助手中才能正确地换行和回到行首
//     if (ch == '\n')
//     {
//         while (DL_UART_Main_isTXFIFOFull(UART_0_INST) == true)
//             ;
//         DL_UART_Main_transmitData(UART_0_INST, '\r');
//     }

//     return ch;
// }
// imu_ahrs_handle_t imu_handle;
// // ================== 主函数 ==================
// int main(void)
// {
//     // 定义核心句柄和数据变量
//     imu_ahrs_config_t imu_config = {
//         .cal_buffer_size = 400,           // 校准缓冲区大小
//         .cal_variance_threshold = 0.017f, // 方差阈值
//     };
//     inv_imu_sensor_data_t sensor_data;
//     char display_buf[40];

//     float roll, pitch, yaw;

//     // 1. 初始化 MCU 时钟和 SysConfig 中配置的所有外设
//     SYSCFG_DL_init();
//     NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
//     // 2. 初始化 LCD
//     lcd_init_async_start();
//     key_init(&ENCODER_SWITCH_KEY, EN_KEY_PORT, EN_KEY_PIN_B18_PIN, 0);
//     // 等待LCD初始化完成，变为阻塞式，确保后续显示正常
//     while (!is_lcd_init_done())
//     {
//         // 等待...
//     }
//     LCD_Clear(BLACK);
//     LCD_ShowString(10, 10, (uint8_t *)"LCD OK! Initializing IMU...", YELLOW, BLACK, 16, 0);

//     // 3. 初始化 IMU 核心库
//     int init_status = imu_ahrs_init(&imu_handle, &imu_config, bsp_imu_get_hal());
//     if (init_status != 0)
//     {
//         // 如果初始化失败，显示错误码并停机
//         sprintf(display_buf, "FCode: %d   ", init_status);
//         LCD_ShowString(10, 30, (uint8_t *)display_buf, RED, BLACK, 16, 0);
//         while (1)
//             ; // 停机
//     }

//     // 初始化成功
//     LCD_ShowString(10, 30, (uint8_t *)"IMU Init OK!", GREEN, BLACK, 16, 0);
//     LCD_Clear(BLACK);
//     LCD_ShowString(10, 30, (uint8_t *)"IMU caling...", GREEN, BLACK, 16, 0);
//     while (imu_ahrs_get_cal_state(&imu_handle) != CAL_STATE_STATIONARY)
//     {
//         imu_ahrs_update(&imu_handle);
//         delay_ms(10);
//     }
//     LCD_Clear(BLACK);
//     // 4. 进入主循环，不断读取和显示数据
//     while (1)
//     {

//         {
//             imu_ahrs_update(&imu_handle);
//             if (key_get_state(&ENCODER_SWITCH_KEY) != KEY_STATE_PRESSED)
//             {
//                 imu_ahrs_get_euler_angles(&imu_handle, &roll, &pitch, &yaw);

//                 // 2. 调用正确的API获取原始数据
//                 int read_status = inv_imu_get_register_data(&imu_handle.device, &sensor_data);

//                 // 3. 显示欧拉角
//                 LCD_ShowString(10, 10, (uint8_t *)"Attitude Angle (Deg)", YELLOW, BLACK, 16, 0);
//                 sprintf(display_buf, "Pitch:%.2f", pitch);
//                 LCD_ShowString(10, 30, (uint8_t *)display_buf, WHITE, BLACK, 16, 0);
//                 sprintf(display_buf, "Roll :%.2f", roll);
//                 LCD_ShowString(10, 50, (uint8_t *)display_buf, WHITE, BLACK, 16, 0);
//                 sprintf(display_buf, "Yaw  :%.2f", yaw);
//                 LCD_ShowString(10, 70, (uint8_t *)display_buf, WHITE, BLACK, 16, 0);
//             }

//             else
//             {

//                 // 读取成功，格式化并显示6轴数据
//                 LCD_ShowString(10, 10, (uint8_t *)"IMU Raw Sensor Data", YELLOW, BLACK, 16, 0);

//                 // 显示加速度计三轴数据
//                 sprintf(display_buf, "ax:%7d", sensor_data.accel_data[0]);
//                 LCD_ShowString(10, 30, (uint8_t *)display_buf, WHITE, BLACK, 16, 0);
//                 sprintf(display_buf, "ay:%7d", sensor_data.accel_data[1]);
//                 LCD_ShowString(10, 50, (uint8_t *)display_buf, WHITE, BLACK, 16, 0);
//                 sprintf(display_buf, "az:%7d", sensor_data.accel_data[2]);
//                 LCD_ShowString(10, 70, (uint8_t *)display_buf, WHITE, BLACK, 16, 0);

//                 // 显示陀螺仪三轴数据
//                 sprintf(display_buf, "gx:%7d", sensor_data.gyro_data[0]);
//                 LCD_ShowString(10, 90, (uint8_t *)display_buf, CYAN, BLACK, 16, 0);
//                 sprintf(display_buf, "gy:%7d", sensor_data.gyro_data[1]);
//                 LCD_ShowString(10, 110, (uint8_t *)display_buf, CYAN, BLACK, 16, 0);
//                 sprintf(display_buf, "gz:%7d", sensor_data.gyro_data[2]);
//                 LCD_ShowString(10, 130, (uint8_t *)display_buf, CYAN, BLACK, 16, 0);
//             }
//         }

//         // 延时100毫秒，以大约10Hz的频率刷新屏幕，方便人眼观察
//         delay_ms(10);
//     }
// }
