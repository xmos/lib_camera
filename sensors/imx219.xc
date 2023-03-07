#include <stdint.h>
#include <stdio.h>
#include <xs1.h>
#include "i2c.h"
#include "imx219.h"

#define SWIDTH    3296
#define SHEIGHT   2480
//#define SWIDTH    0x0D78
//#define SHEIGHT   0x0A83
#define IMX219_I2C_ADDR 0x10

/* IMX219 supported geometry */
#define IMX219_WIDTH			3280
#define IMX219_HEIGHT			2464

typedef struct {
	uint16_t addr;
	uint16_t val;
} imx219_settings_t ;

#define SLEEP   0xFFFF
#define TRSTUS  200
static imx219_settings_t chip_set_up[] = {
	{ 0x0103,   0x01 }, /* software_reset       1, reset the chip */
	{ SLEEP , TRSTUS }, /* software_reset       1, reset the chip */
                        /* First, set up PLLs                     */
	{ 0x812A, 0x1800 }, /* EXCK_FREQ        24.00, for 24 Mhz */
	{ 0x0304,   0x02 }, /* PREPLLCK_VT_DIV      2, for pre divide by 2 */
	{ 0x0305,   0x02 }, /* PREPLLCK_OP_DIV      2, for pre divide by 2 */
	{ 0x8306, 0x0027 }, /* PLL_VT_MPY        0x27, for multiply by 39, pixclk=187.2 MHz */
	{ 0x830C, 0x0040 }, /* PLL_OP_MPY        0x40, for multiply by 64, MIPI clk=768 MHz */
	{ 0x0301,   0x05 }, /* VTPXCK_DIV           5, ? */
	{ 0x0303,   0x01 }, /* VTSYCK_DIV           1, ? */
	{ 0x0309,   0x08 }, /* OPPXCK_DIV           8, has to match RAW8 */
	{ 0x030B,   0x01 }, /* OPSYCK_DIV           1, has to be 1? */
                        /* Set basic parameters */
	{ 0x8160, 0x0A83 }, /* FRM_LENGTH_A      0A83, for 2691 lines per frame */
	{ 0x8162, 0x0D78 }, /* LINE_LENGTH_A     0D78, for 3448 pixels per line */
	{ 0x0170,   0x01 }, /* X_ODD_INC_A          1, ?? */
	{ 0x0171,   0x01 }, /* Y_ODD_INC_A          1, ?? */
	{ 0x0172,   0x00 }, /* FLIP                 0, no flipping */
	{ 0x0174,   0x00 }, /* BINNING_MODE_H_A     2, for no binning */
	{ 0x0175,   0x00 }, /* BINNING_MODE_V_A     2, for no binning */
                        /* Set MIPI interface up */
	{ 0x0114,   0x01 }, /* CSI_LANE_MODE        1, for 2 lanes  */
	{ 0x0128,   0x00 }, /* DPHY_CNTRL           0, for auto timing */
	{ 0x818C, 0x0A08 }, /* CSI_DATA_FORMAT_A 0A08, for RAW8 format */

    { 0x8164,      0 }, /* X_ADD_STA            0, No crop on left */
    { 0x8166, IMX219_WIDTH-1}, /* X_ADD_END       SWIDTH, Full width */
    { 0x8168,      0 }, /* Y_ADD_STA            0, No crop on top */
    { 0x8166, IMX219_HEIGHT-1}, /* Y_ADD_END      SHEIGHT, Full height */
    { 0x816C, IMX219_WIDTH }, /* x_output_size   SWIDTH, Full width */
    { 0x816E, IMX219_HEIGHT}, /* y_output_size  SHEIGHT, Full height */

	{ 0x8600, 0x0004 }, /* test_pattern_mode    3, greyscale (4: PN9) */
    { 0x8620,      0 }, /* Whole test pattern */
    { 0x8622,      0 }, 
    { 0x8624, IMX219_WIDTH },   // Definitely width
    { 0x8626, IMX219_HEIGHT},   // Definitely height
};

static imx219_settings_t start[] = {
	{ 0x0100, 0x01 }, /* mode select streaming on */
};

static imx219_settings_t stop[] = {
	{ 0x0100, 0x00 }, /* mode select streaming off */
};


static int i2c_write(client interface i2c_master_if i2c, int reg, int value) {
    i2c_regop_res_t result;  
    result = i2c.write_reg8_addr16(IMX219_I2C_ADDR, reg, value);
    if (result != I2C_REGOP_SUCCESS) {
        //printf("Failed on address %02x value %02x\n", reg, value);
        //TODO FIXME
    }
    return result != I2C_REGOP_SUCCESS ? -1 : 0;
}

