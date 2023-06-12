

#include <xs1.h>
#include <platform.h>

#include <stdint.h>
#include <stddef.h>

#include "mipi.h"

void MipiPacketRx_init(
    tileref_t tile,
    in_buffered_port_32_t p_mipi_rxd, 
    in_port_t p_mipi_rxv,
    in_port_t p_mipi_rxa, 
    in_port_t p_mipi_clk,
    xclock_t clk_mipi,
    unsigned mipi_shim_cfg0,
    uint32_t mipiClkDiv,
    uint32_t cfgClkDiv)
{
  configure_in_port_strobed_slave(p_mipi_rxd, p_mipi_rxv, clk_mipi);
  set_clock_src(clk_mipi, p_mipi_clk);

  /* Sample on falling edge - shim outputting on rising */
  set_clock_rise_delay(clk_mipi, 1);
  set_pad_delay(p_mipi_rxa, 1);
  start_clock(clk_mipi);
  // take DPHY out of reset
  write_node_config_reg(tile, 
      XS1_SSWITCH_MIPI_DPHY_CFG0_NUM, 3);
  // set clock dividers to create suitable frequency 
  write_node_config_reg(tile, 
      XS1_SSWITCH_MIPI_CLK_DIVIDER_NUM, mipiClkDiv);
  write_node_config_reg(tile, 
      XS1_SSWITCH_MIPI_CFG_CLK_DIVIDER_NUM, cfgClkDiv);

  // set shim config register
  write_node_config_reg(tile, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, mipi_shim_cfg0);

  // Connect the xcore-ports to the MIPI demuxer/PHY.
  int val = getps(XS1_PS_XCORE_CTRL0);
  setps(XS1_PS_XCORE_CTRL0, val | 0x100);
}
