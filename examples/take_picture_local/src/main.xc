#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

#include "i2c.h"
#include "app.h"
#include "packet_rx_simulate.h"

// I2C interface ports
#define Kbps 400
on tile[0]: port p_scl = XS1_PORT_1N;
on tile[0]: port p_sda = XS1_PORT_1O;

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

void camera_main(
  client interface i2c_master_if i2c) 
{
  streaming chan c_stat_thread;
  streaming chan c_pkt;
  streaming chan c_ctrl;
  sensor_control_if sc_if;

  camera_mipi_init(tile[MIPI_TILE],
    p_mipi_clk,
    p_mipi_rxa,
    p_mipi_rxv,
    p_mipi_rxd,
    clk_mipi);
  
  sensor_start(i2c);

  par{
    MipiPacketRx_simulate(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, c_stat_thread);
    isp_pipeline(c_stat_thread, sc_if);
    sensor_control(sc_if, i2c);
  }
}


int main(void)
{
  // Declarations
  chan xscope_chan;
  i2c_master_if i2c[1];  
  
  // Parallel jobs
  par{
    // Xscope and i2c
    xscope_host_data(xscope_chan);
    on tile[MIPI_TILE]: xscope_io_init(xscope_chan);
    // Camera
    on tile[0]: i2c_master(i2c, 1, p_scl, p_sda, Kbps);
    on tile[MIPI_TILE]: camera_main(i2c[0]);
    on tile[MIPI_TILE]: user_app();
  }
  return 0;
}
