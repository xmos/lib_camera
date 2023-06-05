
#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include <stdint.h>

#include "xccompat.h"

#include "camera.h"

#ifdef __XC__
extern "C" {
#endif

typedef struct
{
  mipi_header_t header;
  uint8_t payload[MIPI_MAX_PKT_SIZE_BYTES];
} mipi_packet_t;


void mipi_packet_handler(
    streaming_chanend_t c_pkt, 
    streaming_chanend_t c_ctrl,
    streaming_chanend_t c_out_row,
    streaming_chanend_t c_user_api);


#ifdef __XC__
}
#endif

#endif // PACKET_HANDLER_H
