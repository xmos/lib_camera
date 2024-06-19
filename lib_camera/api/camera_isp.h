// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"
#include "xcore_compat.h"

C_API_START


/**
 * @brief Handles a MIPI packet. Receives MIPI packets from the
 * packet receiver and passes them to `camera_isp_packet_handler()` for parsing and
 * processing.
 * @param c_pkt   Streaming channel to receive MIPI packets from.
 * @param c_ctrl  Streaming channel to send control messages to.
 * @param c_isp   Channel to send ISP commands to.
 */
void camera_isp_thread(
  streaming_chanend_t c_pkt,
  streaming_chanend_t c_ctrl,
  chanend_t c_isp,
  chanend_t c_user);


C_API_END
