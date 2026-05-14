/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000



/* Defines for Motor_PWM */
#define Motor_PWM_INST                                                     TIMG7
#define Motor_PWM_INST_IRQHandler                               TIMG7_IRQHandler
#define Motor_PWM_INST_INT_IRQN                                 (TIMG7_INT_IRQn)
#define Motor_PWM_INST_CLK_FREQ                                         20000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM_C0_PORT                                             GPIOA
#define GPIO_Motor_PWM_C0_PIN                                     DL_GPIO_PIN_23
#define GPIO_Motor_PWM_C0_IOMUX                                  (IOMUX_PINCM53)
#define GPIO_Motor_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM53_PF_TIMG7_CCP0
#define GPIO_Motor_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM_C1_PORT                                             GPIOA
#define GPIO_Motor_PWM_C1_PIN                                     DL_GPIO_PIN_18
#define GPIO_Motor_PWM_C1_IOMUX                                  (IOMUX_PINCM40)
#define GPIO_Motor_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM40_PF_TIMG7_CCP1
#define GPIO_Motor_PWM_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMG0)
#define TIMER_1_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                           (199U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_4_MHZ_115200_BAUD                                        (2)
#define UART_0_FBRD_4_MHZ_115200_BAUD                                       (11)
/* Defines for UART_1 */
#define UART_1_INST                                                        UART3
#define UART_1_INST_IRQHandler                                  UART3_IRQHandler
#define UART_1_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOB
#define GPIO_UART_1_RX_PIN                                        DL_GPIO_PIN_25
#define GPIO_UART_1_TX_PIN                                        DL_GPIO_PIN_12
#define GPIO_UART_1_IOMUX_RX                                     (IOMUX_PINCM55)
#define GPIO_UART_1_IOMUX_TX                                     (IOMUX_PINCM29)
#define GPIO_UART_1_IOMUX_RX_FUNC                      IOMUX_PINCM55_PF_UART3_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                      IOMUX_PINCM29_PF_UART3_TX
#define UART_1_BAUD_RATE                                                (115200)
#define UART_1_IBRD_80_MHZ_115200_BAUD                                      (43)
#define UART_1_FBRD_80_MHZ_115200_BAUD                                      (26)




/* Defines for SPI_LCD */
#define SPI_LCD_INST                                                       SPI1
#define SPI_LCD_INST_IRQHandler                                 SPI1_IRQHandler
#define SPI_LCD_INST_INT_IRQN                                     SPI1_INT_IRQn
#define GPIO_SPI_LCD_PICO_PORT                                            GPIOB
#define GPIO_SPI_LCD_PICO_PIN                                    DL_GPIO_PIN_15
#define GPIO_SPI_LCD_IOMUX_PICO                                 (IOMUX_PINCM32)
#define GPIO_SPI_LCD_IOMUX_PICO_FUNC                 IOMUX_PINCM32_PF_SPI1_PICO
#define GPIO_SPI_LCD_POCI_PORT                                            GPIOB
#define GPIO_SPI_LCD_POCI_PIN                                    DL_GPIO_PIN_14
#define GPIO_SPI_LCD_IOMUX_POCI                                 (IOMUX_PINCM31)
#define GPIO_SPI_LCD_IOMUX_POCI_FUNC                 IOMUX_PINCM31_PF_SPI1_POCI
/* GPIO configuration for SPI_LCD */
#define GPIO_SPI_LCD_SCLK_PORT                                            GPIOA
#define GPIO_SPI_LCD_SCLK_PIN                                    DL_GPIO_PIN_17
#define GPIO_SPI_LCD_IOMUX_SCLK                                 (IOMUX_PINCM39)
#define GPIO_SPI_LCD_IOMUX_SCLK_FUNC                 IOMUX_PINCM39_PF_SPI1_SCLK
/* Defines for SPI_IMU */
#define SPI_IMU_INST                                                       SPI0
#define SPI_IMU_INST_IRQHandler                                 SPI0_IRQHandler
#define SPI_IMU_INST_INT_IRQN                                     SPI0_INT_IRQn
#define GPIO_SPI_IMU_PICO_PORT                                            GPIOA
#define GPIO_SPI_IMU_PICO_PIN                                    DL_GPIO_PIN_14
#define GPIO_SPI_IMU_IOMUX_PICO                                 (IOMUX_PINCM36)
#define GPIO_SPI_IMU_IOMUX_PICO_FUNC                 IOMUX_PINCM36_PF_SPI0_PICO
#define GPIO_SPI_IMU_POCI_PORT                                            GPIOA
#define GPIO_SPI_IMU_POCI_PIN                                    DL_GPIO_PIN_13
#define GPIO_SPI_IMU_IOMUX_POCI                                 (IOMUX_PINCM35)
#define GPIO_SPI_IMU_IOMUX_POCI_FUNC                 IOMUX_PINCM35_PF_SPI0_POCI
/* GPIO configuration for SPI_IMU */
#define GPIO_SPI_IMU_SCLK_PORT                                            GPIOA
#define GPIO_SPI_IMU_SCLK_PIN                                    DL_GPIO_PIN_12
#define GPIO_SPI_IMU_IOMUX_SCLK                                 (IOMUX_PINCM34)
#define GPIO_SPI_IMU_IOMUX_SCLK_FUNC                 IOMUX_PINCM34_PF_SPI0_SCLK



