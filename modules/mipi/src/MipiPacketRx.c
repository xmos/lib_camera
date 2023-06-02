

#include <xcore/port_protocol.h>
#include <xcore/clock.h>
#include <xs1.h>

#include "mipi.h"

#define SHIFT 0x3
#define DELAY_SIZE 0x9
#define DELAY_MASK (((1 << DELAY_SIZE) - 1) << SHIFT)
#define DELAY(x) (((x) & DELAY_MASK) >> SHIFT)
#define DELAY_SET(x, v) (((x) & ~DELAY_MASK) | (((v) << SHIFT) & DELAY_MASK))

void MipiPacketRx_init(
    unsigned tile,
    port_t p_mipi_rxd, 
    port_t p_mipi_rxv,
    port_t p_mipi_rxa, 
    port_t p_mipi_clk,
    xclock_t clk_mipi,
    uint32_t demuxEn, 
    uint32_t dataType,
    xMIPI_DemuxMode_t demuxMode, 
    uint32_t mipiClkDiv,
    uint32_t cfgClkDiv)
{
    //configure_in_port_strobed_slave(p_mipi_rxd, p_mipi_rxv, clk_mipi);
    port_protocol_in_strobed_slave(p_mipi_rxd, p_mipi_rxv, clk_mipi);
    
    //set_clock_src(clk_mipi, p_mipi_clk);
    clock_set_ready_src(clk_mipi, p_mipi_clk);

    /* Sample on falling edge - shim outputting on rising */
    //set_clock_rise_delay(clk_mipi, 1);
    __xcore_resource_setc(clk_mipi, DELAY_SET(0x9007, 1));
    
    //set_pad_delay(p_mipi_rxa, 1);
    __xcore_resource_setc(p_mipi_rxa, DELAY_SET(0x7007, 1));
    
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

    /* Enable pixel demux */ 
    unsigned shimCfg = (((demuxMode & 0xff) << 16) | ((dataType & 0xff) << 8) | demuxEn);
    //write_node_config_reg(tile, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, shimCfg);
    write_sswitch_reg(tile, XS1_SSWITCH_MIPI_SHIM_CFG0_NUM, shimCfg);

    // Connect the xcore-ports to the MIPI demuxer/PHY.
    int val = getps(XS1_PS_XCORE_CTRL0);
    setps(XS1_PS_XCORE_CTRL0, val | 0x100);
}