static int i2c_write_table(client interface i2c_master_if i2c,
                    imx219_settings_t table[],
                    int N) {
	int ret;
    for(int i = 0; i < N; i++) {
        uint32_t address = table[i].addr;
        uint32_t value   = table[i].val;
        if (address == SLEEP) {
            timer tmr;
            int t;
            tmr :> t;
            tmr when timerafter(t + TRSTUS * 100) :> void;
        }
        if (address & 0x8000) {
            address &= 0x7fff;
            ret  = i2c_write(i2c, address,   value >> 8);
            ret |= i2c_write(i2c, address+1, value & 0xff);
        } else {
            ret = i2c_write(i2c, address, value);
        }
		if (ret < 0) {
			return ret;
        }
	}
	return 0;
}

int imx219_stream_stop(client interface i2c_master_if i2c) {
    return i2c_write_table(i2c, stop, sizeof(stop) / sizeof(stop[0]));
}

int imx219_stream_start(client interface i2c_master_if i2c) {
	int ret;
	ret = i2c_write_table(i2c, chip_set_up, sizeof(chip_set_up) / sizeof(chip_set_up[0]));
	if (ret) {
		return ret;
    }
    ret = imx219_set_gain_dB(i2c, GAIN_DEFAULT_DB);
	if (ret)
		return ret;
	return i2c_write_table(i2c, start, sizeof(start) / sizeof(start[0]));
}

#define INTEGRATION_TIMES 41
#define ANALOGUE_GAINS    20
#define DIGITAL_GAINS     25

static uint16_t gain_integration_times[INTEGRATION_TIMES] = {
    0x00a, 0x00b, 0x00c, 0x00e, 0x010, 0x012, 0x014, 0x016, 0x019,
    0x01c, 0x020, 0x024, 0x028, 0x02d, 0x033, 0x039, 0x040, 0x048,
    0x051, 0x05b, 0x066, 0x072, 0x080, 0x090, 0x0a2, 0x0b6, 0x0cc,
    0x0e5, 0x101, 0x120, 0x143, 0x16b, 0x197, 0x1c9, 0x201, 0x23f,
    0x286, 0x2d4, 0x32d, 0x390, 0x400,
};

static uint8_t gain_analogue_gains[ANALOGUE_GAINS+1] = {
    0, 28, 53, 75, 95, 112, 128, 142, 155, 166, 175, 184,
    192, 199, 205, 211, 215, 220, 224, 227, 231,
};

static uint16_t gain_digital_gains[DIGITAL_GAINS+1] = {
    0x0100, 0x011f, 0x0142, 0x0169, 0x0195, 0x01c7, 0x01fe, 0x023d,
    0x0283, 0x02d1, 0x0329, 0x038c, 0x03fb, 0x0477, 0x0503, 0x059f,
    0x064f, 0x0714, 0x07f1, 0x08e9, 0x0a00, 0x0b38, 0x0c96, 0x0e20,
    0x0fd9,
};

int imx219_set_gain_dB(client interface i2c_master_if i2c,
                       uint32_t dBGain) {
    uint32_t time, again, dgain;
    if (dBGain > GAIN_MAX_DB) {
        dBGain = GAIN_MAX_DB;
    }
    if (dBGain < INTEGRATION_TIMES) {
        time = gain_integration_times[dBGain];
        again = gain_analogue_gains[0];
        dgain = gain_digital_gains[0];
    } else {
        time = gain_integration_times[INTEGRATION_TIMES-1];
        if (dBGain < INTEGRATION_TIMES + ANALOGUE_GAINS) {
            again = gain_analogue_gains[dBGain - INTEGRATION_TIMES + 1];
            dgain = gain_digital_gains[0];
        } else {
            again = gain_analogue_gains[ANALOGUE_GAINS];
            dgain = gain_digital_gains[dBGain - INTEGRATION_TIMES - ANALOGUE_GAINS + 1];
        }
    }
	int ret = i2c_write(i2c, 0x0157, again);
	ret |= i2c_write(i2c, 0x0158, dgain >> 8);
	ret |= i2c_write(i2c, 0x0159, dgain & 0xff);
	ret |= i2c_write(i2c, 0x015A, time >> 8);
	ret |= i2c_write(i2c, 0x015B, time & 0xff);

    return ret;
}
