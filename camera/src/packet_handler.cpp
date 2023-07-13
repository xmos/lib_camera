#include <stdio.h>
#include <assert.h>

#include <xcore/channel_streaming.h>
#include <xcore/select.h>

#include "packet_handler.h"
#include "image_vfilter.h"
#include "image_hfilter.h"
#include "camera_api.h"
#include "camera_utils.h"
#include "sensor.h"

#include "packet_handler.h"
#include "cpp/packet_handler.hpp"

// Filter stride
#define HFILTER_INPUT_STRIDE  (APP_DECIMATION_FACTOR)

/**
 * Top level of the packet handling thread. Receives MIPI packets from the
 * packet receiver and passes them to `handle_packet()` for parsing and
 * processing.
 */
void mipi_packet_handler(
    streaming_chanend_t c_pkt, 
    streaming_chanend_t c_ctrl,
    streaming_chanend_t c_stats)
{
  PacketHandler<640, 4> packetHandler;

  packetHandler.thread_entry(c_pkt, c_ctrl, c_stats);
}
