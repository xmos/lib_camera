
#pragma once

#include <stdint.h>
#include <stddef.h>

#include "xccompat.h"

#include "mipi_defines.h"

#ifdef __XC__
typedef tileref tileref_t;
typedef clock xclock_t;
typedef in port in_port_t;
#else
#include <xcore/port.h>
#include <xcore/channel_streaming.h>
#include <xcore/clock.h>
typedef unsigned tileref_t;
typedef port_t in_buffered_port_32_t;
typedef port_t in_port_t;
#endif

void MipiPacketRx_init(
    tileref_t tile,
    in_buffered_port_32_t p_mipi_rxd, 
    in_port_t p_mipi_rxv,
    in_port_t p_mipi_rxa, 
    in_port_t p_mipi_clk,
    xclock_t clk_mipi,
    unsigned mipi_shim_cfg0,
    uint32_t mipiClkDiv,
    uint32_t cfgClkDiv);

void MipiPacketRx(
    in_buffered_port_32_t p_mipi_rxd,
    in_port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl);
    