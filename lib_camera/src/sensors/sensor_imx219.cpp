// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include <platform.h>
#include <xcore/assert.h>
#include <xcore/select.h>
#include <print.h>

#include "sensor_imx219.hpp"
#include "camera_isp.h"

using namespace sensor;

// This header has to be after sensor_imx219.hpp and the namespace
#include "sensor_imx219_reg.h"

IMX219::IMX219(
  i2c_config_t _conf, 
  resolution_t _res, 
  pixel_format_t _pix_fmt, 
  binning_t _binning, 
  centralise_t _centralize) \
  : sensor::SensorBase(_conf), frame_res(_res), pix_fmt(_pix_fmt), binning_2x2(_binning) {
  this->get_x_y_len();
  this->get_offsets_and_check_ranges(_centralize);
  this->adjust_offsets();
}

IMX219::IMX219(
  i2c_config_t _conf, 
  resolution_t _res, 
  pixel_format_t _pix_fmt, 
  binning_t _binning, 
  uint16_t _x_offset, uint16_t _y_offset) \
  : sensor::SensorBase(_conf), frame_res(_res), pix_fmt(_pix_fmt),
  binning_2x2(_binning), x_offset(_x_offset), y_offset(_y_offset) {
  this->get_x_y_len();
  this->check_ranges();
  this->adjust_offsets();
}

int IMX219::initialize() {
  int ret = 0;
  // Send all registers that are common to all modes
  ret |= this->i2c_write_table(GET_TABLE(imx219_common_regs));
  // Configure two or four Lane mode
  ret |= this->i2c_write_table(GET_TABLE(imx219_lanes_regs));
  // set gain
  ret |= this->set_exposure(GAIN_DEFAULT_DB);
  // set orientation
  ret |= this->set_orientation(CONFIG_FLIP);
  return ret;
}

int IMX219::configure() {
  i2c_table_t frame_size_regs = this->get_res_table();
  i2c_table_t pix_format_regs = this->get_pxl_fmt_table();
  i2c_line_t binning_reg[1] = {{BINNING_MODE_REG, 0}};
  binning_reg[0].reg_val = (this->binning_2x2) ? BINNING_2X2 : BINNING_NONE;

  int ret = 0;
  // Check if the sensor is connected
  ret |= this->check_sensor_is_connected();
  // Apply default values of current mode
  ret |= this->i2c_write_table(frame_size_regs);
  // set frame format register
  ret |= this->i2c_write_table(pix_format_regs);
  // set binning
  ret |= this->i2c_write_table(GET_TABLE(binning_reg));
  return ret;
}

int IMX219::stream_start() {
  return this->i2c_write_table(GET_TABLE(start_regs));
}

int IMX219::stream_stop() {
  return this->i2c_write_table(GET_TABLE(stop_regs));
}

int IMX219::set_exposure(uint32_t dBGain) {
  i2c_table_t exposure_regs = this->get_exp_gains_table(dBGain);
  return this->i2c_write_table(exposure_regs);
}

int IMX219::set_orientation(orientation_t orientation) {
  i2c_line_t orient_reg = {REG_ORIENTATION, (uint16_t)orientation};
  return this->i2c_write_line(orient_reg);
}

i2c_table_t IMX219::get_exp_gains_table(uint32_t dBGain) {
  static i2c_line_t exposure_regs[5];
  static i2c_table_t exposure_table = {exposure_regs, 5};
  uint16_t time, dgain;
  uint8_t again;
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
  exposure_regs[0] = {0x0157, again};
  exposure_regs[1] = {0x0158, (uint8_t)(dgain >> 8)};
  exposure_regs[2] = {0x0159, (uint8_t)(dgain)};
  exposure_regs[3] = {0x015A, (uint8_t)(time >> 8)};
  exposure_regs[4] = {0x015B, (uint8_t)(time)};
  return exposure_table;
}

i2c_table_t IMX219::get_pxl_fmt_table() {
  static i2c_line_t format_regs[3];
  static i2c_table_t format_table = {format_regs, 3};
  uint16_t val = 0x08;
  xassert(pix_fmt == MIPI_DT_RAW8 && "Pixel format has to be RAW8");
  format_regs[0] = {0x018c, val};
  format_regs[1] = {0x018d, val};
  format_regs[2] = {0x0309, val};
  return format_table;
}

