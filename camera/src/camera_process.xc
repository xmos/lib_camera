

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "camera_process.h"

void camera_process(chanend c_control) 
{
  streaming chan c_stat_thread;
  streaming chan c_pkt;
  streaming chan c_ctrl;

  camera_mipi_init(
    p_mipi_clk,
    p_mipi_rxa,
    p_mipi_rxv,
    p_mipi_rxd,
    clk_mipi);
  
  par{
    MipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, c_stat_thread);
    isp_pipeline(c_stat_thread, c_control);
    }
}
