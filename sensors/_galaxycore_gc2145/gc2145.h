#include <stdint.h>
#include "i2c.h"

#define GAIN_MIN_DB       0
#define GAIN_MAX_DB      84
#define GAIN_DEFAULT_DB  50

#define GC2145_I2C_ADDR (0x3C)

#define GC2145MIPI_2Lane
#define GC2145MIPI_TEST_PATTERN_CHECKSUM 0x54ddf19b 

/* GC2145 supported geometry */
#define GC2145_WIDTH			(1600)
#define GC2145_HEIGHT			(1200)

int gc2145_stream_start(client interface i2c_master_if i2c);
int gc2145_stream_stop(client interface i2c_master_if i2c);
int gc2145_init(client interface i2c_master_if i2c);
int gc2145_set_gain_dB(client interface i2c_master_if i2c,
                              uint32_t dBGain);


typedef struct {
	uint16_t addr;
	uint16_t val;
} gc_settings_t;

extern const
gc_settings_t chip_set_up[];

extern const
size_t chip_set_up_length;

#define SLEEP   0xFFFF
#define TRSTUS  200