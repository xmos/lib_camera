#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h> // for ports
#include <xccompat.h>

#include "camera_main.h"

#include "mipi.h"
#include "i2c.h"
#include "isp.h"
#include "sensor_control.h"
#include "packet_handler.h"


void camera_mipi_init(
    tileref mipi_tile,
    in port p_mipi_clk,
    in port p_mipi_rxa,
    in port p_mipi_rxv,
    buffered in port:32 p_mipi_rxd,
    clock clk_mipi,
    client interface i2c_master_if i2c)
{  
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

  // Initialize camera and its configurations
  sensor_start(i2c);
  printf("\nCamera_started and configured...\n");
  delay_milliseconds(3000);
}
