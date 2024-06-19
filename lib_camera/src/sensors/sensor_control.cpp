// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.


#include "sensor_control.h"
#include "sensor_base.hpp"
#include "imx219.hpp"

void sensor_control(chanend_t c_control) {
  
  i2c_master_t i2c_ctx;
  sensor::i2c_config_t i2c_conf;

  // I2C settings
  i2c_conf.device_addr = I2C_DEV_ADDR;
  i2c_conf.speed = I2C_DEV_SPEED;
  i2c_conf.p_scl = XS1_PORT_4E;
  i2c_conf.p_sda = XS1_PORT_4E;
  i2c_conf.i2c_ctx_ptr = &i2c_ctx;

  const bool binning = true;
  const bool centralise = true;

  // Create sensor object, to change if different sensor is used
  sensor::IMX219 snsr(
    i2c_conf, 
    (resolution_t)CONFIG_MODE, 
    (pixel_format_t)CONFIG_MIPI_FORMAT, 
    binning, 
    centralise);

  // Initialize sensor loop
  snsr.control(c_control);
}
