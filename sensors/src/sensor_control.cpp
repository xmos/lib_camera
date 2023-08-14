// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

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

  snsr.control(c_control);
}
