// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>

#include "app.h"
#include "sensor_control.h"
#include "packet_rx_simulate.h"

extern "C" {
#include "xscope_io_device.h"
}

/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A;   // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;


// Camera image processing channels
void main_tile1() 
{
  streaming chan c_pkt;
  streaming chan c_ctrl;
  chan c_isp;
  chan c_control;

  camera_mipi_init(
    p_mipi_clk,
    p_mipi_rxa,
    p_mipi_rxv,
    p_mipi_rxd,
    clk_mipi);
  
  par{
    MipiPacketRx_simulate(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, c_isp);
    isp_thread(c_isp, c_control);
    sensor_control(c_control);
    user_app();
  }
}


int main(void)
{
  // Channel declarations
  chan xscope_chan;

  // Parallel jobs
  par
  {
    // xscope
    xscope_host_data(xscope_chan);
    on tile[1]: xscope_io_init(xscope_chan);
    // MIPI, ISP, I2C, and user app
    on tile[1]: main_tile1();
  }
  return 0;
}
