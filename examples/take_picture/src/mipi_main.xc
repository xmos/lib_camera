#include <xs1.h>
#include <print.h>
#include <stdio.h>
#include <platform.h> // for ports
#include <assert.h> 
#include <stdlib.h> // exit status

// I2C
#include "i2c.h"

// MIPI
#include "mipi.h"
#include "mipi_main.h"
#include "extmem_data.h"
#include "MipiPacket.h"
#include "imx219.h"

// Sensor
#define MSG_SUCCESS "Stream start OK\n"
#define MSG_FAIL "Stream start Failed\n"

/*
#include <math.h>
#include <string.h>

#include <stdint.h>
*/

/*
#include "gc2145.h"
#include "debayer.h"
#include "yuv_to_rgb.h"
*/

/* Declaration of the MIPI interface ports:
 * Clock, receiver active, receiver data valid, and receiver data
 */
on tile[MIPI_TILE]:         in port    p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE]:         in port    p_mipi_rxa = XS1_PORT_1E; // activate
on tile[MIPI_TILE]:         in port    p_mipi_rxv = XS1_PORT_1I; // valid
on tile[MIPI_TILE]:buffered in port:32 p_mipi_rxd = XS1_PORT_8A; // data

on tile[MIPI_TILE]:clock               clk_mipi   = MIPI_CLKBLK;


/**
 * The packet buffer is where the packet decoupler will tell the MIPI receiver
 * thread to store received packets.
 */
static mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];
#define DEMUX_DATATYPE    0 // RESERVED
#define DEMUX_MODE        0x00     // no demux
#define DEMUX_EN          0 
#define MIPI_CLK_DIV      1
#define MIPI_CFG_CLK_DIV  3



#pragma unsafe arrays
static void mipi_packet_handler(
    streaming chanend c_pkt, 
    streaming chanend c_ctrl)
{
    mipi_header_t mipiHeader;
    unsigned pkt_idx = 0;
    unsigned in_frame = 0;
    unsigned line_count = 0;
    unsigned byte_count = 0;

    timer tmr;
    unsigned start_time = 0;
    unsigned end_time = 0;

    unsafe {
      while(1) {
        
        // Advance buffer pointer to next empty buffer
        pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT-1);

        // Send pointer to a free buffer to the MIPI receiver thread 
        mipi_packet_t * unsafe pkt = &packet_buffer[pkt_idx];

        outuint((chanend) c_pkt, (unsigned) pkt);

        do { // We only need to switch to a new buffer on a long packet
          
          // MIPI receiver thread will send the packet header when packet has 
          // been fully received.
          mipiHeader = inuint((chanend) c_pkt);

          assert(mipiHeader == pkt->header);

          // If it's a long packet, break the loop
          if(MIPI_IS_LONG_PACKET(mipiHeader))
            break;

          // Otherwise, handle the short packet
          switch(MIPI_GET_DATA_TYPE(mipiHeader)){
            case MIPI_DT_FRAME_START:
                tmr :> start_time;
                in_frame = 1;
                break;

            case MIPI_DT_FRAME_END:
                tmr :> end_time;

            unsigned ticks = end_time - start_time;

            printf("Received EOF\n");
            printf("Total lines received: %u\n", line_count);
            printf("Total bytes received: %u\n", byte_count);
            printf("Tick count: %u\n", ticks);
            exit(0);
            break;
          }

        } while(1);

        // Handle the long packet
        line_count++;
        byte_count += MIPI_GET_WORD_COUNT(mipiHeader);
      }
    }
}




void mipi_main(client interface i2c_master_if i2c)
{
    printf("< Start of MIPI >\n");
    streaming chan c_pkt;
    streaming chan c_ctrl;
    
    // See AN for MPIP shim
    // 0x7E42 >> 0111 1110 0100 0010
    // in the explorer BOARD DPDN is swap 
    write_node_config_reg(tile[MIPI_TILE], 
                          XS1_SSWITCH_MIPI_DPHY_CFG3_NUM , 
                          0x7E42);
    

    // send packet to MIPI shim
    MipiPacketRx_init(tile[MIPI_TILE],
                    p_mipi_rxd, 
                    p_mipi_rxv, 
                    p_mipi_rxa, 
                    p_mipi_clk, 
                    clk_mipi,
                    DEMUX_EN, 
                    DEMUX_DATATYPE, 
                    DEMUX_MODE,
                    MIPI_CLK_DIV, 
                    MIPI_CFG_CLK_DIV);
    
    // Now start the camera
    int r = imx219_stream_start(i2c);
    if (r!=0) {
      printf(MSG_FAIL);
    }
    else {
      printf(MSG_SUCCESS);
    }

    par {
        MipiPacketRx2(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
        mipi_packet_handler(c_pkt, c_ctrl);
    }


    // return 
    printf("Return code = %d\n", r);
    printf("< End of MIPI >\n");
}
