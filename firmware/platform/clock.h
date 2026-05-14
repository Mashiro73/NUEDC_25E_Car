/**
 * @file    clock.h
 * @brief   Delay and timer function declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef __CLOCK_H
#define __CLOCK_H

#include <stdint.h>

void delay_ms(unsigned int ms);

extern volatile uint16_t g_timer_high_bits;

#endif
