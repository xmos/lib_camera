// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include <stdint.h>
#include <stdio.h>

#include <xcore/assert.h>
#include <xcore/channel.h> // includes streaming channel and channend
#include <xccompat.h>

#include "camera_api.h"
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


__attribute__((aligned(8)))
int8_t output_buff[2][APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];
uint8_t out_dex = 0;

// ------------- PH <> ISP communication -----------------------

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
    row_info_t *info){
    chan_out_buf_byte(ch, (uint8_t*)info, sizeof(row_info_t));
}

row_info_t isp_recieve_row_info(chanend ch){
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

static 
void on_new_output_row(
    const int8_t pix_out[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS],
    row_info_t* info)
{
  camera_new_row_decimated(pix_out, info->state_ptr->out_line_number);
  info->state_ptr->out_line_number++;
}

void process_row(chanend c_isp){
    int8_t hfilt_row[APP_IMAGE_WIDTH_PIXELS];

    // recieve the row pointers
    row_info_t info = isp_recieve_row_info(c_isp);

    // Print aux info
    //printf("rx_pix[0]=%d\n", (int8_t)info.row_ptr[0]);
    //printf("R=%d\n", info.state_ptr->in_line_number);

    // Obtain pattern
    unsigned pattern = info.state_ptr->in_line_number % 2;
    
    if(pattern == 0){
        // RED
        pixel_hfilter(
            &hfilt_row[0],
            (int8_t*) info.row_ptr,
            &hfilter_state[CHAN_RED].coef[0],
            hfilter_state[CHAN_RED].acc_init,
            hfilter_state[CHAN_RED].shift,
            HFILTER_INPUT_STRIDE,
            APP_IMAGE_WIDTH_PIXELS);

        image_vfilter_process_row(
            &output_buff[out_dex][CHAN_RED][0],
            &vfilter_accs[CHAN_RED][0],
            &hfilt_row[0]);

        // GREEN
         pixel_hfilter(
            &hfilt_row[0],
            (int8_t*) info.row_ptr,
            &hfilter_state[CHAN_GREEN].coef[0],
            hfilter_state[CHAN_GREEN].acc_init,
            hfilter_state[CHAN_GREEN].shift,
            HFILTER_INPUT_STRIDE,
            APP_IMAGE_WIDTH_PIXELS);

        image_vfilter_process_row(
            &output_buff[out_dex][CHAN_GREEN][0],
            &vfilter_accs[CHAN_GREEN][0],
            &hfilt_row[0]);

    } else{ // GB_PATTERN

        // BLUE
        pixel_hfilter(
            &hfilt_row[0],
            (int8_t*) info.row_ptr,
            &hfilter_state[CHAN_BLUE].coef[0],
            hfilter_state[CHAN_BLUE].acc_init,
            hfilter_state[CHAN_BLUE].shift,
            HFILTER_INPUT_STRIDE,
            APP_IMAGE_WIDTH_PIXELS);

        unsigned new_row = image_vfilter_process_row(
            &output_buff[out_dex][CHAN_BLUE][0],
            &vfilter_accs[CHAN_BLUE][0],
            &hfilt_row[0]);

        if (new_row) {
            on_new_output_row(output_buff[out_dex], &info);
            out_dex ^= 1; //TODO this should be done by PH
        }
        info.state_ptr->out_line_number++; //TODO this should be done in handler
    }
}

void filter_drain(chanend c_isp)
{
    row_info_t info = isp_recieve_row_info(c_isp);
    image_vfilter_drain(&output_buff[out_dex][CHAN_RED][0], &vfilter_accs[CHAN_RED][0]);
    image_vfilter_drain(&output_buff[out_dex][CHAN_GREEN][0], &vfilter_accs[CHAN_GREEN][0]);
    unsigned r = image_vfilter_drain(&output_buff[out_dex][CHAN_BLUE][0], &vfilter_accs[CHAN_BLUE][0]);
    if (r){ //TODO there is never an else
        on_new_output_row(output_buff[out_dex], &info);
    }
}

void isp_thread(chanend c_isp, chanend c_control){
    while(1){
        isp_cmd_t cmd = isp_recieve_cmd(c_isp);
        switch(cmd){
            case FILTER_DRAIN:
                filter_drain(c_isp);
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
