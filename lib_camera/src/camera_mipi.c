// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xs3a_defines.h>
#include <xcore/port_protocol.h>
#include <xcore/clock.h>

#include "camera_mipi.h"
#include "camera_mipi_defines.h"

static
void camera_mipi_ctx_init(
  camera_mipi_ctx_t* ctx) {
  port_enable(ctx->p_mipi_clk);
  port_enable(ctx->p_mipi_rxa);
  port_enable(ctx->p_mipi_rxv);
  port_start_buffered(ctx->p_mipi_rxd, 32);
  clock_enable(ctx->clk_mipi);
}

static
void camera_mipi_packet_init(
  unsigned tile,
  port_t p_mipi_rxd,
  port_t p_mipi_rxv,
  port_t p_mipi_rxa,
  port_t p_mipi_clk,
  xclock_t clk_mipi,
  unsigned mipi_shim_cfg0,
  uint32_t mipiClkDiv,
  uint32_t cfgClkDiv) {
  // This API has been translated from XC
  // original code is kept uncommented

  //configure_in_port_strobed_slave(p_mipi_rxd, p_mipi_rxv, clk_mipi);
  port_protocol_in_strobed_slave(p_mipi_rxd, p_mipi_rxv, clk_mipi);

  //set_clock_src(clk_mipi, p_mipi_clk);
  clock_set_source_port(clk_mipi, p_mipi_clk);

  // Sample on falling edge - shim outputting on rising 
  //set_clock_rise_delay(clk_mipi, 1);
  // 0x9007 is a RISE_DELAY config, see XU316 documentation
  __xcore_resource_setc(clk_mipi, XS1_SETC_VALUE_SET(0x9007, 1));

  //set_pad_delay(p_mipi_rxa, 1);
  // 0x7007 is a PAD_DELAY config, see XU316 documentation
  __xcore_resource_setc(p_mipi_rxa, XS1_SETC_VALUE_SET(0x7007, 1));

  //start_clock(clk_mipi);
  clock_start(clk_mipi);

  // take DPHY out of reset

  //write_node_config_reg(tile, XS1_SSWITCH_MIPI_DPHY_CFG0_NUM, 3);
  write_sswitch_reg(tile, XS1_SSWITCH_MIPI_DPHY_CFG0_NUM, 3);

  // set clock dividers to create suitable frequency 
  //write_node_config_reg(tile, XS1_SSWITCH_MIPI_CLK_DIVIDER_NUM, mipiClkDiv);
  write_sswitch_reg(tile, XS1_SSWITCH_MIPI_CLK_DIVIDER_NUM, mipiClkDiv);

  //write_node_config_reg(tile, XS1_SSWITCH_MIPI_CFG_CLK_DIVIDER_NUM, cfgClkDiv);
  write_sswitch_reg(tile, XS1_SSWITCH_MIPI_CFG_CLK_DIVIDER_NUM, cfgClkDiv);

  // set shim config register
  //write_node_config_reg(tile, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, mipi_shim_cfg0);
  write_sswitch_reg(tile, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, mipi_shim_cfg0);

  // Connect the xcore-ports to the MIPI demuxer/PHY.
  int val = getps(XS1_PS_XCORE_CTRL0);
  setps(XS1_PS_XCORE_CTRL0, val | 0x100);
}


void camera_mipi_init(
  camera_mipi_ctx_t* ctx
) {
  // Start context
  camera_mipi_ctx_init(ctx);

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
  camera_mipi_packet_init(tileid,
    ctx->p_mipi_rxd,
    ctx->p_mipi_rxv,
    ctx->p_mipi_rxa,
    ctx->p_mipi_clk,
    ctx->clk_mipi,
    mipi_shim_cfg0,
    MIPI_CLK_DIV,
    MIPI_CFG_CLK_DIV);
}
