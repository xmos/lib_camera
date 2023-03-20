#include <stdint.h>
#include <stdio.h>
#include <xs1.h>
#include "i2c.h"
#include "imx219.h"
#include "imx219_reg.h"


static int i2c_write(client interface i2c_master_if i2c, int reg, int value)
{
    i2c_regop_res_t result;
    // Write an 8-bit register on a slave device from a 16-bit register address.
    result = i2c.write_reg8_addr16(IMX219_I2C_ADDR, reg, value);
    if (result != I2C_REGOP_SUCCESS)
    {
        // printf("Failed on address %02x value %02x\n", reg, value);
        // TODO FIXME
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
            ret  = i2c_write(i2c, address,   value >> 8); // B1 B2 B3 B4 -> B1 B2
            ret |= i2c_write(i2c, address+1, value & 0xff); // B1 B2 B3 B4 -> B3 B4
        } else {
            ret = i2c_write(i2c, address, value);
        }
		if (ret < 0) {
			return ret;
        }
	}
	return 0;
}



static int i2c_write_table_val(client interface i2c_master_if i2c,
                    imx219_settings_t table[],
                    int N) {
	int ret;
    char mode = 's';

    for(int i = 0; i < N; i++) {
        uint32_t address = table[i].addr;
        uint32_t value   = table[i].val;
        if (address == SLEEP) {
            timer tmr;
            int t;
            tmr :> t;
            tmr when timerafter(t + TRSTUS * 100) :> void;
        }

        if ((value & 0xFF00) || (address & 0x8000)){
            if (address & 0x8000) {
                address &= 0x7fff;
            }
            mode = 'c';
            printf("mode=%c , adress = 0x%04x, value = 0x%02x\n", mode, address, value >> 8);
            printf("mode=%c , adress+ = 0x%04x, value = 0x%02x\n", mode, address+1, value & 0xff);   
            // continous writte
            ret  = i2c_write(i2c, address,   value >> 8); // B1 B2 B3 B4 -> B1 B2
            ret |= i2c_write(i2c, address+1, value & 0xff); // B1 B2 B3 B4 -> B3 B4 
        }
        else
        {
            // single writte
            mode = 's';
            ret = i2c_write(i2c, address, value);
            printf("mode=%c , adress = 0x%04x, value = 0x%02x\n", mode, address, value);
        }    
		if (ret < 0) {
			return ret;
        }
	}
	return 0;
}



void read(client interface i2c_master_if i2c){
i2c_regop_res_t res;
// res = i2c.write_reg(GC2145_I2C_ADDR, 0xFE, (page & 0x03));
}


/// -------------------------------------------------------------------------------

int imx219_init(client interface i2c_master_if i2c)
{
    int ret = 0;
    // Send all registers that are common to all modes
    ret = i2c_write_table_val(i2c, imx219_common_regs, sizeof(imx219_common_regs) / sizeof(imx219_common_regs[0]));
    // Configure two or four Lane mode
    ret = i2c_write_table_val(i2c, imx219_lanes_regs, sizeof(imx219_lanes_regs) / sizeof(imx219_lanes_regs[0]));
    return ret;
}

int imx219_configure_mode(client interface i2c_master_if i2c)
{
    int ret = 0;
    // Apply default values of current mode
    ret = i2c_write_table_val(i2c, CONFIG_REG, sizeof(CONFIG_REG) / sizeof(CONFIG_REG[0]));
    // set frame format register
    ret = i2c_write_table_val(i2c, raw10_framefmt_regs, sizeof(raw10_framefmt_regs) / sizeof(raw10_framefmt_regs[0]));
    // set binning
    ret = i2c_write_table_val(i2c, binning_regs, sizeof(binning_regs) / sizeof(binning_regs[0]));
    return ret;
}


int imx219_stream_start(client interface i2c_master_if i2c){
    int ret = 0;
    /* set stream on register */
    ret = i2c_write_table_val(i2c, start_regs, sizeof(start_regs) / sizeof(start_regs[0]));
    return ret;
}

int imx219_stream_stop(client interface i2c_master_if i2c){
    return i2c_write_table(i2c, stop, sizeof(stop) / sizeof(stop[0]));
}

int imx219_set_gain_dB(client interface i2c_master_if i2c,
                       uint32_t dBGain)
{
    uint32_t time, again, dgain;
    if (dBGain > GAIN_MAX_DB)
    {
        dBGain = GAIN_MAX_DB;
    }
    if (dBGain < INTEGRATION_TIMES)
    {
        time = gain_integration_times[dBGain];
        again = gain_analogue_gains[0];
        dgain = gain_digital_gains[0];
    }
    else
    {
        time = gain_integration_times[INTEGRATION_TIMES - 1];
        if (dBGain < INTEGRATION_TIMES + ANALOGUE_GAINS)
        {
            again = gain_analogue_gains[dBGain - INTEGRATION_TIMES + 1];
            dgain = gain_digital_gains[0];
        }
        else
        {
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


// https://github.com/torvalds/linux/blob/63355b9884b3d1677de6bd1517cd2b8a9bf53978/drivers/media/i2c/imx219.c#L993
