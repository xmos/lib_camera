// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>

#include "app_raw.h"
#include "sensor_control.h"

/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A;   // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;

extern "C" {
#include "xscope_io_device.h"
}

// Camera control channels
void main_tile0(chanend_t c_control){
  sensor_control(c_control);
}

// Camera image processing channels
void main_tile1(chanend_t c_control) 
{
  streaming chan c_pkt;
  streaming chan c_ctrl;
  chan c_isp;

  camera_mipi_init(
    p_mipi_clk,
    p_mipi_rxa,
    p_mipi_rxv,
    p_mipi_rxd,
    clk_mipi);
  
  par{
    MipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, c_isp);
    isp_thread(c_isp, c_control);
    user_app();
  }
}


int main(void)
{
  // Channel declarations
  chan xscope_chan;
  chan c_control;

  // Parallel jobs
  par{
    on tile[0]: main_tile0(c_control);
    on tile[1]: main_tile1(c_control);
    // xscope
    xscope_host_data(xscope_chan);
    on tile[1]: xscope_io_init(xscope_chan);
  }
  return 0;
}
