/**
 * @file    line_sensor.c
 * @brief   8-channel line-tracking sensor array driver
 * @author  Dinglei Xu
 * @date    2025
 */

#include "line_sensor.h"

void line_sensor_init(LineSensor_t *self, GPIO_Regs *ports[], uint32_t pins[])
{
    for (int i = 0; i < LINE_SENSOR_COUNT; i++)
    {
        self->sensorPorts[i] = ports[i];
        self->sensorPins[i] = pins[i];
    }
}

uint8_t line_sensor_read(LineSensor_t *self)
{
    uint8_t sensor_values = 0;

    for (int i = 0; i < LINE_SENSOR_COUNT; i++)
    {
        if (DL_GPIO_readPins(self->sensorPorts[i], self->sensorPins[i]))
        {
            sensor_values |= (1 << i);
        }
    }

    return sensor_values;
}
