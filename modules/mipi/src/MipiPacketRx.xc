

#include <xs1.h>
#include <platform.h>

#include <stdint.h>
#include <stddef.h>

#include "mipi.h"

void MipiPacketRx_init(
    tileref tile,
    buffered in port:32 p_mipi_rxd, 
    in port p_mipi_rxv,
    in port p_mipi_rxa, 
    in port p_mipi_clk,
    clock clk_mipi,
    uint32_t demuxEn, 
    uint32_t dataType,
    xMIPI_DemuxMode_t demuxMode, 
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
  /* Enable pixel demux */ 
  unsigned shimCfg = (((demuxMode & 0xff) << 16) | ((dataType & 0xff) << 8) | demuxEn);
  write_node_config_reg(tile, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, shimCfg);

  // Connect the xcore-ports to the MIPI demuxer/PHY.
  int val = getps(XS1_PS_XCORE_CTRL0);
  setps(XS1_PS_XCORE_CTRL0, val | 0x100);
}
