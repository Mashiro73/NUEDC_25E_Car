/**
 * @file    motor.c
 * @brief   Motor driver: PWM control, encoder interrupt, PID speed regulation
 * @author  Dinglei Xu
 * @date    2025
 */

#include "motor.h"
#include "pid.h"

int Speed_L, Speed_R;
// 在 motor.c 文件顶部，Speed_L 和 Speed_R 的声明下方
volatile int32_t g_encoder_total_L = 0;
volatile int32_t g_encoder_total_R = 0;
/**
 * @brief 初始化电机驱动模块
 * @details 将电机驱动的引脚设置为安全的刹车状态，并清零PWM输出。
 */
void motor_init(void)
{
    // 将两个电机的方向控制引脚都设为低电平，实现刹车
    DL_GPIO_clearPins(AIN_PORT, AIN_AIN1_PIN | AIN_AIN2_PIN);
    DL_GPIO_clearPins(BIN_PORT, BIN_BIN1_PIN | BIN_BIN2_PIN);

    // 将PWM占空比设置为0
    DL_TimerG_setCaptureCompareValue(Motor_PWM_INST, 0, GPIO_Motor_PWM_C1_IDX); // 左轮
    DL_TimerG_setCaptureCompareValue(Motor_PWM_INST, 0, GPIO_Motor_PWM_C0_IDX); // 右轮
}

void motor_set_pulse(Motor_ID motor, int16_t speed)
{
    if (motor == MOTOR_A) // MOTOR_A 对应左轮
    {
		if(speed>=0)
		{
			DL_GPIO_clearPins(AIN_PORT,AIN_AIN1_PIN);
			DL_GPIO_setPins(AIN_PORT,AIN_AIN2_PIN);
			DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,speed,GPIO_Motor_PWM_C0_IDX);
		}
		else
		{
			
			DL_GPIO_clearPins(AIN_PORT,AIN_AIN2_PIN);
			DL_GPIO_setPins(AIN_PORT,AIN_AIN1_PIN);
			DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,-speed,GPIO_Motor_PWM_C0_IDX);
		}
    }
    else if (motor == MOTOR_B) // MOTOR_B 对应右轮
    {
		if(speed>=0)
		{
			DL_GPIO_clearPins(BIN_PORT,BIN_BIN2_PIN);
			DL_GPIO_setPins(BIN_PORT,BIN_BIN1_PIN);
			DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,speed,GPIO_Motor_PWM_C1_IDX);
		}
		else
		{
			DL_GPIO_clearPins(BIN_PORT,BIN_BIN1_PIN);
			DL_GPIO_setPins(BIN_PORT,BIN_BIN2_PIN);
			
			

			DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,-speed,GPIO_Motor_PWM_C1_IDX);
		}
    }
}

/**
 * @brief 使指定电机刹车
 * @param motor 要刹车的电机 (MOTOR_A 或 MOTOR_B)
 */
void motor_brake(Motor_ID motor)
{
    // 刹车即等同于将目标速度设置为0
    motor_set_pulse(motor, 0);
}

//void Motor_Output()
//{
//	int Output_L=0;
//	int Output_R=0;
//	
//	Output_L=PID_Control_Inc(&PID_Motor_L,1);
//	Output_R=PID_Control_Inc(&PID_Motor_R,1);
//	
//	if(Output_L>=0)
//	{
//		DL_GPIO_clearPins(AIN_PORT,AIN_AIN2_PIN);
//		DL_GPIO_setPins(AIN_PORT,AIN_AIN1_PIN);
//		DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,Output_L,GPIO_Motor_PWM_C1_IDX);
//	}
//	else
//	{
//		DL_GPIO_clearPins(AIN_PORT,AIN_AIN1_PIN);
//		DL_GPIO_setPins(AIN_PORT,AIN_AIN2_PIN);
//		DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,-Output_L,GPIO_Motor_PWM_C1_IDX);
//	}