/* Defines for ADC_VAL */
#define ADC_VAL_INST                                                        ADC1
#define ADC_VAL_INST_IRQHandler                                  ADC1_IRQHandler
#define ADC_VAL_INST_INT_IRQN                                    (ADC1_INT_IRQn)
#define ADC_VAL_ADCMEM_ADC_CH0                                DL_ADC12_MEM_IDX_0
#define ADC_VAL_ADCMEM_ADC_CH0_REF               DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_VAL_ADCMEM_ADC_CH0_REF_VOLTAGE_V                                     3.3
#define GPIO_ADC_VAL_C0_PORT                                               GPIOA
#define GPIO_ADC_VAL_C0_PIN                                       DL_GPIO_PIN_15



/* Port definition for Pin Group BEEP */
#define BEEP_PORT                                                        (GPIOB)

/* Defines for USER_BEEP: GPIOB.27 with pinCMx 58 on package pin 29 */
#define BEEP_USER_BEEP_PIN                                      (DL_GPIO_PIN_27)
#define BEEP_USER_BEEP_IOMUX                                     (IOMUX_PINCM58)
/* Port definition for Pin Group IMU_CS */
#define IMU_CS_PORT                                                      (GPIOB)

/* Defines for PIN_CS: GPIOB.25 with pinCMx 56 on package pin 27 */
#define IMU_CS_PIN_CS_PIN                                       (DL_GPIO_PIN_25)
#define IMU_CS_PIN_CS_IOMUX                                      (IOMUX_PINCM56)
/* Port definition for Pin Group GPIO_LEDS */
#define GPIO_LEDS_PORT                                                   (GPIOA)

/* Defines for USER_LED_3: GPIOA.29 with pinCMx 4 on package pin 36 */
#define GPIO_LEDS_USER_LED_3_PIN                                (DL_GPIO_PIN_29)
#define GPIO_LEDS_USER_LED_3_IOMUX                                (IOMUX_PINCM4)
/* Defines for USER_LED_4: GPIOA.30 with pinCMx 5 on package pin 37 */
#define GPIO_LEDS_USER_LED_4_PIN                                (DL_GPIO_PIN_30)
#define GPIO_LEDS_USER_LED_4_IOMUX                                (IOMUX_PINCM5)
/* Port definition for Pin Group LCD */
#define LCD_PORT                                                         (GPIOB)

