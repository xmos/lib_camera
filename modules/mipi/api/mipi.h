
#pragma once

#include <stdint.h>
#include <stddef.h>

#include "mipi_defines.h"

#ifndef __XC__
#include <xcore/port.h>
#include <xcore/channel_streaming.h>
#include <xcore/clock.h>
#endif

#ifdef __XC__

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
    uint32_t cfgClkDiv);

void MipiPacketRx(
    buffered in port:32 p_mipi_rxd,
    in port p_mipi_rxa,
    streaming chanend c_pkt,
    streaming chanend c_ctrl);

#else

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
    uint32_t cfgClkDiv);

void MipiPacketRx(
    port_t p_mipi_rxd,
    port_t p_mipi_rxa,
    chanend_t c_pkt,
    chanend_t c_ctrl);
    
#endif
