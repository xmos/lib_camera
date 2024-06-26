// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include <xs1.h>

#include "xcore_compat.h"
#include "mipi.h"

#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif

#define MIPI_TILE                 1

#include "isp_image_hfilter.h"
#include "isp_image_vfilter.h"
#include "packet_handler.h"
#include "camera_api.h"
#include "camera_utils.h"

// MIPI Shim configuration register layout (MIPI_SHIM_CFG0) 
#define MIPI_SHIM_BIAS_ENABLE       1       //  Offset output pixels [1]
#define MIPI_SHIM_STUFF_ENABLE      0       // Add a zero byte after every RGB pixel [2]
#define MIPI_SHIM_DEMUX_MODE        0       // demux mode (see xMIPI_DemuxMode_t), Unused if MIPI_SHIM_DEMUX_EN = 0 
#define MIPI_SHIM_DEMUX_DATATYPE    0       // CSI-2 packet type to demux, Unused if MIPI_SHIM_DEMUX_EN = 0
#define MIPI_SHIM_DEMUX_EN          0       // MIPI shim 0 = disabled, 1 = enabled

// Mipi shim clock settings
#define MIPI_CLK_DIV      1     // CLK DIVIDER
#define MIPI_CFG_CLK_DIV  3     // CFG DIVIDER

/**
 * Thread entry point for interfacing with the camera sensor.
 * 
 * This function will configure mipi and initialize the camera sensor
 * 
 * @param p_mipi_clk  The MIPI clock input port
 * @param p_mipi_rxa  The MIPI active input port
 * @param p_mipi_rxv  The MIPI valid input port
 * @param p_mipi_rxd  The MIPI data input port
 * @param clk_mipi    The MIPI clock block
 */
void camera_mipi_init(
    port_t p_mipi_clk,
    port_t p_mipi_rxa,
    port_t p_mipi_rxv,
    in_buffered_port_32_t p_mipi_rxd,
    xclock_t clk_mipi);

/* Notes

[1]
Indicates whether the MIPI shim should apply a bias to received pixel data.
0 = disabled, 1 = enabled  (does not require MIPI_SHIM_DEMUX_EN = 1)
The bias subtracts 128 from each received pixel value, converting the
pixel data from uint8 to int8.
TODO: astew: This is currently ignored -- the bias is always enabled in the
             downsample app, and disabled in the raw capture app.

[2]
Indicates whether the MIPI shim should stuff an extra byte into each triplet
of received pixel data, in order to achieve 4-byte alignment of pixel
values.
0 = disabled, 1 = enabled  (does not require MIPI_SHIM_DEMUX_EN = 1)

*/
