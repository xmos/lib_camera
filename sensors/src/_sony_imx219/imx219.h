#pragma once

#include <xs1.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stddef.h> // size_t

#include "i2c.h"
#include "sensor_settings.h"
#include "imx219_reg.h"

// Definitions
#define CONTINUOUS_MODE(val, add) ((val & 0xFF00) || (add & 0x8000))
#ifndef GAIN_DB
#define GAIN_DB 40
#endif

#define PRINT_I2C_REG 0 


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