// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include "sensor_control.h"
#include "imx219.hpp"

using namespace sensor;

i2c_master_t i2c_ctx;
i2c_config_t i2c_conf;

void sensor_control(chanend_t c_control) {
  // I2C settings
  i2c_conf.device_addr = I2C_DEV_ADDR;
  i2c_conf.speed = I2C_DEV_SPEED;
  i2c_conf.p_scl = XS1_PORT_1N;
  i2c_conf.p_sda = XS1_PORT_1O;
  i2c_conf.i2c_ctx_ptr = &i2c_ctx;

  const bool binning = false;
  const bool centralise = false;

  IMX219 snsr(
    i2c_conf, 
    (resolution_t)CONFIG_MODE, 
    (pixel_format_t)CONFIG_MIPI_FORMAT, 
    binning, 
    centralise);

  snsr.control(c_control);
}
