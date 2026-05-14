/**
 * @file    lcd_init.c
 * @brief   LCD initialization: SPI communication and ST7735S init sequence
 * @author  Dinglei Xu
 * @date    2025
 */

#include "lcd_init.h"
#include <stdbool.h> // ��Ҫ���� bool ����

void LCD_GPIO_Init(void)
{
}

/******************************************************************************
	  ����˵����LCD��������д�뺯��
	  ������ݣ�dat  Ҫд��Ĵ�������
	  ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
	//	u8 i;
	//	LCD_CS_Clr();
	//	for(i=0;i<8;i++)
	//	{
	//		LCD_SCLK_Clr();
	//		if(dat&0x80)
	//		{
	//		   LCD_MOSI_Set();
	//		}
	//		else
	//		{
	//		   LCD_MOSI_Clr();
	//		}
	//		LCD_SCLK_Set();
	//		dat<<=1;
	//	}
	//  LCD_CS_Set();
	uint8_t recv_data = 0;

	LCD_CS_Clr();

	// ��������
	DL_SPI_transmitData8(SPI_LCD_INST, dat);
	// �ȴ�SPI���߿���
	while (DL_SPI_isBusy(SPI_LCD_INST))
		;

	// ��������
	recv_data = DL_SPI_receiveData8(SPI_LCD_INST);
	// �ȴ�SPI���߿���
	while (DL_SPI_isBusy(SPI_LCD_INST))
		;

	LCD_CS_Set();
}

/******************************************************************************
	  ����˵����LCDд������
	  ������ݣ�dat д�������
	  ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	  ����˵����LCDд������
	  ������ݣ�dat д�������
	  ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	  ����˵����LCDд������
	  ������ݣ�dat д�������
	  ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr(); // д����
	LCD_Writ_Bus(dat);
	LCD_DC_Set(); // д����
}

/******************************************************************************
	  ����˵����������ʼ�ͽ�����ַ
	  ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
				y1,y2 �����е���ʼ�ͽ�����ַ
	  ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
	if (USE_HORIZONTAL == 0)
	{
		LCD_WR_REG(0x2a); // �е�ַ����
		LCD_WR_DATA(x1 + 2);
		LCD_WR_DATA(x2 + 2);
		LCD_WR_REG(0x2b); // �е�ַ����
		LCD_WR_DATA(y1 + 1);
		LCD_WR_DATA(y2 + 1);
		LCD_WR_REG(0x2c); // ������д
	}
	else if (USE_HORIZONTAL == 1)
	{
		LCD_WR_REG(0x2a); // �е�ַ����
		LCD_WR_DATA(x1 + 2);
		LCD_WR_DATA(x2 + 2);
		LCD_WR_REG(0x2b); // �е�ַ����
		LCD_WR_DATA(y1 + 1);
		LCD_WR_DATA(y2 + 1);
		LCD_WR_REG(0x2c); // ������д
	}
	else if (USE_HORIZONTAL == 2)
	{
		LCD_WR_REG(0x2a); // �е�ַ����
		LCD_WR_DATA(x1 + 1);
		LCD_WR_DATA(x2 + 1);
		LCD_WR_REG(0x2b); // �е�ַ����
		LCD_WR_DATA(y1 + 2);
		LCD_WR_DATA(y2 + 2);
		LCD_WR_REG(0x2c); // ������д
	}
	else
	{
		LCD_WR_REG(0x2a); // �е�ַ����
		LCD_WR_DATA(x1 + 1);
		LCD_WR_DATA(x2 + 1);
		LCD_WR_REG(0x2b); // �е�ַ����
		LCD_WR_DATA(y1 + 2);
		LCD_WR_DATA(y2 + 2);
		LCD_WR_REG(0x2c); // ������д
	}
}

 void LCD_Init(void)
 {
 	LCD_GPIO_Init(); // ��ʼ��GPIO

 	LCD_RES_Clr(); // ��λ
 	delay_ms(100);
 	LCD_RES_Set();
 	delay_ms(100);

 	LCD_BLK_Set(); // �򿪱���
 	delay_ms(100);

 	//************* Start Initial Sequence **********//
 	LCD_WR_REG(0x11); // Sleep out
 	delay_ms(120);	  // Delay 120ms
 	//------------------------------------ST7735S Frame Rate-----------------------------------------//
 	LCD_WR_REG(0xB1);
 	LCD_WR_DATA8(0x05);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_REG(0xB2);
 	LCD_WR_DATA8(0x05);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_REG(0xB3);
 	LCD_WR_DATA8(0x05);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_DATA8(0x05);
 	LCD_WR_DATA8(0x3C);
 	LCD_WR_DATA8(0x3C);
 	//------------------------------------End ST7735S Frame Rate---------------------------------//
 	LCD_WR_REG(0xB4); // Dot inversion
 	LCD_WR_DATA8(0x03);
 	//------------------------------------ST7735S Power Sequence---------------------------------//
 	LCD_WR_REG(0xC0);
 	LCD_WR_DATA8(0x28);
 	LCD_WR_DATA8(0x08);
 	LCD_WR_DATA8(0x04);
 	LCD_WR_REG(0xC1);
 	LCD_WR_DATA8(0XC0);
 	LCD_WR_REG(0xC2);
 	LCD_WR_DATA8(0x0D);
 	LCD_WR_DATA8(0x00);
 	LCD_WR_REG(0xC3);
 	LCD_WR_DATA8(0x8D);
 	LCD_WR_DATA8(0x2A);
 	LCD_WR_REG(0xC4);
 	LCD_WR_DATA8(0x8D);
 	LCD_WR_DATA8(0xEE);
 	//---------------------------------End ST7735S Power Sequence-------------------------------------//
 	LCD_WR_REG(0xC5); // VCOM
 	LCD_WR_DATA8(0x1A);
 	LCD_WR_REG(0x36); // MX, MY, RGB mode
 	if (USE_HORIZONTAL == 0)
 		LCD_WR_DATA8(0x00);
 	else if (USE_HORIZONTAL == 1)
 		LCD_WR_DATA8(0xC0);
 	else if (USE_HORIZONTAL == 2)
 		LCD_WR_DATA8(0x70);
 	else
 		LCD_WR_DATA8(0xA0);
 	//------------------------------------ST7735S Gamma Sequence---------------------------------//
 	LCD_WR_REG(0xE0);
 	LCD_WR_DATA8(0x04);
 	LCD_WR_DATA8(0x22);
 	LCD_WR_DATA8(0x07);
 	LCD_WR_DATA8(0x0A);
 	LCD_WR_DATA8(0x2E);
 	LCD_WR_DATA8(0x30);
 	LCD_WR_DATA8(0x25);
 	LCD_WR_DATA8(0x2A);
 	LCD_WR_DATA8(0x28);
 	LCD_WR_DATA8(0x26);
 	LCD_WR_DATA8(0x2E);
 	LCD_WR_DATA8(0x3A);
 	LCD_WR_DATA8(0x00);
 	LCD_WR_DATA8(0x01);
 	LCD_WR_DATA8(0x03);
 	LCD_WR_DATA8(0x13);
 	LCD_WR_REG(0xE1);
 	LCD_WR_DATA8(0x04);
 	LCD_WR_DATA8(0x16);
 	LCD_WR_DATA8(0x06);
 	LCD_WR_DATA8(0x0D);
 	LCD_WR_DATA8(0x2D);
 	LCD_WR_DATA8(0x26);
 	LCD_WR_DATA8(0x23);
 	LCD_WR_DATA8(0x27);
 	LCD_WR_DATA8(0x27);
 	LCD_WR_DATA8(0x25);
 	LCD_WR_DATA8(0x2D);
 	LCD_WR_DATA8(0x3B);
 	LCD_WR_DATA8(0x00);
 	LCD_WR_DATA8(0x01);
 	LCD_WR_DATA8(0x04);
 	LCD_WR_DATA8(0x13);
 	//------------------------------------End ST7735S Gamma Sequence-----------------------------//
 	LCD_WR_REG(0x3A); // 65k mode
 	LCD_WR_DATA8(0x05);
 	LCD_WR_REG(0x29); // Display on
 }



