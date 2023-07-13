
#include <assert.h>

#include "imx219.hpp"


#define GAIN_DB 40

using namespace imx219;

imx219_settings_t imx219_common_regs[] = {
    {0x0103, 0x01},   /* software_reset       1, reset the chip */
    {SLEEP, TRSTUS},  /* software_reset       1, reset the chip */

	{0x0100, 0x00},	/* Mode Select */

	/* To Access Addresses 3000-5fff, send the following commands */
	{0x30eb, 0x0c},
	{0x30eb, 0x05},
	{0x300a, 0xff},
	{0x300b, 0xff},
	{0x30eb, 0x05},
	{0x30eb, 0x09},

	/* PLL Clock Table */
	{ 0x812A, 0x1800 }, /* EXCK_FREQ        24.00, for 24 Mhz */
	{ 0x0304,   0x02 }, /* PREPLLCK_VT_DIV      2, for pre divide by 2 */
	{ 0x0305,   0x02 }, /* PREPLLCK_OP_DIV      2, for pre divide by 2 */
	{ 0x8306,  PLL_VT_MPY}, /* PLL_VT_MPY        0x27, for multiply by 39, pixclk=187.2 MHz */
	{ 0x830C,  PLL_OP_MPY}, /* PLL_OP_MPY        0x40, for multiply by 64, MIPI clk=768 MHz */
	{ 0x0301,   0x0A }, /* VTPXCK_DIV           5, ? */
	{ 0x0303,   0x01 }, /* VTSYCK_DIV           1, ? */
	{ 0x0309,   0x0A }, /* OPPXCK_DIV           8, has to match RAW8 if you have raw8*/
	{ 0x030B,   0x01 }, /* OPSYCK_DIV           1, has to be 1? */
    // pck clock
    {0x1148,    0x00},    
    {0x1149,    0xF0},
	/* Undocumented registers */
	{0x455e, 0x00},
	{0x471e, 0x4b},
	{0x4767, 0x0f},
	{0x4750, 0x14},
	{0x4540, 0x00},
	{0x47b4, 0x14},
	{0x4713, 0x30},
	{0x478b, 0x10},
	{0x478f, 0x10},
	{0x4793, 0x10},
	{0x4797, 0x0e},
	{0x479b, 0x0e},

	/* Frame Bank Register Group "A" */
	{0x0162, 0x0d},	/* Line_Length_A */
	{0x0163, 0x78},
	{0x0170, 0x01}, /* X_ODD_INC_A */
	{0x0171, 0x01}, /* Y_ODD_INC_A */

	/* Output setup registers */
	{0x0114, 0x01},	/* CSI 2-Lane Mode */
	{0x0128, 0x00},	/* DPHY Auto Mode */
	{0x012a, 0x18},	/* EXCK_Freq */
	{0x012b, 0x00},
  IMX219_END_SETTINGS
};


imx219_settings_t imx219_lanes_regs[] = {
  {CSI_LANE_MODE_REG, CSI_LANE_MODE_2_LANES},
  IMX219_END_SETTINGS
};





int SensorBase::initialize()
{
  int ret = 0;
  // Send all registers that are common to all modes
  ret = imx219_i2c_write_table_val(this->i2c_client, imx219_common_regs);
  // Configure two or four Lane mode
  ret = imx219_i2c_write_table_val(this->i2c_client, imx219_lanes_regs);
  // set gain
  ret = this->set_exposure(GAIN_DB);
  return ret;
}




imx219_settings_t start_regs[] = {
  {0x0100, 0x01}, /* mode select streaming on */
  IMX219_END_SETTINGS
};


int SensorBase::stream_start() 
{
  int ret = 0;
  /* set stream on register */
  ret = imx219_i2c_write_table_val(this->i2c_client, start_regs);
  return ret;
}



imx219_settings_t mode_640_480_regs[] = {
  {0x0164, 0x03},
  {0x0165, 0xe8},
  {0x0166, 0x08},
  {0x0167, 0xe7},
  {0x0168, 0x02},
  {0x0169, 0xf0},
  {0x016a, 0x06},
  {0x016b, 0xaf},
  {0x016c, 0x02},
  {0x016d, 0x80},
  {0x016e, 0x01},
  {0x016f, 0xe0},
  {0x0624, 0x06},
  {0x0625, 0x68},
  {0x0626, 0x04},
  {0x0627, 0xd0},
  IMX219_END_SETTINGS
};



imx219_settings_t mode_1640_1232_regs[] = {
  {0x0164, 0x00},
  {0x0165, 0x00},
  {0x0166, 0x0c},
  {0x0167, 0xcf},
  {0x0168, 0x00},
  {0x0169, 0x00},
  {0x016a, 0x09},
  {0x016b, 0x9f},
  {0x016c, 0x06},
  {0x016d, 0x68},
  {0x016e, 0x04},
  {0x016f, 0xd0},
  {0x0624, 0x06},
  {0x0625, 0x68},
  {0x0626, 0x04},
  {0x0627, 0xd0},
  IMX219_END_SETTINGS
};



imx219_settings_t mode_1920_1080_regs[] = {
  {0x0164, 0x02},
  {0x0165, 0xa8},
  {0x0166, 0x0a},
  {0x0167, 0x27},
  {0x0168, 0x02},
  {0x0169, 0xb4},
  {0x016a, 0x06},
  {0x016b, 0xeb},
  {0x016c, 0x07},
  {0x016d, 0x80},
  {0x016e, 0x04},
  {0x016f, 0x38},
  {0x0624, 0x07},
  {0x0625, 0x80},
  {0x0626, 0x04},
  {0x0627, 0x38},
  IMX219_END_SETTINGS
};


