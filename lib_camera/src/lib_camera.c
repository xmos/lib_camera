// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xcore/channel.h>
#include <xcore/port.h>
#include <xcore/parallel.h>
#include <xccompat.h>

#include "camera_mipi.h"
#include "camera_mipi_rx.h"
#include "camera_isp.h"
#include "sensor_control.h"

#include "lib_camera.h"

DECLARE_JOB(camera_mipi_rx, (in_buffered_port_32_t, port_t, streaming_chanend_t, streaming_chanend_t));
DECLARE_JOB(camera_isp_thread, (streaming_chanend_t, streaming_chanend_t, chanend_t, chanend_t));
DECLARE_JOB(camera_sensor_control, (chanend_t));



void lib_camera_main(chanend_t c_user){

    // Channels
    streaming_channel_t c_pkt = s_chan_alloc();
    streaming_channel_t c_ctrl = s_chan_alloc();
    channel_t c_control = chan_alloc();

    // Camera Init
    camera_mipi_ctx_t ctx = {
        .p_mipi_clk = XS1_PORT_1O,
        .p_mipi_rxa = XS1_PORT_1E,
        .p_mipi_rxv = XS1_PORT_1I,
        .p_mipi_rxd = XS1_PORT_8A,
        .clk_mipi = MIPI_CLKBLK
    };
    camera_mipi_init(&ctx);

    // Parallel Jobs
    PAR_JOBS(
        PJOB(camera_mipi_rx, (ctx.p_mipi_rxd, ctx.p_mipi_rxa, c_pkt.end_a, c_ctrl.end_a)),
        PJOB(camera_isp_thread,(c_pkt.end_b, c_ctrl.end_b, c_control.end_a, c_user)),
        PJOB(camera_sensor_control,(c_control.end_b))
    );

    s_chan_free(c_pkt);
    s_chan_free(c_ctrl);
    chan_free(c_control);
}