// 1. ����LCD��ʼ�������С����衱��״̬��
typedef enum
{
	LCD_INIT_START,
	LCD_INIT_RESET_LOW,
	LCD_INIT_WAIT1, // �ȴ�100ms
	LCD_INIT_RESET_HIGH,
	LCD_INIT_WAIT2, // �ȴ�100ms
	LCD_INIT_BLK_ON,
	LCD_INIT_WAIT3, // �ȴ�100ms
	LCD_INIT_SLEEP_OUT,
	LCD_INIT_WAIT4,			// �ȴ�120ms
	LCD_INIT_SEND_SEQUENCE, // ����һϵ������ָ��
	LCD_INIT_DONE			// ��ʼ�����
} LcdInitState_e;

// 2. ���徲̬���������ٵ�ǰ״̬�ͼ�ʱ
static LcdInitState_e g_lcdInitState = LCD_INIT_START;
static uint32_t g_lcdTimer = 0;

// ������ʼ��
void lcd_init_async_start(void)
{
	g_lcdInitState = LCD_INIT_START;
}

// ����Ƿ����
bool is_lcd_init_done(void)
{
	return (g_lcdInitState == LCD_INIT_DONE);
}

// LCD��ʼ��״̬��������ĺ���
void lcd_init_task(void)
{
	// ����Ѿ���ʼ����ɣ���ֱ�ӷ���
	if (g_lcdInitState == LCD_INIT_DONE)
	{
		return;
	}

	// �����ʱ�����ڼ�ʱ����ݼ�����������
	// ������������ڱ������Ե��ã����磬��SysTickÿ5ms����һ�Σ�
	if (g_lcdTimer > 0)
	{
		g_lcdTimer--;
		return;
	}

	// ���ݵ�ǰ״̬��ִ��һС��������Ȼ��ת������һ��״̬
	switch (g_lcdInitState)
	{
	case LCD_INIT_START:
		// �����￪ʼ
		g_lcdInitState = LCD_INIT_RESET_LOW;
		break;

	case LCD_INIT_RESET_LOW:
		LCD_RES_Clr();	 // ��λ
		g_lcdTimer = 20; // ������ʱ: 20 * 5ms = 100ms
		g_lcdInitState = LCD_INIT_WAIT1;
		break;

	case LCD_INIT_WAIT1: // �ȴ�100ms����
		g_lcdInitState = LCD_INIT_RESET_HIGH;
		break;

	case LCD_INIT_RESET_HIGH:
		LCD_RES_Set();
		g_lcdTimer = 20; // ������ʱ: 20 * 5ms = 100ms
		g_lcdInitState = LCD_INIT_WAIT2;
		break;

	case LCD_INIT_WAIT2: // �ȴ�100ms����
		g_lcdInitState = LCD_INIT_BLK_ON;
		break;

	case LCD_INIT_BLK_ON:
		LCD_BLK_Set();	 // �򿪱���
		g_lcdTimer = 20; // ������ʱ: 20 * 5ms = 100ms
		g_lcdInitState = LCD_INIT_WAIT3;
		break;

	case LCD_INIT_WAIT3: // �ȴ�100ms����
		g_lcdInitState = LCD_INIT_SLEEP_OUT;
		break;

	case LCD_INIT_SLEEP_OUT:
		LCD_WR_REG(0x11); // Sleep out
		g_lcdTimer = 24;  // ������ʱ: 24 * 5ms = 120ms
		g_lcdInitState = LCD_INIT_WAIT4;
		break;

	case LCD_INIT_WAIT4: // �ȴ�120ms����
		g_lcdInitState = LCD_INIT_SEND_SEQUENCE;
		break;

	case LCD_INIT_SEND_SEQUENCE:
		// ���������һ���԰����в���Ҫ��ʱ��ָ��ȫ�����ͳ�ȥ
		// ��Ϊ��Щ�����ܿ죬�����������
		//------------------------------------ST7735S Frame Rate-----------------------------------------//
		LCD_WR_REG(0xB1);
		LCD_WR_DATA8(0x05);
		LCD_WR_DATA8(0x3C);
		LCD_WR_DATA8(0x3C);
		LCD_WR_REG(0xB2);
		LCD_WR_DATA8(0x05);
		LCD_WR_DATA8(0x3C);
		LCD_WR_DATA8(0x3C);
		LCD_WR_REG(0xB3);
		LCD_WR_DATA8(0x05);
		LCD_WR_DATA8(0x3C);
		LCD_WR_DATA8(0x3C);
		LCD_WR_DATA8(0x05);
		LCD_WR_DATA8(0x3C);
		LCD_WR_DATA8(0x3C);
		//------------------------------------End ST7735S Frame Rate---------------------------------//
		LCD_WR_REG(0xB4);
		LCD_WR_DATA8(0x03);
		//------------------------------------ST7735S Power Sequence---------------------------------//
		LCD_WR_REG(0xC0);
		LCD_WR_DATA8(0x28);
		LCD_WR_DATA8(0x08);
		LCD_WR_DATA8(0x04);
		LCD_WR_REG(0xC1);
		LCD_WR_DATA8(0XC0);
		LCD_WR_REG(0xC2);
		LCD_WR_DATA8(0x0D);
		LCD_WR_DATA8(0x00);
		LCD_WR_REG(0xC3);
		LCD_WR_DATA8(0x8D);
		LCD_WR_DATA8(0x2A);
		LCD_WR_REG(0xC4);
		LCD_WR_DATA8(0x8D);
		LCD_WR_DATA8(0xEE);
		//---------------------------------End ST7735S Power Sequence-------------------------------------//
		LCD_WR_REG(0xC5);
		LCD_WR_DATA8(0x1A);
		LCD_WR_REG(0x36);
		if (USE_HORIZONTAL == 0)
			LCD_WR_DATA8(0x00);
		else if (USE_HORIZONTAL == 1)
			LCD_WR_DATA8(0xC0);
		else if (USE_HORIZONTAL == 2)
			LCD_WR_DATA8(0x70);
		else
			LCD_WR_DATA8(0xA0);
		//------------------------------------ST7735S Gamma Sequence---------------------------------//
		LCD_WR_REG(0xE0);
		LCD_WR_DATA8(0x04);
		LCD_WR_DATA8(0x22);
		LCD_WR_DATA8(0x07);
		LCD_WR_DATA8(0x0A);
		LCD_WR_DATA8(0x2E);
		LCD_WR_DATA8(0x30);
		LCD_WR_DATA8(0x25);
		LCD_WR_DATA8(0x2A);
		LCD_WR_DATA8(0x28);
		LCD_WR_DATA8(0x26);
		LCD_WR_DATA8(0x2E);
		LCD_WR_DATA8(0x3A);
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x01);
		LCD_WR_DATA8(0x03);
		LCD_WR_DATA8(0x13);
		LCD_WR_REG(0xE1);
		LCD_WR_DATA8(0x04);
		LCD_WR_DATA8(0x16);
		LCD_WR_DATA8(0x06);
		LCD_WR_DATA8(0x0D);
		LCD_WR_DATA8(0x2D);
		LCD_WR_DATA8(0x26);
		LCD_WR_DATA8(0x23);
		LCD_WR_DATA8(0x27);
		LCD_WR_DATA8(0x27);
		LCD_WR_DATA8(0x25);
		LCD_WR_DATA8(0x2D);
		LCD_WR_DATA8(0x3B);
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x01);
		LCD_WR_DATA8(0x04);
		LCD_WR_DATA8(0x13);
		//------------------------------------End ST7735S Gamma Sequence-----------------------------//
		LCD_WR_REG(0x3A);
		LCD_WR_DATA8(0x05);
		LCD_WR_REG(0x29); // Display on

		// ����ָ�����ϣ���������״̬
		g_lcdInitState = LCD_INIT_DONE;
		break;

	case LCD_INIT_DONE:
		// ��ʼ������ɣ�����ִ���κβ���
		break;
	}
}