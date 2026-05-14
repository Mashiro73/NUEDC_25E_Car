/**
 * @file    pid.h
 * @brief   PID controller data structure and function declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef PID_H_
#define PID_H_

#include "ti_msp_dl_config.h"



typedef struct
{
    float SetValue; 
    float ActualValue; 
    float KP; 
    float KI; 
    float KD; 
    float ek; 
    float ek_1; 
    float ek_2; 
    float ek_sum; 
	
    float Sum_max;
    float Sum_min;
	
    float PIDmax; 
    float PIDmin; 
    float PIDout; 
}PID_DataStruct;

extern volatile PID_DataStruct PID_Motor_L;//Left Speed PID
extern volatile PID_DataStruct PID_Motor_R;//Right Speed PID
extern volatile PID_DataStruct PID_Angle;//Angle PID
extern volatile PID_DataStruct PID_Differential;//Differential PID
extern volatile PID_DataStruct PID_LineTracker; // 循迹PID
extern volatile PID_DataStruct PID_Position_L; // 左轮位置环    
extern volatile PID_DataStruct PID_Position_R; // 右轮位置环
void PID_Init();
float PID_Control_Inc(volatile PID_DataStruct* pid, int flag); //
float PID_Control_Pos(volatile PID_DataStruct* pid, int flag); 

#endif