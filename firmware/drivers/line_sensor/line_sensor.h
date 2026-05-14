/**
 * @file    line_sensor.h
 * @brief   Line-tracking sensor array type and function declarations
 * @author  Dinglei Xu
 * @date    2025
 */

#ifndef __LINE_SENSOR_H__
#define __LINE_SENSOR_H__

#include <stdint.h>
#include "ti_msp_dl_config.h"

#define LINE_SENSOR_COUNT (8)

typedef struct
{
    GPIO_Regs *sensorPorts[LINE_SENSOR_COUNT];
    uint32_t sensorPins[LINE_SENSOR_COUNT];
} LineSensor_t;

void line_sensor_init(LineSensor_t *self, GPIO_Regs *ports[], uint32_t pins[]);

uint8_t line_sensor_read(LineSensor_t *self);

#endif 