/* Defines for RES: GPIOB.16 with pinCMx 33 on package pin 4 */
#define LCD_RES_PIN                                             (DL_GPIO_PIN_16)
#define LCD_RES_IOMUX                                            (IOMUX_PINCM33)
/* Defines for CS: GPIOB.20 with pinCMx 48 on package pin 19 */
#define LCD_CS_PIN                                              (DL_GPIO_PIN_20)
#define LCD_CS_IOMUX                                             (IOMUX_PINCM48)
/* Defines for BLK: GPIOB.22 with pinCMx 50 on package pin 21 */
#define LCD_BLK_PIN                                             (DL_GPIO_PIN_22)
#define LCD_BLK_IOMUX                                            (IOMUX_PINCM50)
/* Defines for DC: GPIOB.17 with pinCMx 43 on package pin 14 */
#define LCD_DC_PIN                                              (DL_GPIO_PIN_17)
#define LCD_DC_IOMUX                                             (IOMUX_PINCM43)
/* Defines for SENSOR_0: GPIOB.11 with pinCMx 28 on package pin 63 */
#define GPIO_SENSOR_0_PORT                                               (GPIOB)
#define GPIO_SENSOR_0_PIN                                       (DL_GPIO_PIN_11)
#define GPIO_SENSOR_0_IOMUX                                      (IOMUX_PINCM28)
/* Defines for SENSOR_1: GPIOB.9 with pinCMx 26 on package pin 61 */
#define GPIO_SENSOR_1_PORT                                               (GPIOB)
#define GPIO_SENSOR_1_PIN                                        (DL_GPIO_PIN_9)
#define GPIO_SENSOR_1_IOMUX                                      (IOMUX_PINCM26)
/* Defines for SENSOR_2: GPIOB.5 with pinCMx 18 on package pin 53 */
#define GPIO_SENSOR_2_PORT                                               (GPIOB)
#define GPIO_SENSOR_2_PIN                                        (DL_GPIO_PIN_5)
#define GPIO_SENSOR_2_IOMUX                                      (IOMUX_PINCM18)
/* Defines for SENSOR_3: GPIOB.7 with pinCMx 24 on package pin 59 */
#define GPIO_SENSOR_3_PORT                                               (GPIOB)
#define GPIO_SENSOR_3_PIN                                        (DL_GPIO_PIN_7)
#define GPIO_SENSOR_3_IOMUX                                      (IOMUX_PINCM24)
/* Defines for SENSOR_4: GPIOB.3 with pinCMx 16 on package pin 51 */
#define GPIO_SENSOR_4_PORT                                               (GPIOB)
#define GPIO_SENSOR_4_PIN                                        (DL_GPIO_PIN_3)
#define GPIO_SENSOR_4_IOMUX                                      (IOMUX_PINCM16)
/* Defines for SENSOR_5: GPIOA.7 with pinCMx 14 on package pin 49 */
#define GPIO_SENSOR_5_PORT                                               (GPIOA)
#define GPIO_SENSOR_5_PIN                                        (DL_GPIO_PIN_7)
#define GPIO_SENSOR_5_IOMUX                                      (IOMUX_PINCM14)
/* Defines for SENSOR_6: GPIOB.0 with pinCMx 12 on package pin 47 */
#define GPIO_SENSOR_6_PORT                                               (GPIOB)
#define GPIO_SENSOR_6_PIN                                        (DL_GPIO_PIN_0)
#define GPIO_SENSOR_6_IOMUX                                      (IOMUX_PINCM12)
/* Defines for SENSOR_7: GPIOA.31 with pinCMx 6 on package pin 39 */
#define GPIO_SENSOR_7_PORT                                               (GPIOA)
#define GPIO_SENSOR_7_PIN                                       (DL_GPIO_PIN_31)
#define GPIO_SENSOR_7_IOMUX                                       (IOMUX_PINCM6)
/* Defines for s1: GPIOB.21 with pinCMx 49 on package pin 20 */
#define key_s1_PORT                                                      (GPIOB)
#define key_s1_PIN                                              (DL_GPIO_PIN_21)
#define key_s1_IOMUX                                             (IOMUX_PINCM49)
/* Defines for s2: GPIOB.23 with pinCMx 51 on package pin 22 */
#define key_s2_PORT                                                      (GPIOB)
#define key_s2_PIN                                              (DL_GPIO_PIN_23)
#define key_s2_IOMUX                                             (IOMUX_PINCM51)
/* Defines for s3: GPIOB.24 with pinCMx 52 on package pin 23 */
#define key_s3_PORT                                                      (GPIOB)
#define key_s3_PIN                                              (DL_GPIO_PIN_24)
#define key_s3_IOMUX                                             (IOMUX_PINCM52)
/* Defines for s4: GPIOA.24 with pinCMx 54 on package pin 25 */
#define key_s4_PORT                                                      (GPIOA)
#define key_s4_PIN                                              (DL_GPIO_PIN_24)
#define key_s4_IOMUX                                             (IOMUX_PINCM54)
/* Port definition for Pin Group RGB */
#define RGB_PORT                                                         (GPIOA)

