// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.


#include "sensor_control.h"
#include "sensor_base.hpp"
#include "camera_utils.h"
#include "imx219.hpp"

using namespace sensor;

// Global sensor object pointer
IMX219* camera_sensor_ptr = nullptr;
i2c_master_t i2c_ctx;
i2c_config_t i2c_conf;

void camera_sensor_init() {
  // I2C settings
  i2c_conf.device_addr = I2C_DEV_ADDR;
  i2c_conf.speed = I2C_DEV_SPEED;
  i2c_conf.p_scl = XS1_PORT_4E;
  i2c_conf.p_sda = XS1_PORT_4E;
  i2c_conf.i2c_ctx_ptr = &i2c_ctx;

  // Global sensor object
  camera_sensor_ptr = new IMX219(
    i2c_conf, 
    (resolution_t)CONFIG_MODE, 
    (pixel_format_t)CONFIG_MIPI_FORMAT, 
    CONFIG_BINNING, 
    CONFIG_CENTRALISE);

  // Init the I2C sensor first configuration
  puts("Camera init.\n");
  int ret = 0;
  ret |= camera_sensor_ptr->initialize();
  delay_milliseconds_cpp(100);
  ret |= camera_sensor_ptr->configure();
  delay_milliseconds_cpp(500);
  ret |= camera_sensor_ptr->stream_start();
  delay_milliseconds_cpp(500);
  xassert((ret == 0) && "Could not initialise camera");
  puts("Camera_started and configured.\n");
}

void camera_sensor_start() {
  int ret = camera_sensor_ptr->stream_start();
  xassert((ret == 0) && "Could not start camera");
}

void camera_sensor_stop() {
  int ret = camera_sensor_ptr->stream_stop();
  xassert((ret == 0) && "Could not stop camera");
}
