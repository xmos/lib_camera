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
#if (CONFIG_MODE == 0)
    #define CONFIG_REG      mode_640_480_regs
#elif (CONFIG_MODE == 2)
    #define CONFIG_REG      mode_1640_1232_regs
#else
    #error "Invalid configuration mode"
#endif

// functions
int imx219_init(client interface i2c_master_if i2c);
int imx219_stream_start(client interface i2c_master_if i2c);
int imx219_configure_mode_0(client interface i2c_master_if i2c);
int imx219_configure_mode(client interface i2c_master_if i2c);
int imx219_stream_stop(client interface i2c_master_if i2c);
int imx219_set_gain_dB(client interface i2c_master_if i2c, uint32_t dBGain);
int imx219_set_binning(client interface i2c_master_if i2c, uint32_t H_binning, uint32_t V_binning);
#endif