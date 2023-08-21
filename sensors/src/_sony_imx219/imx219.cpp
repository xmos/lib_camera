// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include <xcore/assert.h>

#include "imx219.hpp"

using namespace sensor;

// This header has to be after imx219.hpp and the namespace
#include "imx219_reg.h"

IMX219::IMX219(i2c_config_t _conf,resolution_t _res, pixel_format_t _pix_fmt, bool _binning, bool _centralize)
        : sensor::SensorBase(_conf), frame_res(_res), pix_fmt(_pix_fmt),
        binning_2x2(_binning) {
  this->get_x_y_len();
  this->get_offsets_and_check_ranges(_centralize);
  this->adjust_offsets();
}

IMX219::IMX219(i2c_config_t _conf,resolution_t _res, pixel_format_t _pix_fmt, bool _binning, uint16_t _x_offset, uint16_t _y_offset)
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
  ret |= this->set_exposure(GAIN_DB);
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

int IMX219::configure() {
  i2c_table_t frame_size_regs = this->get_res_table();
  i2c_table_t pix_format_regs = this->get_pxl_fmt_table();
  i2c_line_t binning_reg[1] = {{BINNING_MODE_REG, 0}};
  binning_reg[0].reg_val = (this->binning_2x2) ? BINNING_2X2 : BINNING_NONE;

  int ret = 0;
  // Apply default values of current mode
  ret |= this->i2c_write_table(frame_size_regs);
  // set frame format register
  ret |= this->i2c_write_table(pix_format_regs);
  // set binning
  ret |= this->i2c_write_table(GET_TABLE(binning_reg));
  return ret;
}

void IMX219::control(chanend_t c_control) {
  // Init the I2C sensor first configuration
  int ret = 0;
  ret |= this->initialize();
  delay_milliseconds(100);
  ret |= this->configure();
  delay_milliseconds(600);
  ret |= this->stream_start();
  delay_milliseconds(600);
  xassert((ret == 0) && "Could not initialise camera");
  puts("\nCamera_started and configured...");

  // store the response
  uint32_t encoded_response;
  sensor_control_t cmd;
  uint8_t arg;

  // sensor control logic
  while(1) {
    encoded_response = chan_in_word(c_control);
    chan_out_word(c_control, 0);
    cmd = (sensor_control_t) DECODE_CMD(encoded_response);

    #if ENABLE_PRINT_SENSOR_CONTROL
      printf("--------------- Received command %d\n", cmd);
    #endif
    
    switch (cmd)
    {
    case SENSOR_INIT:
      ret = this->initialize();
      break;
    case SENSOR_CONFIG:
      //TODO reimplement when dynamic configuration is supported
      ret = this->configure();
      break;
    case SENSOR_STREAM_START:
      ret = this->stream_start();
      break;
    case SENSOR_STREAM_STOP:
      ret = this->stream_stop();            
      break;
    case SENSOR_SET_EXPOSURE:
      arg = DECODE_ARG(encoded_response);
      ret = this->set_exposure(arg);
      break;
    default:
      break;
    }
    xassert((ret == 0) && "Could not perform I2C write");
  }
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
  uint16_t val = 0;
  if(pix_fmt == FMT_RAW8) {
    val = 0x08;
  } else if(pix_fmt == FMT_RAW10) {
    val = 0x0a;
  } else {
    xassert(0 && "Pixel format has to be either RAW8 or RAW10");
  }
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
  if(this->frame_res == RES_640_480) {
    this->x_len = 640;
    this->y_len = 480;
  } else if (this->frame_res == RES_1280_960) {
    this->x_len = 1280;
    this->y_len = 960;
  } else {
    xassert(0 && "Given resolution format is not supported");
  }
}

void IMX219::get_offsets_and_check_ranges(bool centralize) {
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
