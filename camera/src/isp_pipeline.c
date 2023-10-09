// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include <stdint.h>
#include <stdio.h>

#include <xcore/assert.h>
#include <xcore/channel.h> // includes streaming channel and channend
#include <xccompat.h>

#include "sensor_control.h"
#include "print.h"

#include "isp_pipeline.h"

// ISP settings
isp_params_t isp_params = {
  .channel_gain = {
  AWB_gain_RED,
  AWB_gain_GREEN,
  AWB_gain_BLUE
  }
};

vfilter_acc_t vfilter_accs[APP_IMAGE_CHANNEL_COUNT][VFILTER_ACC_COUNT];
hfilter_state_t hfilter_state[APP_IMAGE_CHANNEL_COUNT];


isp_cmd_t isp_recieve_cmd(chanend ch){
    isp_cmd_t cmd = (isp_cmd_t)chanend_in_word(ch);
    chanend_out_word(ch, RESP_OK);
    return cmd;
}
unsigned isp_send_cmd(chanend ch, isp_cmd_t cmd){
    chanend_out_word(ch, (uint32_t)cmd);
    return (unsigned)chanend_in_word(ch);
}


void recieve_isp_row_ptr(chanend ch, int8_t *pix_data){
    pix_data = (int8_t *)chanend_in_word(ch);
    chanend_out_word(ch, RESP_OK);
}
unsigned send_isp_row_ptr(chanend ch, int8_t *pix_data){
    chanend_out_word(ch, (unsigned)pix_data);
    return (unsigned)chanend_in_word(ch);
}

void isp_send_row_info(
    chanend ch,
    row_info_t *info)
{
    chan_out_buf_byte(ch, (uint8_t*)info, sizeof(row_info_t));
}

row_info_t isp_recieve_row_info(chanend ch)
{
    row_info_t info; 
    chan_in_buf_byte(ch, (uint8_t*)&info, sizeof(row_info_t));
    return info;
}


// ------------- Core functions -----------------------
void filter_update()
{
    for (int c = 0; c < APP_IMAGE_CHANNEL_COUNT; c++) {
        pixel_hfilter_update_scale(
            &hfilter_state[c],
            isp_params.channel_gain[c],
            (c == 0) ? 0 : 1);

        image_vfilter_frame_init(&vfilter_accs[c][0]);
    }
}

void process_row(chanend c_isp){
    row_info_t info = isp_recieve_row_info(c_isp);

    //printf("rx_pix[0]=%d\n", (int8_t)info.row_ptr[0]);
    //printf("row = %d\n", info.state_ptr->in_line_number);
}

void isp_thread(chanend c_isp, chanend c_control){
    while(1){
        isp_cmd_t cmd = isp_recieve_cmd(c_isp);
        switch(cmd){
            case FILTER_DRAIN:
                break;
            case FILTER_UPDATE:
                filter_update();
                break;
            case PROCESS_ROW:
                process_row(c_isp);
                break;
            default:
                printf("404 in ISP\n");
                break;
        }
    }
}
