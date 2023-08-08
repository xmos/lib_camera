// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

/**
 * @brief Mipi packet reciever that takes an image from a file and injects to the board
 * 
 * @param p_mipi_rxd    High-Speed Receive Data
 * @param p_mipi_rxa    RxActiveHS (Output): High-Speed Reception Active. This active high signal indicates that the lane module is actively receiving a high-speed transmission from the lane interconnect.
 * @param c_pkt         Channel to send packets to
 * @param c_ctrl        Channel to send control messages to
 */
void MipiPacketRx_simulate(
    in_buffered_port_32_t p_mipi_rxd,
    port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl);
