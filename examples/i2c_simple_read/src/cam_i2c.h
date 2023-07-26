#pragma once

#include <xs1.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stddef.h> // size_t

#include "i2c.h"
#include "imx219_reg.h"
#include "common.h"

// Definitions
#define CONTINUOUS_MODE(val, add) ((val & 0xFF00) || (add & 0x8000))
#ifndef GAIN_DB
#define GAIN_DB 40
#endif


#ifdef __XC__
extern "C" {
#endif

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
    i2c_settings_t* regs_pixel_format;
} regs_config_t;

// ------------------- IMX219 -------------------


// Read functions
void i2c_init(i2c_config_t cfg);
uint16_t imx219_read(i2c_config_t cfg, uint16_t reg);
// Write functions
int imx219_write(i2c_config_t cfg, i2c_settings_t settings);
int imx219_write_table(i2c_config_t cfg, i2c_settings_t table[], size_t table_size);

// High level functions
int imx219_initialize(i2c_config_t cfg);
int imx219_configure(i2c_config_t cfg, regs_config_t reg_cfg);
int imx219_set_exposure(i2c_config_t cfg, uint32_t dBGain);

int imx219_stream_start(i2c_config_t cfg);
int imx219_stream_stop(i2c_config_t cfg);


// Example 
void camera_i2c_master();

#ifdef __XC__
}
#endif
