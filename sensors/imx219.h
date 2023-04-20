#pragma once

#include "sensor.h"

#include <stdint.h>
#include "i2c.h"

// I2C adress
#define IMX219_I2C_ADDR 0x10

// TODO maybe out of here
typedef struct
{
    uint16_t addr;
    uint16_t val;
} imx219_settings_t;

#ifdef __XC__

// configure registers
#if ((CONFIG_MODE == 0) || (CONFIG_MODE == 1))
    #define CONFIG_REG      mode_640_480_regs
#elif (CONFIG_MODE == 2)
    #define CONFIG_REG      mode_1640_1232_regs
#else
    #error "Invalid configuration mode"
#endif

// Configure formats
#if EXPECTED_FORMAT == MIPI_DT_RAW10
    #define DATA_FORMAT_REGS raw10_framefmt_regs

#elif EXPECTED_FORMAT == MIPI_DT_RAW8
    #define DATA_FORMAT_REGS  raw8_framefmt_regs
    
#endif

// configure FPS
#if   defined(FPS_13) 
    #define PLL_VT_MPY  0x0030   
#elif defined(FPS_24)
    #define PLL_VT_MPY  0x0047
#elif defined(FPS_30)
    #define PLL_VT_MPY  0x0058 
#elif defined(FPS_38)
    #define PLL_VT_MPY  0x0070 
#elif defined(FPS_53)
    #define PLL_VT_MPY  0x009C 
#elif defined(FPS_76)
    #define PLL_VT_MPY  0x00E0 
#else
    #warning fps not defined, selecting default value
    #define PLL_VT_MPY  0x0027   
#endif


// functions
int imx219_init(client interface i2c_master_if i2c);
int imx219_stream_start(client interface i2c_master_if i2c);
int imx219_configure_mode(client interface i2c_master_if i2c);
int imx219_stream_stop(client interface i2c_master_if i2c);
int imx219_set_gain_dB(client interface i2c_master_if i2c, uint32_t dBGain);
int imx219_set_binning(client interface i2c_master_if i2c, uint32_t H_binning, uint32_t V_binning);
int imx219_read(client interface i2c_master_if i2c, uint16_t addr);

#endif