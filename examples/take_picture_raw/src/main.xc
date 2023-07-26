#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

#include "app_raw.h"

typedef chanend chanend_t;
typedef enum {
    SENSOR_INIT = 0,
    SENSOR_CONFIG,
    SENSOR_STREAM_START,
    SENSOR_STREAM_STOP,
    SENSOR_SET_EXPOSURE
} camera_control_t;

// I2C interface ports
#define Kbps 400
on tile[0]: port p_scl = XS1_PORT_1N;
on tile[0]: port p_sda = XS1_PORT_1O;

extern "C" {
#include "xscope_io_device.h"
}

extern "C" {
  void sensor_i2c_start(chanend_t schan[]);
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


void main_tile1(chanend_t schan[]) 
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
    isp_pipeline(c_stat_thread, schan[SENSOR_STREAM_STOP], schan[SENSOR_SET_EXPOSURE]);
    user_app();
  }
}


int main(void)
{
  // Camera control channels
  chan schan[5];
  chan xscope_chan;

  // Parallel jobs
  par{
    on tile[0]: sensor_i2c_start(schan);
    on tile[MIPI_TILE]: main_tile1(schan);
    // xscope
    xscope_host_data(xscope_chan);
    on tile[MIPI_TILE]: xscope_io_init(xscope_chan);
  }
  return 0;
}
