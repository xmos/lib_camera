// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h>
#include <xs3a_defines.h>
#include <xcore/port_protocol.h>
#include <xcore/clock.h>

#include "camera_mipi.h"

// debug options
// (can be enabled via: -DDEBUG_PRINT_ENABLE_CAM_MIPI=1)
#define DEBUG_UNIT CAM_MIPI 
#include <debug_print.h>

static
void debug_mipi_info(
  unsigned mipi_clk_divider,
  unsigned mipi_clk_cfg_divider,
  unsigned mipi_shim_cfg0)
{
  #ifdef DEBUG_PRINT_ENABLE_CAM_MIPI
  // debug MIPI info
  int mipi_phy_freq = 600 / (2 * (mipi_clk_cfg_divider + 1));
  int mipi_shim_freq = 600 / (2 * (mipi_clk_divider + 1));
  debug_printf("mipi_phy_freq = %d MHz\n", mipi_phy_freq);
  debug_printf("mipi_shim_freq = %d MHz\n", mipi_shim_freq);
  debug_printf("mipi_shim_cfg0 = 0x%08X\n", mipi_shim_cfg0);
  assert(mipi_phy_freq < 300 && "MIPI PHY frequency too high");
  assert(mipi_shim_freq < 200 && "MIPI shim frequency too high");
  assert(mipi_phy_freq > mipi_shim_freq && "PHY has to be faster than shim");
  #endif
}

static
void set_pad_delay(port_t p, unsigned int del)
{
  const unsigned mode = (7 << 0);
  const unsigned lmode = (7 << 12);
  const unsigned delay = (del << 3);
  const unsigned ctrl_value = mode | lmode | delay;
  port_write_control_word(p, ctrl_value);
}

void camera_mipi_init(
  camera_mipi_ctx_t* ctx)
{
  // ---------------- MIPI config variables ----------------
  const unsigned mipi_clk_divider = 0x1;
  const unsigned mipi_clk_cfg_divider = 0x2;
  const unsigned mipi_shim_cfg0 = MIPI_SHIM_CFG0_PACK(
    MIPI_SHIM_DEMUX_EN,
    MIPI_SHIM_DEMUX_DATATYPE,
    MIPI_SHIM_DEMUX_MODE,
    MIPI_SHIM_STUFF_ENABLE,
    MIPI_SHIM_BIAS_ENABLE
  );
  debug_mipi_info(
    mipi_clk_divider,
    mipi_clk_cfg_divider,
    mipi_shim_cfg0
  );

  // ---------------- Port configuration ----------------

  // enable ports and clock
  port_enable(ctx->p_mipi_clk);
  port_enable(ctx->p_mipi_rxa);
  port_enable(ctx->p_mipi_rxv);
  port_start_buffered(ctx->p_mipi_rxd, 32);
  clock_enable(ctx->clk_mipi);

  // configure data port clocked input with data valid signal
  port_protocol_in_strobed_slave(ctx->p_mipi_rxd, ctx->p_mipi_rxv, ctx->clk_mipi);

  // configure clk_mipi to follow p_mipi_clk
  clock_set_source_port(ctx->clk_mipi, ctx->p_mipi_clk);

  // Sample on falling edge - shim outputting on rising (only if secondary pll)
  // set_clock_rise_delay(clk_mipi, 0);

  // set pad delay rise edge
  set_pad_delay(ctx->p_mipi_rxa, 1);

  // start mipi clock
  clock_start(ctx->clk_mipi);

  // ---------------- Tile configuration ----------------
  unsigned tileid = get_local_tile_id();
  int ret = 0;

  // set Always-on-power for the DPHY
  ret |= write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_DPHY_CFG0_NUM, 0b10);

  // clock mipi shim (<200 MHz)
  ret |= write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_CLK_DIVIDER_NUM, mipi_clk_divider);

  // clock mipi dphy (<300 MHz)
  ret |= write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_CFG_CLK_DIVIDER_NUM, mipi_clk_cfg_divider);

  // lanes and polarities (see definition)
  ret |= write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_DPHY_CFG3_NUM, DEFAULT_MIPI_DPHY_CFG3);

  // Configure MIPI shim
  ret |= write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, mipi_shim_cfg0);

  // take phy out of reset
  ret |= write_sswitch_reg(tileid, XS1_SSWITCH_MIPI_DPHY_CFG0_NUM, 0b11);

  assert(ret && "Error configuring MIPI D-PHY tile");

  // Connect the xcore-ports to the MIPI demuxer/PHY.
  int val = getps(XS1_PS_XCORE_CTRL0);
  setps(XS1_PS_XCORE_CTRL0, val | 0x100);
}
