// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <xcore/assert.h>
#include <xcore/channel.h>

#include "sensor_settings.h"

#ifdef __cplusplus

namespace sensor {

class SensorBase {

  private:

    i2c_config_t i2c_cfg;

    void i2c_init();

  protected:

    uint16_t i2c_read(uint16_t reg);

    int i2c_write_line(i2c_line_t line);

    int i2c_write_line(uint16_t reg, uint8_t val);

    int i2c_write_table(i2c_table_t table);

  public:

    SensorBase(i2c_config_t _conf);

}; // SensorBase

} // sensor

#endif // __cplusplus
