// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include "imx219_reg.h"

#ifndef GAIN_DB
#define GAIN_DB 40
#endif

#ifdef __cplusplus

#include "SensorBase.hpp"
#include "sensor.h"

namespace sensor {

template<resolution_t TRES>
i2c_table_t get_res_table();

template<resolution_t TRES>
i2c_table_t get_binning_mode();

template<pixel_format_t TFMT>
i2c_table_t get_pxl_fmt_table();

template <resolution_t TRES, pixel_format_t TFMT>
class IMX219 : public SensorBase {

  private:

    void calculate_exposure_gains(uint32_t dBGain, i2c_line_t * exposure_regs);

  public:

    // initialize i2c master as well
    IMX219(i2c_config_t _conf);

    // basic camera init
    int initialize();

    int stream_start();

    int stream_stop();

    int set_exposure(uint32_t dBGain);

    // set resolution, binnig mode and a raw format
    int configure();

    // thread entry, will do all initialization inside and run a control loop
    void control(chanend_t c_control);

}; // IMX219

} // sensor

template <resolution_t TRES, pixel_format_t TFMT>
sensor::IMX219<TRES, TFMT>::IMX219(i2c_config_t _conf) : sensor::SensorBase(_conf) {};

template <resolution_t TRES, pixel_format_t TFMT>
int sensor::IMX219<TRES, TFMT>::initialize() {
  int ret = 0;
  // Send all registers that are common to all modes
  ret = this->i2c_write_table(GET_TABLE(imx219_common_regs));
  // Configure two or four Lane mode
  ret |= this->i2c_write_table(GET_TABLE(imx219_lanes_regs));
  // set gain
  ret |= this->set_exposure(GAIN_DB);
  return ret;
}

template <resolution_t TRES, pixel_format_t TFMT>
int sensor::IMX219<TRES, TFMT>::stream_start() {
  return this->i2c_write_table(GET_TABLE(start_regs));
}

template <resolution_t TRES, pixel_format_t TFMT>
int sensor::IMX219<TRES, TFMT>::stream_stop() {
  return this->i2c_write_table(GET_TABLE(stop_regs));
}

template <resolution_t TRES, pixel_format_t TFMT>
int sensor::IMX219<TRES, TFMT>::set_exposure(uint32_t dBGain) {
  i2c_line_t exposure_regs[5] = {{0}};
  this->calculate_exposure_gains(dBGain, exposure_regs);
  return this->i2c_write_table(GET_TABLE(exposure_regs));
}

template <resolution_t TRES, pixel_format_t TFMT>
int sensor::IMX219<TRES, TFMT>::configure() {
  i2c_table_t frame_size_regs = get_res_table<TRES>();
  i2c_table_t binning_regs = get_binning_mode<TRES>();
  i2c_table_t pixel_format_regs = get_pxl_fmt_table<TFMT>();

  int ret = 0;
  // Apply default values of current mode
  ret |= this->i2c_write_table(frame_size_regs);
  // set frame format register
  ret |= this->i2c_write_table(pixel_format_regs);
  // set binning
  ret |= this->i2c_write_table(binning_regs);
  return ret;
}

template <resolution_t TRES, pixel_format_t TFMT>
void sensor::IMX219<TRES, TFMT>::calculate_exposure_gains(uint32_t dBGain, i2c_line_t * exposure_regs) {
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
}

template <resolution_t TRES, pixel_format_t TFMT>
void sensor::IMX219<TRES, TFMT>::control(chanend_t c_control) {
  // Init the I2C sensor first configuration
  int r = 0;
  r |= this->initialize();
  delay_milliseconds(100);
  r |= this->configure();
  delay_milliseconds(600);
  r |= this->stream_start();
  delay_milliseconds(600);
  xassert((r == 0) && "Could not initialise camera");
  puts("\nCamera_started and configured...");

  // store the response
  uint32_t encoded_response;
  sensor_control_t cmd;
  uint8_t arg;

  // sensor control logic
  while(1){
    encoded_response = chan_in_word(c_control);
    chan_out_word(c_control, 0);
    cmd = (sensor_control_t) DECODE_CMD(encoded_response);

    #if ENABLE_PRINT_SENSOR_CONTROL
      printf("--------------- Received command %d\n", cmd);
    #endif
    
    switch (cmd)
    {
    case SENSOR_INIT:
      this->initialize();
      break;
    case SENSOR_CONFIG:
      //TODO reimplement when dynamic configuration is supported
      this->configure();
      break;
    case SENSOR_STREAM_START:
      this->stream_start();
      break;
    case SENSOR_STREAM_STOP:
      this->stream_stop();            
      break;
    case SENSOR_SET_EXPOSURE:
      arg = DECODE_ARG(encoded_response);
      this->set_exposure(arg);
      break;
    default:
      break;
    }
  }
}

template<>
i2c_table_t sensor::get_res_table<RES_640_480>()
{ return GET_TABLE(mode_640_480_regs); }

template<>
i2c_table_t sensor::get_res_table<RES_1280_960>()
{ return GET_TABLE(mode_1280_960_regs); }

template<>
i2c_table_t sensor::get_binning_mode<RES_640_480>()
{ return GET_TABLE(binning_2x2_regs); }

template<>
i2c_table_t sensor::get_binning_mode<RES_1280_960>()
{ return GET_TABLE(binning_2x2_regs); }

template<>
i2c_table_t sensor::get_pxl_fmt_table<FMT_RAW8>()
{ return GET_TABLE(raw8_framefmt_regs); }

template<>
i2c_table_t sensor::get_pxl_fmt_table<FMT_RAW10>()
{ return GET_TABLE(raw10_framefmt_regs); }

#endif // __cplusplus
