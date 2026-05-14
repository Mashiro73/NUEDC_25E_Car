/**
 * @file    pid.c
 * @brief   Incremental and positional PID controller implementation
 * @author  Dinglei Xu
 * @date    2025
 */

#include "pid.h"

volatile PID_DataStruct PID_Motor_L = {0};      // Left Speed PID
volatile PID_DataStruct PID_Motor_R = {0};      // Right Speed PID
volatile PID_DataStruct PID_Angle = {0};        // Differential PID
volatile PID_DataStruct PID_Differential = {0}; // Differential PID
volatile PID_DataStruct PID_LineTracker = {0};  // 循迹PID
volatile PID_DataStruct PID_Position_L = {0}; // 左轮位置环
volatile PID_DataStruct PID_Position_R = {0}; // 右轮位置环

void PID_Init()
{
    PID_Motor_L.KP = 5.0;
    PID_Motor_L.KI = 4;
    PID_Motor_L.KD = 0;
    PID_Motor_L.PIDmax = 600;
    PID_Motor_L.PIDmin = -600;
    PID_Motor_L.PIDout = 0;
	PID_Motor_L.Sum_max = 500;
	PID_Motor_L.Sum_min = 0;

    PID_Motor_R.KP = 5.0;
    PID_Motor_R.KI = 4;
    PID_Motor_R.KD = 0;
    PID_Motor_R.PIDmax = PID_Motor_L.PIDmax;
    PID_Motor_R.PIDmin = PID_Motor_L.PIDmin;
    PID_Motor_R.PIDout = 0;
	PID_Motor_R.Sum_max = 500;
	PID_Motor_R.Sum_min = 0;

    // 初始化左轮位置环PID参数 (通常位置环以P控制为主)
    PID_Position_L.KP = 0.5;     // 比例系数，核心参数，决定响应速度
    PID_Position_L.KI = 0.0;     // 积分系数，通常为0，避免过冲
    PID_Position_L.KD = 0.0;     // 微分系数，通常为0，或一个很小的值用于抑制震荡
    PID_Position_L.PIDmax = 50;  // 位置环的输出是速度，这里限制最大期望速度为50 (脉冲数/周期)
    PID_Position_L.PIDmin = -50; // 限制最小(反向)期望速度

    // 初始化右轮位置环PID参数 (通常位置环以P控制为主)
    PID_Position_R.KP = 0.5;     // 比例系数，核心参数，决定响应速度
    PID_Position_R.KI = 0.0;     // 积分系数，通常为0，避免过冲
    PID_Position_R.KD = 0.0;     // 微分系数，通常为0，或一个很小的值用于抑制震荡
    PID_Position_R.PIDmax = 50;  // 位置环的输出是速度，这里限制最大期望速度为50 (脉冲数/周期)
    PID_Position_R.PIDmin = -50; // 限制最小(反向)期望速度

    PID_Angle.KP = 0.218;
    PID_Angle.KI = 0.0;
    PID_Angle.KD = 0.0;
    PID_Angle.Sum_max = 1000;
    PID_Angle.Sum_min = -1000;
    PID_Angle.PIDmax = 600;
    PID_Angle.PIDmin = -600;

    PID_Differential.SetValue = 0;
    PID_Differential.KP = 0.192;
    PID_Differential.KI = 0.0;
    PID_Differential.KD = 100.0;
    PID_Differential.Sum_max = 600;
    PID_Differential.Sum_min = -600;
    PID_Differential.PIDmax = 1000;
    PID_Differential.PIDmin = -1000;

    PID_LineTracker.SetValue = 0;    // 目标是让小车始终在中心，所以误差目标为0
    PID_LineTracker.KP = 0.5;        // 比例系数：决定对当前误差的反应速度
    PID_LineTracker.KI = 0.0;        // 积分系数：用于消除静差（暂时不用）
    PID_LineTracker.KD = 0.8;        // 微分系数：用于抑制震荡，预测误差变化
    PID_LineTracker.Sum_max = 1000;  // 积分上限
    PID_LineTracker.Sum_min = -1000; // 积分下限
    PID_LineTracker.PIDmax = 300;    // PID输出最大值 (限制最大转向速度)
    PID_LineTracker.PIDmin = -300;   // PID输出最小值
}

// float PID_Control_Inc(volatile PID_DataStruct* pid, int flag) //
// {
//     float inc = 0;

//     pid->ek = pid->SetValue - pid->ActualValue;

// 	inc = pid->KP * (pid->ek - pid->ek_1) + pid->KI * pid->ek;

//     pid->ek_2 = pid->ek_1;
//     pid->ek_1 = pid->ek;

//     if(flag == 1)
//     {
//         if(inc > pid->PIDmax)
//             inc = pid->PIDmax;
//         if(inc < pid->PIDmin)
//             inc = pid->PIDmin;
//     }
//     pid->PIDout += inc;
//     return pid->PIDout;
// }
float PID_Control_Inc(volatile PID_DataStruct *pid, int flag)
{
    float inc = 0;

    pid->ek = pid->SetValue - pid->ActualValue;

    inc = pid->KP * (pid->ek - pid->ek_1) + pid->KI * pid->ek;

    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;

    // 原来的代码在这里有一个对增量inc的限幅，但它不是问题的关键
    // if(flag == 1)
    // {
    //     if(inc > pid->PIDmax) inc = pid->PIDmax;
    //     if(inc < pid->PIDmin) inc = pid->PIDmin;
    // }

    // 将计算出的增量累加到总输出中
    pid->PIDout += inc;

    // 【关键修正】: 在这里对累加后的总输出进行限幅 (Anti-Windup)
    // 这可以防止积分持续累积导致饱和
    if (pid->PIDout > pid->PIDmax)
    {
        pid->PIDout = pid->PIDmax;
    }
    if (pid->PIDout < pid->PIDmin)
    {
        pid->PIDout = pid->PIDmin;
    }

    return pid->PIDout;
}

float PID_Control_Pos(volatile PID_DataStruct *pid, int flag)
{
    float Pos = 0;

    pid->ek = pid->SetValue - pid->ActualValue;
    pid->ek_sum += pid->ek;

    if (pid->ek_sum > pid->Sum_max)
        pid->ek_sum = pid->Sum_max;
    if (pid->ek_sum < pid->Sum_min)
        pid->ek_sum = pid->Sum_min;

    Pos = pid->KP * pid->ek + pid->KI * pid->ek_sum + pid->KD * (pid->ek - pid->ek_1);

    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;

    if (flag == 1)
    {
        if (Pos > pid->PIDmax)
            Pos = pid->PIDmax;
        if (Pos < pid->PIDmin)
            Pos = pid->PIDmin;
    }
    pid->PIDout = Pos;

    return pid->PIDout;
}