i2c_table_t IMX219::get_res_table() {
  static i2c_line_t resolution_regs[12];
  static i2c_table_t resolution_table = {resolution_regs, 12};
  uint16_t x_full_len = (this->binning_2x2) ? this->x_len * 2 : this->x_len;
  uint16_t y_full_len = (this->binning_2x2) ? this->y_len * 2 : this->y_len;
  uint16_t x_end = this->x_offset + x_full_len - 1;
  uint16_t y_end = this->y_offset + y_full_len - 1;
  // x start - end
  resolution_regs[0]  = {0x0164, (uint8_t)(this->x_offset >> 8)};
  resolution_regs[1]  = {0x0165, (uint8_t)(this->x_offset)};
  resolution_regs[2]  = {0x0166, (uint8_t)(x_end >> 8)};
  resolution_regs[3]  = {0x0167, (uint8_t)(x_end)};
  // y start - end
  resolution_regs[4]  = {0x0168, (uint8_t)(this->y_offset >> 8)};
  resolution_regs[5]  = {0x0169, (uint8_t)(this->y_offset)};
  resolution_regs[6]  = {0x016a, (uint8_t)(y_end >> 8)};
  resolution_regs[7]  = {0x016b, (uint8_t)(y_end)};
  // x, y len (non binned)
  resolution_regs[8]  = {0x016c, (uint8_t)(this->x_len >> 8)};
  resolution_regs[9]  = {0x016d, (uint8_t)(this->x_len)};
  resolution_regs[10] = {0x016e, (uint8_t)(this->y_len >> 8)};
  resolution_regs[11] = {0x016f, (uint8_t)(this->y_len)};
  return resolution_table;
}

void IMX219::get_x_y_len() {
  this->x_len = this->frame_res.sensor_width;
  this->y_len = this->frame_res.sensor_height;
  xassert(this->x_len <= SENSOR_X_LIM && "X resolution exceeds sensor frame limits");
  xassert(this->y_len <= SENSOR_Y_LIM && "Y resolution exceeds sensor frame limits");
}

void IMX219::get_offsets_and_check_ranges(centralise_t centralize) {
  uint16_t x_full_len = (this->binning_2x2) ? this->x_len * 2 : this->x_len;
  uint16_t y_full_len = (this->binning_2x2) ? this->y_len * 2 : this->y_len;
  if((x_full_len > SENSOR_X_LIM) || (y_full_len > SENSOR_Y_LIM)) {
    xassert(0 && "Given resolution and binnig mode exceed sensor frame limits");
  }
  if(centralize) {
    this->x_offset = (SENSOR_X_LIM / 2) - (x_full_len / 2);
    this->y_offset = (SENSOR_Y_LIM / 2) - (y_full_len / 2);
  } else {
    this->x_offset = 0;
    this->y_offset = 0;
  }
}

void IMX219::adjust_offsets() {
  if(this->x_offset % 2) {
    this->x_offset -= 1;
    puts("Warning: X offset has been adjusted to be even");
  }
  if(this->y_offset % 2) {
    this->y_offset -= 1;
    puts("Warning: Y offset has been adjusted to be even");
  }
}

void IMX219::check_ranges() {
  uint16_t x_full_len = (this->binning_2x2) ? this->x_len * 2 : this->x_len;
  uint16_t y_full_len = (this->binning_2x2) ? this->y_len * 2 : this->y_len;
  if(((this->x_offset + x_full_len) >= SENSOR_X_LIM) || ((this->y_offset + y_full_len) >= SENSOR_Y_LIM)) {
    xassert(0 && "Given resolution and binnig mode exceed sensor frame limits");
  }
}

void IMX219::control(chanend_t c_control) {
  // as there is no dynamic control
  // this function is empty  
}

int IMX219::set_test_pattern(uint16_t pattern) {
  i2c_line_t test_pattern_regs[] = {
        {REG_TEST_PATTERN_MSB, 0x00},
        {REG_TEST_PATTERN_LSB, pattern},
        {REG_TP_X_OFFSET_MSB, 0x00},     // tp offset x 0
        {REG_TP_X_OFFSET_LSB, 0x00},
        {REG_TP_Y_OFFSET_MSB, 0x00}, // tp offset y 0
        {REG_TP_Y_OFFSET_LSB, 0x00},
        {REG_TP_WIDTH_MSB, 0x02}, // TP width 1920(0x780) 1280(0x500) 640(0x280)
        {REG_TP_WIDTH_LSB, 0x80},
        {REG_TP_HEIGHT_MSB, 0x01}, // TP height 1080(0x438) 960(0x3C0) 480(0x1E0)
        {REG_TP_HEIGHT_LSB, 0xE0},
  };
  i2c_table_t test_table = {test_pattern_regs, 10};
  int ret = this->i2c_write_table(test_table);
  return ret;
}

int IMX219::check_sensor_is_connected()
{
  port_t flashing_led_port = PORT_LED;
  unsigned flashing_seconds = 10;
  unsigned flashing_delay_ms = 80;
  unsigned flashing_delay_ticks = flashing_delay_ms * XS1_TIMER_KHZ;
  unsigned flashing_end = get_reference_time() + flashing_seconds * XS1_TIMER_HZ;

  // read model ID and check if it is correct
  int ret = this->i2c_read(REG_MODEL_ID);
  if (ret != VAL_MODEL_ID) {
    port_enable(flashing_led_port);
    hwtimer_t tmr = hwtimer_alloc();
    while (get_reference_time() <= flashing_end) { // flip red
      port_out(flashing_led_port, (port_peek(flashing_led_port) ^ 0b10));
      hwtimer_delay(tmr, flashing_delay_ticks);
    }
    port_disable(flashing_led_port);
    hwtimer_free(tmr);
    printstrln("ERROR: IMX219 not connected");
    printstrln(">> Verify that the sensor is properly connected");
    return -1;
  }
  return 0;
}
