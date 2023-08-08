// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdint.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel.h> // includes streaming channel and channend
#include <xccompat.h> // important because streaming channel xc/c compatibility

#include "sensor.h"

#define ENABLE_PRINT_SENSOR_CONTROL 0
#define ENCODE(cmd, arg) (((uint32_t)(cmd) << 16) | (uint32_t)(arg))
#define DECODE_CMD(value) ((uint16_t)((value) >> 16))
#define DECODE_ARG(value) ((uint16_t)((value) & 0xFFFF))

#if CONFIG_IMX219_SUPPORT
    #include "imx219.h"
    #define sensor_initialize(cfg)          imx219_initialize(cfg)
    #define sensor_stream_start(cfg)        imx219_stream_start(cfg)
    #define sensor_configure(cfg, rg)       imx219_configure(cfg, rg)
    #define sensor_set_exposure(cfg,ex)     imx219_set_exposure(cfg,ex)
    #define sensor_stream_stop(cfg)         imx219_stream_stop(cfg)

#elif CONFIG_OTHER_SENSOR_SUPPORT           // Demo of how another sensor can be added
    #include "other_sensor.h"
    #define sensor_initialize(cfg)          other_sensor_initialize(cfg)
    #define sensor_stream_start(cfg)        other_sensor_stream_start(cfg)
    #define sensor_configure(cfg, rg)       other_sensor_configure(cfg, rg)
    #define sensor_set_exposure(cfg,ex)     other_sensor_set_exposure(cfg,ex)
    #define sensor_stream_stop(cfg)         other_sensor_stream_stop(cfg)
#endif

// Main control functions
void sensor_i2c_init();
void sensor_control(chanend_t c_control);
