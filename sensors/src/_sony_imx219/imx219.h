#pragma once

#include <xs1.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h> // size_t

#include <xcore/assert.h>

#include "i2c.h"
#include "sensor_settings.h"
#include "imx219_reg.h"

// Definitions
#define CONTINUOUS_MODE(val, add) ((val & 0xFF00) || (add & 0x8000))
#ifndef GAIN_DB
#define GAIN_DB 40
#endif

#define PRINT_I2C_REG 0 

#define DEFAULT_REG_CONF (regs_config_t){ \
    .regs_frame_size = mode_640_480_regs, \
    .regs_frame_size_size = sizeof(mode_640_480_regs) / sizeof(i2c_settings_t), \
    .regs_pixel_format = raw8_framefmt_regs, \
    .regs_pixel_format_size = sizeof(raw8_framefmt_regs) / sizeof(i2c_settings_t) \
}

#define ALT_REG_CONF (regs_config_t){ \
    .regs_frame_size = mode_1280_960_regs, \
    .regs_frame_size_size = sizeof(mode_1280_960_regs) / sizeof(i2c_settings_t), \
    .regs_pixel_format = raw8_framefmt_regs, \
    .regs_pixel_format_size = sizeof(raw8_framefmt_regs) / sizeof(i2c_settings_t) \
}

// ------------------- IMX219 -------------------
// Init function
void imx219_i2c_init(i2c_config_t cfg);
// Read functions
uint16_t imx219_read(i2c_config_t cfg, uint16_t reg);
// Write functions
int imx219_write(i2c_config_t cfg, i2c_settings_t settings);
int imx219_write_table(i2c_config_t cfg, i2c_settings_t table[], size_t table_size);
// High level functions
int imx219_initialize(i2c_config_t cfg);
int imx219_stream_start(i2c_config_t cfg);
int imx219_configure(i2c_config_t cfg, regs_config_t reg_cfg);
int imx219_set_exposure(i2c_config_t cfg, uint32_t dBGain);
int imx219_stream_stop(i2c_config_t cfg);
