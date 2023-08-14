// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xcore/channel.h>

#include "sensor_control.h"
#include "imx219.hpp"

i2c_master_t i2c_ctx;
i2c_config_t i2c_conf;

void sensor_control(chanend_t c_control) {
  // I2C settings
  i2c_conf.device_addr = 0x10;
  i2c_conf.speed = 400;
  i2c_conf.p_scl = XS1_PORT_1N;
  i2c_conf.p_sda = XS1_PORT_1O;
  i2c_conf.i2c_ctx_ptr = &i2c_ctx;

  sensor::IMX219<sensor::RES_640_480, sensor::FMT_RAW8> snsr(i2c_conf);

  puts("\nI2C initialized...");

  // Init the I2C sensor first configuration
  int r = 0;
  r |= snsr.initialize();
  delay_milliseconds(100);
  r |= snsr.configure();
  delay_milliseconds(600);
  r |= snsr.stream_start();
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
      snsr.initialize();
      break;
    case SENSOR_CONFIG:
      //TODO reimplement when dynamic configuration is supported
      snsr.configure();
      break;
    case SENSOR_STREAM_START:
      snsr.stream_start();
      break;
    case SENSOR_STREAM_STOP:
      snsr.stream_stop();            
      break;
    case SENSOR_SET_EXPOSURE:
      arg = DECODE_ARG(encoded_response);
      snsr.set_exposure(arg);
      break;
    default:
      break;
    }
  }
}
