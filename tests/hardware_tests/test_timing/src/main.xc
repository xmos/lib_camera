// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include "i2c.h"
#include "app.h"

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
  i2c_master_if i2c[1];
  par {
    on tile[0]: i2c_master(i2c, 1, p_scl, p_sda, 400);
    on tile[MIPI_TILE]: mipi_main(i2c[0]);

  }
  return 0;
}