//	if(Output_R>=0)
//	{
//		DL_GPIO_clearPins(BIN_PORT,BIN_BIN2_PIN);
//		DL_GPIO_setPins(BIN_PORT,BIN_BIN1_PIN);
//		DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,Output_R,GPIO_Motor_PWM_C0_IDX);
//	}
//	else
//	{
//		DL_GPIO_clearPins(BIN_PORT,BIN_BIN1_PIN);
//		DL_GPIO_setPins(BIN_PORT,BIN_BIN2_PIN);
//		DL_TimerG_setCaptureCompareValue(Motor_PWM_INST,-Output_R,GPIO_Motor_PWM_C0_IDX);
//	}
//}

void Motor_GetSpeed()
{
	PID_Motor_L.ActualValue=Speed_L*2*Wheel_radius*3.14*1000/Full_turnPulse/Sampling_time;
	PID_Motor_R.ActualValue=Speed_R*2*Wheel_radius*3.14*1000/Full_turnPulse/Sampling_time;
	Speed_L=0;
	Speed_R=0;
}

void GROUP1_IRQHandler(void)
{
	int E1Status=0;
	int E2Status=0;
	switch( DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) )
    {
        case E1_INT_IIDX: // 左轮 (E1) 编码器中断
			E1Status = DL_GPIO_getEnabledInterruptStatus(E1_PORT,E1_E1A_PIN | E1_E1B_PIN);
			if ((E1Status & E1_E1A_PIN) ==E1_E1A_PIN) 
			{
				if( DL_GPIO_readPins(E1_PORT, E1_E1B_PIN) > 0 )
				{
					Speed_L++;
				}
				else
				{
					Speed_L--;
				}

				DL_GPIO_clearInterruptStatus(E1_PORT, E1_E1A_PIN);
			}
 
			if ((E1Status & E1_E1B_PIN) == E1_E1B_PIN) 
			{
				if(DL_GPIO_readPins(E1_PORT,E1_E1A_PIN)>0)
				{
					Speed_L--;
				}
				else
				{
					Speed_L++;
				}
				DL_GPIO_clearInterruptStatus(E1_PORT, E1_E1B_PIN);
			}
			break;

		case E2_INT_IIDX: // 右轮 (E2) 编码器中断
			E2Status = DL_GPIO_getEnabledInterruptStatus(E2_PORT,E2_E2A_PIN | E2_E2B_PIN);
			if ((E2Status & E2_E2A_PIN) ==E2_E2A_PIN) 
			{
				if( DL_GPIO_readPins(E2_PORT, E2_E2B_PIN) > 0 )
				{
                    Speed_R--;
                }
				else
				{
					Speed_R++;
				}

				DL_GPIO_clearInterruptStatus(E2_PORT, E2_E2A_PIN);
			}
 
			if ((E2Status & E2_E2B_PIN) == E2_E2B_PIN) 
			{
				if(DL_GPIO_readPins(E2_PORT,E2_E2A_PIN)>0)
				{
					Speed_R++;
				}
				else
				{
                    Speed_R--;
                }
				DL_GPIO_clearInterruptStatus(E2_PORT, E2_E2B_PIN);
			}
			break;
    }
}
void Motor_Stop()
{
	motor_set_pulse(0,0);
	motor_set_pulse(1,0);
}

void uart_printf(UART_Regs *uart, const char *format, ...);

/**
 * @brief 更新并执行双轮的【串级PID】闭环控制
 * @details 外环为位置环，内环为速度环。该函数应被周期性调用。
 */
void motor_speed_pid_update(void)
{
    PID_Motor_L.ActualValue = Speed_L;
    PID_Motor_R.ActualValue = Speed_R;

    // c. 清零瞬时速度计数，为下一周期准备
    Speed_L = 0;
    Speed_R = 0;


    // b. 计算速度环，得到最终的PWM输出
    int16_t pwm_l = (int16_t)PID_Control_Pos(&PID_Motor_L, 1);
    int16_t pwm_r = (int16_t)PID_Control_Pos(&PID_Motor_R, 1);

    // --- 4. 将最终PWM值应用到电机 ---
    motor_set_pulse(MOTOR_A, pwm_l); // 左轮
    motor_set_pulse(MOTOR_B, pwm_r); // 右轮
}
