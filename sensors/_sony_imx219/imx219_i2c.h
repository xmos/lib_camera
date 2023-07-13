#pragma once

#include <stdint.h>

#include "xccompat.h"

#ifdef __XC__
#include "i2c.h"
#endif

#include "imx219.h"

 
uint16_t imx219_i2c_read_reg16(
    CLIENT_INTERFACE(i2c_master_if, i2c_client),
    uint16_t reg,
    int* result);

int imx219_i2c_write(
    CLIENT_INTERFACE(i2c_master_if, i2c_client), 
    int reg, 
    int value);
 
int imx219_i2c_write_table(
    CLIENT_INTERFACE(i2c_master_if, i2c_client),
    const imx219_settings_t table[]);
 
int imx219_i2c_write_table_val(
    CLIENT_INTERFACE(i2c_master_if, i2c_client),
    const imx219_settings_t table[]);

