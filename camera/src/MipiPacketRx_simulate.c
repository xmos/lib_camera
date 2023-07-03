#include <stdio.h>
#include <assert.h>

#include "MipiPacketRx_simulate.h"

#include "packet_handler.h"
#include "io_utils.h"
#include <string.h>

#define STEP_JUMP 1

//function to fill the array
/*
static
void fill_array(mipi_packet_t* pkt) {
    int8_t pixel_value = 0;
    for (int i = 0; i < MIPI_MAX_PKT_SIZE_BYTES; i = i + STEP_JUMP) {
        if (i % 2) {
            pixel_value = (i % 256) - 128;
        }
        else {
            pixel_value = -128;
        }
        pkt->payload[i] = pixel_value;
    }
}
*/

static
void fill_array_from_file(xscope_file_t *fp, mipi_packet_t* pkt) {
    xscope_fread(
        fp, 
        (uint8_t*) &pkt->payload[0], 
        MIPI_IMAGE_WIDTH_BYTES);
    //printf("num_bytes: %d\n", num_bytes);
}

void MipiPacketRx_simulate(
    in_buffered_port_32_t p_mipi_rxd,
    in_port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl)
{
    const char * filename = "test_generated_int8.raw";
    xscope_file_t fp = xscope_open_file(filename, "rb");
 
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
                // fill_array(pkt);
                fill_array_from_file(&fp, pkt);
                break;
            }
            // send back the data
            s_chan_out_word(c_pkt, (unsigned)pkt);
            delay_milliseconds(2); // LB
        }
        xscope_fseek(&fp, 0, SEEK_SET);
        delay_milliseconds(10); // FB
    }
}
