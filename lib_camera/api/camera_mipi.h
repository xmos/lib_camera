// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <xcore/port.h>
#include <xccompat.h>

#include "xcore_compat.h"
#include "camera_mipi_defines.h"

/**
 * @brief Initialize the MIPI context.
 *
 * @param ctx  The MIPI context to initialize
 */
void mipi_ctx_init(camera_mipi_ctx_t* ctx);

/**
 * @brief Initialize the MIPI camera.
 *
 * @param ctx  The MIPI context to initialize
 */
void mipi_camera_init(camera_mipi_ctx_t* ctx);

/**
 * @brief Mipi packet reciever
 *
 * @param p_mipi_rxd    High-Speed Receive Data
 * @param p_mipi_rxa    RxActiveHS (Output): High-Speed Reception Active. This active high signal indicates that the lane module is actively receiving a high-speed transmission from the lane interconnect.
 * @param c_pkt         Channel to send packets to
 * @param c_ctrl        Channel to send control messages to
 * @note: This function is implemented in assembly
 */
void mipi_packet_rx(
    in_buffered_port_32_t p_mipi_rxd,
    port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl);
