// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "xccompat.h"

#include "camera_main.h"
#include "isp_pipeline.h"

// Represents a received MIPI packet.
typedef struct
{
  mipi_header_t header;
  uint8_t payload[MIPI_MAX_PKT_SIZE_BYTES];
} mipi_packet_t;

/**
 * @brief Handles a MIPI packet. Receives MIPI packets from the
 * packet receiver and passes them to `handle_packet()` for parsing and
 * processing.
 * @param c_pkt   Streaming channel to receive MIPI packets from. 
 * @param c_ctrl  Streaming channel to send control messages to.
 */
void mipi_packet_handler(
    streaming_chanend_t c_pkt, 
    streaming_chanend_t c_ctrl,
    chanend c_isp);
    
