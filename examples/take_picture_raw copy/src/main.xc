#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

#include "app_raw.h"

// I2C interface ports
#define Kbps 400
on tile[0]: port p_scl = XS1_PORT_1N;
on tile[0]: port p_sda = XS1_PORT_1O;

extern "C" {
#include "xscope_io_device.h"
}

extern "C" {
  void sensor_i2c_start();
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


void camera_main() 
{
  streaming chan c_stat_thread;
  streaming chan c_pkt;
  streaming chan c_ctrl;

  camera_mipi_init(
    p_mipi_clk,
    p_mipi_rxa,
    p_mipi_rxv,
    p_mipi_rxd,
    clk_mipi);
  
  par{
    MipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, c_stat_thread);
    isp_pipeline(c_stat_thread);
  }
}
 


int main(void)
{
  // Declarations
  chan xscope_chan;
  
  // Parallel jobs
  par{
    // I2C
    on tile[0]: sensor_i2c_start();
    // Xscope and i2c
    xscope_host_data(xscope_chan);
    on tile[MIPI_TILE]: xscope_io_init(xscope_chan);
    // Camera
    on tile[MIPI_TILE]: camera_main();
    on tile[MIPI_TILE]: user_app();
  }
  return 0;
}
