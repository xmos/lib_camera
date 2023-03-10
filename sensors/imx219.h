#include <stdint.h>
#include "i2c.h"

#define SWIDTH 3296
#define SHEIGHT 2480
// #define SWIDTH    0x0D78
// #define SHEIGHT   0x0A83
#define IMX219_I2C_ADDR 0x10

/* IMX219 supported geometry */
#define IMX219_WIDTH 3280
#define IMX219_HEIGHT 2464

// Imx219 specific adress and data structure
typedef struct
{
    uint16_t addr;
    uint16_t val;
} imx219_settings_t;

// functions
extern int imx219_stream_start(client interface i2c_master_if i2c);
extern int imx219_stream_stop(client interface i2c_master_if i2c);
extern int imx219_set_gain_dB(client interface i2c_master_if i2c,
                                uint32_t dBGain);
extern int imx219_set_binning(client interface i2c_master_if i2c,
                                uint32_t H_binning,
                                uint32_t V_binning);
