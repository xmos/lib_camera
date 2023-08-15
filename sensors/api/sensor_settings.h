// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

// This file is used from each sensor
// It can't be included in sensor.h becuse it depends on i2C that is not xc compatible. 

#include <stdint.h>

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

#include "i2c.h"

#if defined(__XC__) || defined(__cplusplus)
}
#endif

typedef struct
{
  uint16_t reg_addr;
  uint16_t reg_val;
} i2c_line_t;

typedef struct
{
  i2c_line_t * table;
  size_t num_lines;
} i2c_table_t;

#define GET_NUM_LINES(regs_arr) (sizeof(regs_arr) / sizeof(i2c_line_t))
#define GET_TABLE(regs_arr) (i2c_table_t){regs_arr, GET_NUM_LINES(regs_arr)}

/* Test 1b port SCL 1b port SDA */
typedef struct 
{
  uint8_t  device_addr;
  uint16_t speed;
  port_t p_scl;
  port_t p_sda;
  i2c_master_t* i2c_ctx_ptr;
} i2c_config_t;
