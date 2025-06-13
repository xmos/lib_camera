// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <xcore/assert.h>
#include <xcore/chanend.h>
#include <xcore/channel.h>
#include "debug_print.h"

#include "camera.h"
#include "camera_mipi.h"
#include "sensor_base.hpp"
#include "camera_utils.h"

#include "sensor_wrapper.h"

#define MSG1 "Camera sensor I2C channel already set"
#define MSG2 "Camera sensor I2C channel not set"

chanend_t c_i2c = 0;

void camera_sensor_set_channel(chanend_t c_in)
{
  xassert(c_i2c == 0 && MSG1);
  c_i2c = c_in;
  debug_printf("Camera sensor I2C channel set\n");
}

void camera_sensor_init()
{
  xassert(c_i2c != 0 && MSG2);
  chan_out_word(c_i2c, SENSOR_INIT);
}

void camera_sensor_start()
{
  xassert(c_i2c != 0 && MSG2);
  chan_out_word(c_i2c, SENSOR_STREAM_START);
}

void camera_sensor_stop()
{
  xassert(c_i2c != 0 && MSG2);
  chan_out_word(c_i2c, SENSOR_STREAM_STOP);
}

void camera_sensor_set_tp(uint16_t pattern)
{
  xassert(c_i2c != 0 && MSG2);
  chan_out_word(c_i2c, SENSOR_SET_TEST_PATTERN);
  chan_out_word(c_i2c, pattern);
}

void camera_sensor_set_exposure(uint32_t dBGain)
{
  xassert(c_i2c != 0 && MSG2);
  chan_out_word(c_i2c, SENSOR_SET_EXPOSURE);
  chan_out_word(c_i2c, dBGain);
}
