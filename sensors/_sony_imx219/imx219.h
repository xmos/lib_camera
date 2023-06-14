#pragma once

#include "sensor.h"

#include <stdint.h>
#include "i2c.h"
#include "xccompat.h"

// I2C adress
#define IMX219_I2C_ADDR 0x10

// Imx settings
typedef struct
{
    uint16_t addr;
    uint16_t val;
} imx219_settings_t;

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
int imx219_init(CLIENT_INTERFACE(i2c_master_if, i2c));
int imx219_stream_start(CLIENT_INTERFACE(i2c_master_if, i2c));
int imx219_configure_mode(CLIENT_INTERFACE(i2c_master_if, i2c));
int imx219_stream_stop(CLIENT_INTERFACE(i2c_master_if, i2c));
int imx219_set_gain_dB(CLIENT_INTERFACE(i2c_master_if, i2c), uint32_t dBGain);
int imx219_set_binning(CLIENT_INTERFACE(i2c_master_if, i2c), uint32_t H_binning, uint32_t V_binning);
int imx219_read(CLIENT_INTERFACE(i2c_master_if, i2c), uint16_t addr);
void imx219_read_gains(CLIENT_INTERFACE(i2c_master_if, i2c), uint16_t values[5]);


#define camera_init(X)                imx219_init(X)
#define camera_start(X)               imx219_stream_start(X)
#define camera_configure(X)           imx219_configure_mode(X)
#define camera_set_exposure(iic,ex)   imx219_set_gain_dB(iic,ex)



