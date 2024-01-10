// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h> // for ports

#include "camera_main.h"

#include "mipi.h"
#include "packet_handler.h"


void camera_mipi_init(
    port_t p_mipi_clk,
    port_t p_mipi_rxa,
    port_t p_mipi_rxv,
    in_buffered_port_32_t p_mipi_rxd,
    xclock_t clk_mipi)
{
  // Tile ids have weird values, so we get them with this API
  unsigned tileid = get_local_tile_id();
  // Assign lanes and polarities
  //write_node_config_reg(mipi_tile, XS1_SSWITCH_MIPI_DPHY_CFG3_NUM, DEFAULT_MIPI_DPHY_CFG3);
  write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_DPHY_CFG3_NUM, DEFAULT_MIPI_DPHY_CFG3);

  // Configure MIPI shim
  unsigned mipi_shim_cfg0 = MIPI_SHIM_CFG0_PACK(MIPI_SHIM_DEMUX_EN,
                                                MIPI_SHIM_DEMUX_DATATYPE,  
                                                MIPI_SHIM_DEMUX_MODE, 
                                                MIPI_SHIM_STUFF_ENABLE,
                                                1); // enable bias
  // Initialize MIPI receiver
  MipiPacketRx_init(tileid,
                    p_mipi_rxd,
                    p_mipi_rxv,
                    p_mipi_rxa,
                    p_mipi_clk,
                    clk_mipi,
                    mipi_shim_cfg0,
                    MIPI_CLK_DIV,
                    MIPI_CFG_CLK_DIV);
}
