// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <platform.h>
#include <xscope.h>

#include "app.h"

extern "C" {
  void sensor_i2c_init();
  void sensor_control(chanend_t c_control);
}

// I2C interface ports
on tile[0]: port p_scl = XS1_PORT_1N;
on tile[0]: port p_sda = XS1_PORT_1O;

// astew: TIL xscope_user_init() is an XC magic function that gets called
//        automatically..for some reason.

void xscope_user_init() {
  xscope_register(0, 0, "", 0, "");
  xscope_config_io(XSCOPE_IO_BASIC);
}

int main(void) 
{
  chan c_control;
  par {
    on tile[0]: sensor_i2c_init();
    on tile[0]: sensor_control(c_control);
    on tile[MIPI_TILE]: mipi_main(c_control);
  }
  return 0;
}
