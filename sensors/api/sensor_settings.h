// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

// This file is used from each sensor
// It can't be included in sensor.h becuse it depends on i2C that is not xc compatible. 

#include <stdlib.h>
#include <stdint.h>

#include "i2c.h"

typedef struct
{
    uint16_t reg_addr;
    uint16_t reg_val;
} i2c_settings_t;

/* Test 1b port SCL 1b port SDA */
typedef struct 
{
    uint8_t  device_addr;
    uint16_t speed;
    port_t p_scl;
    port_t p_sda;
    i2c_master_t* i2c_ctx_ptr;
} i2c_config_t;


typedef struct
{
    i2c_settings_t* regs_frame_size;
    size_t regs_frame_size_size;
    i2c_settings_t* regs_pixel_format;
    size_t regs_pixel_format_size;
} regs_config_t;
