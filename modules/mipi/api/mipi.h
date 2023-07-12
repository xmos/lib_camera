
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

/**
 * @brief Initialize the MIPI packet handler.
 * 
 * @param tile          Tile where the MIPI packet handler is running
 * @param p_mipi_rxd    High-Speed Receive Data
 * @param p_mipi_rxv    High-Speed Receive Data Valid. This active high signal indicates that the lane module is driving valid data to the protocol on the RxDataHS output.
 * @param p_mipi_rxa    RxActiveHS (Output): High-Speed Reception Active. This active high signal indicates that the lane module is actively receiving a high-speed transmission from the lane interconnect.
 * @param p_mipi_clk    High-Speed Receive Byte Clock. This signal is used to synchronize signals in the high-speed receive clock domain.
 * @param clk_mipi      Clock block used to generate the MIPI clock
 * @param mipi_shim_cfg0 Configuration of the mipi shim
 * @param mipiClkDiv    MIPI clock divider
 * @param cfgClkDiv     Configuration clock divider
 */
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

/**
 * @brief Mipi packet reciever
 * 
 * @param p_mipi_rxd    High-Speed Receive Data
 * @param p_mipi_rxa    RxActiveHS (Output): High-Speed Reception Active. This active high signal indicates that the lane module is actively receiving a high-speed transmission from the lane interconnect.
 * @param c_pkt         Channel to send packets to
 * @param c_ctrl        Channel to send control messages to
 */
void MipiPacketRx(
    in_buffered_port_32_t p_mipi_rxd,
    in_port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl);


