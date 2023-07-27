#pragma once 

#include <stdio.h>
#include <stdint.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel.h> // includes streaming channel and channend

#include "sensor.h"

#if CONFIG_IMX219_SUPPORT
    #include "imx219.h"
    #define sensor_initialize(cfg)          imx219_initialize(cfg)
    #define sensor_stream_start(cfg)        imx219_stream_start(cfg)
    #define sensor_configure(cfg, rg)       imx219_configure(cfg, rg)
    #define sensor_set_exposure(cfg,ex)     imx219_set_exposure(cfg,ex)
    #define sensor_stream_stop(cfg)         imx219_stream_stop(cfg)

#elif CONFIG_OTHER_SENSOR_SUPPORT           // this is a demo of how we would add another sensor
    #include "other_sensor.h"
    #define sensor_initialize(cfg)          other_sensor_initialize(cfg)
    #define sensor_stream_start(cfg)        other_sensor_stream_start(cfg)
    #define sensor_configure(cfg, rg)       other_sensor_configure(cfg, rg)
    #define sensor_set_exposure(cfg,ex)     other_sensor_set_exposure(cfg,ex)
    #define sensor_stream_stop(cfg)         other_sensor_stream_stop(cfg)
#endif

#define N_COMMANDS 5
typedef enum {
    SENSOR_INIT = 0,
    SENSOR_CONFIG,
    SENSOR_STREAM_START,
    SENSOR_STREAM_STOP,
    SENSOR_SET_EXPOSURE
} camera_control_t;

void sensor_i2c_start();
void sensor_control(chanend_t schan[]);