/* Defines for PIN_R: GPIOA.26 with pinCMx 59 on package pin 30 */
#define RGB_PIN_R_PIN                                           (DL_GPIO_PIN_26)
#define RGB_PIN_R_IOMUX                                          (IOMUX_PINCM59)
/* Defines for PIN_G: GPIOA.27 with pinCMx 60 on package pin 31 */
#define RGB_PIN_G_PIN                                           (DL_GPIO_PIN_27)
#define RGB_PIN_G_IOMUX                                          (IOMUX_PINCM60)
/* Defines for PIN_B: GPIOA.28 with pinCMx 3 on package pin 35 */
#define RGB_PIN_B_PIN                                           (DL_GPIO_PIN_28)
#define RGB_PIN_B_IOMUX                                           (IOMUX_PINCM3)
/* Port definition for Pin Group E1 */
#define E1_PORT                                                          (GPIOA)

/* Defines for E1A: GPIOA.8 with pinCMx 19 on package pin 54 */
// pins affected by this interrupt request:["E1A","E1B"]
#define E1_INT_IRQN                                             (GPIOA_INT_IRQn)
#define E1_INT_IIDX                             (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define E1_E1A_IIDX                                          (DL_GPIO_IIDX_DIO8)
#define E1_E1A_PIN                                               (DL_GPIO_PIN_8)
#define E1_E1A_IOMUX                                             (IOMUX_PINCM19)
/* Defines for E1B: GPIOA.9 with pinCMx 20 on package pin 55 */
#define E1_E1B_IIDX                                          (DL_GPIO_IIDX_DIO9)
#define E1_E1B_PIN                                               (DL_GPIO_PIN_9)
#define E1_E1B_IOMUX                                             (IOMUX_PINCM20)
/* Port definition for Pin Group E2 */
#define E2_PORT                                                          (GPIOB)

/* Defines for E2A: GPIOB.13 with pinCMx 30 on package pin 1 */
// pins affected by this interrupt request:["E2A","E2B"]
#define E2_INT_IRQN                                             (GPIOB_INT_IRQn)
#define E2_INT_IIDX                             (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define E2_E2A_IIDX                                         (DL_GPIO_IIDX_DIO13)
#define E2_E2A_PIN                                              (DL_GPIO_PIN_13)
#define E2_E2A_IOMUX                                             (IOMUX_PINCM30)
/* Defines for E2B: GPIOB.2 with pinCMx 15 on package pin 50 */
#define E2_E2B_IIDX                                          (DL_GPIO_IIDX_DIO2)
#define E2_E2B_PIN                                               (DL_GPIO_PIN_2)
#define E2_E2B_IOMUX                                             (IOMUX_PINCM15)
/* Port definition for Pin Group AIN */
#define AIN_PORT                                                         (GPIOA)

/* Defines for AIN1: GPIOA.21 with pinCMx 46 on package pin 17 */
#define AIN_AIN1_PIN                                            (DL_GPIO_PIN_21)
#define AIN_AIN1_IOMUX                                           (IOMUX_PINCM46)
/* Defines for AIN2: GPIOA.22 with pinCMx 47 on package pin 18 */
#define AIN_AIN2_PIN                                            (DL_GPIO_PIN_22)
#define AIN_AIN2_IOMUX                                           (IOMUX_PINCM47)
/* Port definition for Pin Group BIN */
#define BIN_PORT                                                         (GPIOB)

/* Defines for BIN2: GPIOB.10 with pinCMx 27 on package pin 62 */
#define BIN_BIN2_PIN                                            (DL_GPIO_PIN_10)
#define BIN_BIN2_IOMUX                                           (IOMUX_PINCM27)
/* Defines for BIN1: GPIOB.8 with pinCMx 25 on package pin 60 */
#define BIN_BIN1_PIN                                             (DL_GPIO_PIN_8)
#define BIN_BIN1_IOMUX                                           (IOMUX_PINCM25)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_Motor_PWM_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_UART_1_init(void);
void SYSCFG_DL_SPI_LCD_init(void);
void SYSCFG_DL_SPI_IMU_init(void);
void SYSCFG_DL_ADC_VAL_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
