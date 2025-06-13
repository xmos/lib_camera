// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <xcore/assert.h>
#include <xcore/select.h>
#include "debug_print.h"

#include "camera.h"
#include "camera_mipi.h"
#include "sensor_base.hpp"
#include "camera_utils.h"

#include "sensor_imx219.hpp"

#include "sensor_wrapper.h"
#include "board_support.h"

using namespace sensor;

// Global sensor object pointer
IMX219* camera_sensor_ptr = nullptr;
i2c_master_t i2c_ctx;
i2c_config_t i2c_conf;

void camera_sensor_init_rx() {
  // I2C settings
  i2c_conf.device_addr = I2C_DEV_ADDR;
  i2c_conf.speed = I2C_DEV_SPEED;
  i2c_conf.p_scl = PORT_I2C_SCL;
  i2c_conf.p_sda = PORT_I2C_SDA;
  i2c_conf.i2c_ctx_ptr = &i2c_ctx;

  // Sensor settings
  resolution_t res = {
    .sensor_width = SENSOR_WIDTH,
    .sensor_height = SENSOR_HEIGHT
  };
  
  // Global sensor object
  camera_sensor_ptr = new IMX219(
    (i2c_config_t)i2c_conf, 
    (resolution_t)res, 
    (pixel_format_t)MIPI_DT_RAW8, 
    (binning_t)CONFIG_BINNING, 
    (centralise_t)CONFIG_CENTRALISE);

  // Init the I2C sensor first configuration
  debug_printf("Camera init\n");
  int ret = 0;
  ret |= camera_sensor_ptr->initialize();
  delay_milliseconds_cpp(100);
  ret |= camera_sensor_ptr->configure();
  delay_milliseconds_cpp(500);
  ret |= camera_sensor_ptr->stream_start();
  delay_milliseconds_cpp(500);
  xassert((ret == 0) && "Could not initialise camera");
  debug_printf("Camera_started and configured\n");
}

void camera_sensor_start_rx() {
  int ret = camera_sensor_ptr->stream_start();
  xassert((ret == 0) && "Could not start camera");
}

void camera_sensor_stop_rx() {
  int ret = camera_sensor_ptr->stream_stop();
  xassert((ret == 0) && "Could not stop camera");
}

void camera_sensor_set_tp_rx(uint16_t pattern){
  int ret = camera_sensor_ptr->set_test_pattern(pattern);
  xassert((ret == 0) && "Could not set test pattern");
}

void camera_sensor_set_exposure_rx(uint32_t dBGain){
  int ret = camera_sensor_ptr->set_exposure(dBGain);
  xassert((ret == 0) && "Could not set exposure");
}

void camera_i2c_thread(chanend_t c_i2c)
{
	sensor_control_t cmd = SENSOR_INIT;
	SELECT_RES(CASE_THEN(c_i2c, c_i2c_handler))
	{
	c_i2c_handler:
		cmd = (sensor_control_t)chan_in_word(c_i2c);
		switch (cmd) {
			case SENSOR_INIT: {
				camera_sensor_init_rx(); 
				xassert(camera_sensor_ptr != nullptr && "Camera sensor pointer is null");
				break;
			}
			case SENSOR_STREAM_START: {
				camera_sensor_start_rx(); 
				break;
			}
			case SENSOR_STREAM_STOP: {
				camera_sensor_stop_rx(); 
				break;
			}
			case SENSOR_SET_EXPOSURE: {
				uint32_t dBGain = chan_in_word(c_i2c);
				camera_sensor_set_exposure_rx(dBGain);
				break;
			}
			case SENSOR_SET_TEST_PATTERN: {
				uint16_t pattern = chan_in_word(c_i2c);
				camera_sensor_set_tp_rx(pattern);
				break;
			}
			default: {
				xassert(0 && "Unknown sensor control command");
				break;
			}
		}
		continue;
	}
}
