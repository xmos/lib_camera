// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

#include "app.h"
#include "sensor_control.h"
#include "camera_process.h"

extern "C" {
#include "xscope_io_device.h"
}

// Declaration of the MIPI interface ports:
// Clock, receiver active, receiver data valid, and receiver data
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A;   // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;

int main(void)
{
  // Channel declarations
  chan xscope_chan;
  chan c_control;

  // Parallel jobs
  par{
    on tile[0]: sensor_control(c_control);
    on tile[1]: camera_process(c_control);
    on tile[1]: user_app();
    // xscope
    xscope_host_data(xscope_chan);
    on tile[1]: xscope_io_init(xscope_chan);
  }
  return 0;
}
