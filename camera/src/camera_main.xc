
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h> // for ports
#include <xccompat.h>
 
#include "i2c.h"
#include "camera_main.h"
#include "mipi_defines.h"
#include "packet_handler.h"
#include "statistics.h"
#include "sensor_control.h"



void camera_main(
    tileref mipi_tile,
    in port p_mipi_clk,
    in port p_mipi_rxa,
    in port p_mipi_rxv,
    buffered in port:32 p_mipi_rxd,
    clock clk_mipi,
    client interface i2c_master_if i2c,
    chanend c_stop)
{

  streaming chan c_pkt;
  streaming chan c_ctrl;
  streaming chan c_stat_thread;
  sensor_control_if sc_if;

  // set the channels for the camera api
  camera_api_init();
  
  // Assign lanes and polarities
  write_node_config_reg(mipi_tile,
                        XS1_SSWITCH_MIPI_DPHY_CFG3_NUM,
                        DEFAULT_MIPI_DPHY_CFG3);

  // Configure MIPI shim
  unsigned mipi_shim_cfg0 = MIPI_SHIM_CFG0_PACK(MIPI_SHIM_DEMUX_EN,
                                                MIPI_SHIM_DEMUX_DATATYPE,  
                                                MIPI_SHIM_DEMUX_MODE, 
                                                MIPI_SHIM_STUFF_ENABLE,
                                                1); // enable bias
  // Initialize MIPI receiver
  MipiPacketRx_init(mipi_tile,
                    p_mipi_rxd,
                    p_mipi_rxv,
                    p_mipi_rxa,
                    p_mipi_clk,
                    clk_mipi,
                    mipi_shim_cfg0,
                    MIPI_CLK_DIV,
                    MIPI_CFG_CLK_DIV); 

  // Start camera and its configurations
  int r = 0;
  r |= camera_init(i2c);
  delay_milliseconds(100);
  r |= camera_configure(i2c);
  delay_milliseconds(600);
  r |= camera_start(i2c);
  assert(r == 0); // assert that camera is started and configured
  printf("\nCamera_started and configured...\n");
  delay_milliseconds(2000);

  // start the different jobs (packet controller, handler, and post_process)
  par
  {
    MipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, c_stat_thread, c_stop);
    statistics_thread(c_stat_thread, sc_if);
    sensor_control(sc_if, i2c);
  }
}
