// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mipi.h"
#include "packet_handler.h"
#include "io_utils.h"

#include "packet_rx_simulate.h"


void MipiPacketRx_simulate(
    in_buffered_port_32_t p_mipi_rxd,
    port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl)
{
    const char* filename = "tmp.raw";
    io_open_file(filename);

    while (1) {
        // send data
        for (uint16_t row = 0; row < MIPI_IMAGE_HEIGHT_PIXELS + 2; row++) {
            mipi_packet_t* pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
            // if null pointer return
            if (pkt == NULL) {
                return;
            }
            // else continue
            switch (row)
            {
            case 0:
                pkt->header = (uint32_t)MIPI_DT_FRAME_START;
                break;

            case MIPI_IMAGE_HEIGHT_PIXELS + 1:
                pkt->header = (uint32_t)MIPI_DT_FRAME_END;
                break;

            default:
                pkt->header = (uint32_t)MIPI_DT_RAW8; // header type RAW8
                io_fill_array_from_file((uint8_t*)&pkt->payload[0], MIPI_IMAGE_WIDTH_BYTES);
                break;
            }
            // send back the data
            s_chan_out_word(c_pkt, (unsigned)pkt);
            delay_milliseconds(2); // LB
        }
        io_rewind_file();
        delay_milliseconds(10); // FB
    }
}
