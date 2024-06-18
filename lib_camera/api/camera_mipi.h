// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <xcore/port.h>
#include <xccompat.h>

#include "xcore_compat.h"

// MIPI Shim configuration register layout (MIPI_SHIM_CFG0) 
#define MIPI_SHIM_BIAS_ENABLE       1       //  Offset output pixels [1]
#define MIPI_SHIM_STUFF_ENABLE      0       // Add a zero byte after every RGB pixel [2]
#define MIPI_SHIM_DEMUX_MODE        0       // demux mode (see xMIPI_DemuxMode_t), Unused if MIPI_SHIM_DEMUX_EN = 0 
#define MIPI_SHIM_DEMUX_DATATYPE    0       // CSI-2 packet type to demux, Unused if MIPI_SHIM_DEMUX_EN = 0
#define MIPI_SHIM_DEMUX_EN          0       // MIPI shim 0 = disabled, 1 = enabled

// Mipi shim clock settings
#define MIPI_CLK_DIV      1     // CLK DIVIDER
#define MIPI_CFG_CLK_DIV  3     // CFG DIVIDER
#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif

typedef struct
{
    port_t p_mipi_clk;
    port_t p_mipi_rxa;
    port_t p_mipi_rxv;
    in_buffered_port_32_t p_mipi_rxd;
    xclock_t clk_mipi;
} camera_mipi_ctx_t;

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