imx219_settings_t mode_3280x2464_regs[] = {
  {0x0164, 0x00},
  {0x0165, 0x00},
  {0x0166, 0x0c},
  {0x0167, 0xcf},
  {0x0168, 0x00},
  {0x0169, 0x00},
  {0x016a, 0x09},
  {0x016b, 0x9f},
  {0x016c, 0x0c},
  {0x016d, 0xd0},
  {0x016e, 0x09},
  {0x016f, 0xa0},
  {0x0624, 0x0c},
  {0x0625, 0xd0},
  {0x0626, 0x09},
  {0x0627, 0xa0},
  IMX219_END_SETTINGS
};


imx219_settings_t raw10_framefmt_regs[] = {
  {0x018c, 0x0a},
  {0x018d, 0x0a},
  {0x0309, 0x0a},
  IMX219_END_SETTINGS
};


imx219_settings_t raw8_framefmt_regs[] = {
  {0x018c, 0x08},
  {0x018d, 0x08},
  {0x0309, 0x08},
  IMX219_END_SETTINGS
};


imx219_settings_t binning_regs[] = {
	{BINNING_MODE_REG, BINNING_MODE},
  IMX219_END_SETTINGS
};



int SensorBase::configure(
  imx219_settings_t* resolution,
  imx219_settings_t* pixel_format) 
{
    int ret = 0;
    // Apply default values of current mode
    ret = imx219_i2c_write_table_val(this->i2c_client, resolution);
    // set frame format register
    ret = imx219_i2c_write_table_val(this->i2c_client, pixel_format);
    // set binning
    ret = imx219_i2c_write_table_val(this->i2c_client, binning_regs);
    return ret;
}




imx219_settings_t stop_regs[] = {
  {0x0100, 0x00}, /* mode select streaming off */
  IMX219_END_SETTINGS
};

int SensorBase::stream_stop() 
{
  return imx219_i2c_write_table(this->i2c_client, stop_regs);
}





uint16_t gain_integration_times[INTEGRATION_TIMES] = {
  0x00a,
  0x00b,
  0x00c,
  0x00e,
  0x010,
  0x012,
  0x014,
  0x016,
  0x019,
  0x01c,
  0x020,
  0x024,
  0x028,
  0x02d,
  0x033,
  0x039,
  0x040,
  0x048,
  0x051,
  0x05b,
  0x066,
  0x072,
  0x080,
  0x090,
  0x0a2,
  0x0b6,
  0x0cc,
  0x0e5,
  0x101,
  0x120,
  0x143,
  0x16b,
  0x197,
  0x1c9,
  0x201,
  0x23f,
  0x286,
  0x2d4,
  0x32d,
  0x390,
  0x400,
};


uint8_t gain_analogue_gains[ANALOGUE_GAINS + 1] = {
  0,
  28,
  53,
  75,
  95,
  112,
  128,
  142,
  155,
  166,
  175,
  184,
  192,
  199,
  205,
  211,
  215,
  220,
  224,
  227,
  231,
};


uint16_t gain_digital_gains[DIGITAL_GAINS + 1] = {
  0x0100,
  0x011f,
  0x0142,
  0x0169,
  0x0195,
  0x01c7,
  0x01fe,
  0x023d,
  0x0283,
  0x02d1,
  0x0329,
  0x038c,
  0x03fb,
  0x0477,
  0x0503,
  0x059f,
  0x064f,
  0x0714,
  0x07f1,
  0x08e9,
  0x0a00,
  0x0b38,
  0x0c96,
  0x0e20,
  0x0fd9,
};

int SensorBase::set_exposure(uint32_t dBGain) 
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
  int ret = 0;
  ret |= imx219_i2c_write(this->i2c_client, 0x0157, again);
  ret |= imx219_i2c_write(this->i2c_client, 0x0158, dgain >> 8);
  ret |= imx219_i2c_write(this->i2c_client, 0x0159, dgain & 0xff);
  ret |= imx219_i2c_write(this->i2c_client, 0x015A, time >> 8);
  ret |= imx219_i2c_write(this->i2c_client, 0x015B, time & 0xff);
  return ret;
}



// int imx219::SensorBase::set_binning(uint32_t H_binning, uint32_t V_binning)
// {
// }


int SensorBase::read(uint16_t addr)
{
  int res;
  uint16_t val = imx219_i2c_read_reg16(this->i2c_client, 
                                       addr, 
                                       &res);
  assert(res == 0);
  return val;
}


void SensorBase::read_gains(uint16_t values[5])
{
  values[0] = this->read(0x0157);
  values[1] = this->read(0x0158);
  values[2] = this->read(0x0159);
  values[3] = this->read(0x015A);
  values[4] = this->read(0x015B);
}



// These templates avoid needing to include all of the underlying arrays
// in the binary if they are not used.

template<>
imx219_settings_t* imx219::get_res_array<RES_640_480>()
{ return mode_640_480_regs; }

template<>
imx219_settings_t* imx219::get_res_array<RES_1640_1232>()
{ return mode_1640_1232_regs; }


template<>
imx219_settings_t* imx219::get_pxl_fmt_array<FMT_RAW10>()
{ return raw10_framefmt_regs; }

template<>
imx219_settings_t* imx219::get_pxl_fmt_array<FMT_RAW8>()
{ return raw8_framefmt_regs; }
