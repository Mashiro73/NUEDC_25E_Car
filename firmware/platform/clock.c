/**
 * @file    clock.c
 * @brief   Delay functions and timer utilities
 * @author  Dinglei Xu
 * @date    2025
 */

#include "clock.h"

volatile uint16_t g_timer_high_bits = 0;

void delay_ms(unsigned int ms)
{
    extern volatile unsigned int g_delay_times;
    g_delay_times = ms;
    while (g_delay_times != 0)
        ;
}

void TIMER_1_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_1_INST))
    {
    case DL_TIMERG_IIDX_LOAD:
        g_timer_high_bits++;
        break;
    default:
        break;
    }
}
