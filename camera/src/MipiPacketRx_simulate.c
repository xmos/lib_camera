#include <stdio.h>
#include <assert.h>

#include "packet_handler.h"

#include "MipiPacketRx_simulate.h"

#define STEP_JUMP 1

void MipiPacketRx_simulate(
    in_buffered_port_32_t p_mipi_rxd,
    in_port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl)
{
    while (1) {
        // send data
        for (int j = 0; j < MIPI_IMAGE_HEIGHT_PIXELS + 2; j++) {
            mipi_packet_t* pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
            // if null pointer return
            if (pkt == NULL) {
                return;
            }
            // else continue
            switch (j)
            {
            case 0:
                pkt->header = (uint32_t)MIPI_DT_FRAME_START;
                break;

            case MIPI_IMAGE_HEIGHT_PIXELS + 1:
                pkt->header = (uint32_t)MIPI_DT_FRAME_END;
                break;

            default:
                pkt->header = (uint32_t)MIPI_DT_RAW8; // header type RAW8
                for (int i = 0; i < MIPI_MAX_PKT_SIZE_BYTES; i = i + STEP_JUMP) {
                    int8_t pixel_value = (i % 256) - 128;
                    pkt->payload[i] = pixel_value;
                }
                break;
            }
            // send back the data
            s_chan_out_word(c_pkt, (unsigned)pkt);
            delay_milliseconds(2); // LB
        }
        delay_milliseconds(10); // FB
    }
}
