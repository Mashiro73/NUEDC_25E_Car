/**
 * @file    lcd_init.h
 * @brief   LCD initialization: SPI communication and ST7735S init sequence
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

#define u8  unsigned char
#define u16 unsigned int
#define u32 unsigned long

#define USE_HORIZONTAL 1  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif



//-----------------LCD�˿ڶ���---------------- 

//#define LCD_SCLK_Clr() DL_GPIO_clearPins(LCD_SCL_PORT,LCD_SCL_PIN)//SCL=SCLK
//#define LCD_SCLK_Set() DL_GPIO_setPins(LCD_SCL_PORT,LCD_SCL_PIN)

//#define LCD_MOSI_Clr() DL_GPIO_clearPins(LCD_SDA_PORT,LCD_SDA_PIN)//SDA=MOSI
//#define LCD_MOSI_Set() DL_GPIO_setPins(LCD_SDA_PORT,LCD_SDA_PIN)

#define LCD_RES_Clr()  DL_GPIO_clearPins(LCD_PORT,LCD_RES_PIN)//RES
#define LCD_RES_Set()  DL_GPIO_setPins(LCD_PORT,LCD_RES_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(LCD_PORT,LCD_DC_PIN)//DC
#define LCD_DC_Set()   DL_GPIO_setPins(LCD_PORT,LCD_DC_PIN)

#define LCD_CS_Clr()   DL_GPIO_clearPins(LCD_PORT,LCD_CS_PIN)//CS
#define LCD_CS_Set()   DL_GPIO_setPins(LCD_PORT,LCD_CS_PIN)

#define LCD_BLK_Clr()  DL_GPIO_clearPins(LCD_PORT,LCD_BLK_PIN)//BLK
#define LCD_BLK_Set()  DL_GPIO_setPins(LCD_PORT,LCD_BLK_PIN)

/**
 * @brief �����첽������������LCD��ʼ�����̡�
 * @note  ��main������ʼʱ����һ�Ρ�
 */
void lcd_init_async_start(void);

/**
 * @brief LCD��ʼ��״̬����������
 * @note  ��Ҫ����ѭ����SysTick�б������Ե��á�
 */
void lcd_init_task(void);

/**
 * @brief ���LCD�Ƿ��Ѿ���ɳ�ʼ����
 * @return bool - �����ʼ������򷵻� true�����򷵻� false��
 */
bool is_lcd_init_done(void);

void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��
#endif




