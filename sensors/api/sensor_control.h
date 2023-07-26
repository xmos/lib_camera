#pragma once 

#include <stdio.h>
#include <stdint.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel.h> // includes streaming channel and channend

#include "imx219.h"

#define SETSR(c) asm volatile("setsr %0" : : "n"(c));

#define N_COMMANDS 5
typedef enum {
    SENSOR_INIT = 0,
    SENSOR_CONFIG,
    SENSOR_STREAM_START,
    SENSOR_STREAM_STOP,
    SENSOR_SET_EXPOSURE
} camera_control_t;

// In order to interface the handler and api
/*
void sensor_control(
    i2c_config_t sony_i2c_cfg,
    streaming_channel_t sensor_schan[N_COMMANDS]
);
*/

void sensor_i2c_start(chanend_t schan[]);

void simple_sensor_control(chanend_t schan[]);

void sensor_control(chanend_t schan[]);
