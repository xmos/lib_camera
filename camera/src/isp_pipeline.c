// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include <stdint.h>
#include <stdio.h>

#include <xcore/assert.h>
#include <xcore/channel.h> // includes streaming channel and channend

#include "camera_api.h"
#include "sensor_control.h"
#include "print.h"

#include "isp_pipeline.h"
#include "stats.h"

// ISP global variables
isp_params_t isp_params = {                                              
  .channel_gain = {
  AWB_gain_RED,
  AWB_gain_GREEN,
  AWB_gain_BLUE
  }
};

static
vfilter_acc_t vfilter_accs[APP_IMAGE_CHANNEL_COUNT][VFILTER_ACC_COUNT];

static
hfilter_state_t hfilter_state[APP_IMAGE_CHANNEL_COUNT];

__attribute__((aligned(8)))
int8_t output_buff[2][APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];

static 
unsigned out_dex = 0;                                                       


// gamma 1.8, with substract 10 and 1.05 multiplier (int8 version)
const int8_t gamma_int8[256] = {
-128,-128,-128,-128,-128,-128,-128,-126,-123,-120,-117,-114,-112,-109,-107,-105,
-102,-100,-98, -96, -94, -92, -90, -88, -86, -85, -83, -81, -79, -78, -76, -74,
-73, -71, -69, -68, -66, -65, -63, -62, -60, -59, -57, -56, -55, -53, -52, -51,
-49, -48, -46, -45, -44, -43, -41, -40, -39, -37, -36, -35, -34, -33, -31, -30,
-29, -28, -27, -25, -24, -23, -22, -21, -20, -19, -18, -16, -15, -14, -13, -12,
-11, -10, -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,  0,2,3,4,5,
6,6,7,8,9,10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
21,  22,  23,  24,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  33,  34,
35,  36,  37,  38,  39,  39,  40,  41,  42,  43,  44,  45,  45,  46,  47,  48,
49,  50,  50,  51,  52,  53,  54,  54,  55,  56,  57,  58,  58,  59,  60,  61,
62,  62,  63,  64,  65,  66,  66,  67,  68,  69,  69,  70,  71,  72,  72,  73,
74,  75,  75,  76,  77,  78,  78,  79,  80,  81,  81,  82,  83,  84,  84,  85,
86,  87,  87,  88,  89,  89,  90,  91,  92,  92,  93,  94,  94,  95,  96,  97,
97,  98,  99,  99,  100, 101, 101, 102, 103, 104, 104, 105, 106, 106, 107, 108,
108, 109, 110, 110, 111, 112, 112, 113, 114, 114, 115, 116, 116, 117, 118, 118,
119, 120, 120, 121, 122, 122, 123, 124, 124, 125, 126, 126, 127, 127, 127, 127,
};

// Stats functions
static histograms_t histograms;
static statistics_t statistics;


// ------------- PH <> ISP communication -----------------------

isp_cmd_t isp_recieve_cmd(chanend_t ch){
    isp_cmd_t cmd = (isp_cmd_t)chanend_in_word(ch);
    chanend_out_word(ch, RESP_OK);
    return cmd;
}
isp_cmd_t isp_send_cmd(chanend_t ch, isp_cmd_t cmd){
    chanend_out_word(ch, (uint32_t)cmd);
    return (isp_cmd_t)chanend_in_word(ch);
}


void isp_send_row_info(
    chanend_t ch,
    row_info_t *info){
    chan_out_buf_byte(ch, (uint8_t*)info, sizeof(row_info_t));
}
row_info_t isp_recieve_row_info(chanend_t ch){
    row_info_t info; 
    chan_in_buf_byte(ch, (uint8_t*)&info, sizeof(row_info_t));
    return info;
}

isp_cmd_t isp_wait(chanend_t ch){
    return (isp_cmd_t)chanend_in_word(ch);
}
void isp_signal(chanend_t ch){
    chanend_out_word(ch, RESP_OK);
}

// ------------- ISP functions -----------------------
static
int8_t csign(float x) {
  return (x > 0) - (x < 0);
}

static inline uint8_t AE_is_adjusted(float sk) {
  return (sk < AE_MARGIN && sk > -AE_MARGIN) ? 1 : 0;
}

static
uint8_t AE_compute_new_exposure(float exposure, float skewness)
{
  static float a  = 0;     // minimum value for exposure
  static float fa = -1;   // minimimum skewness
  static float b  = 80;    // maximum value for exposure
  static float fb = 1;    // minimum skewness
  static int count = 0;
  float c  = exposure;
  float fc = skewness;

  if(csign(fc) == csign(fa)){
    a = c; fa = fc;
  }
  else{
    b = c; fb = fc;
  }
  c  = b - fb*((b - a)/(fb - fa));

  // each X samples, restart AE algorithm
  if (count < 5){
    count = count + 1;
  }
  else{
    // restart auto exposure
    count = 0; a = 0; fa = -1; b = 80; fb = 1;    
  }
  return c;
}

static
uint8_t AE_control_exposure(
    statistics_t* global_stats,
    chanend_t c_control)
{
  // Initial exposure
  static uint8_t new_exp = AE_INITIAL_EXPOSURE;
  static uint8_t skip_ae_control = 0; // if too dark for a ceertain frames, skip AE control

  // Compute skewness and adjust exposure if needed
  float sk = stats_compute_mean_skewness(global_stats);
  if (AE_is_adjusted(sk)) {
    return 1;
  } else {
    // Adjust exposure
    new_exp = AE_compute_new_exposure((float)new_exp, sk);
    // Send new exposure
    uint32_t encoded_cmd = ENCODE(SENSOR_SET_EXPOSURE, new_exp);
    chan_out_word(c_control, encoded_cmd);
    chan_in_word(c_control);
    // Skip AE control if too dark
    if (new_exp > 70) {
      skip_ae_control++;
      if (skip_ae_control > 5) {
        skip_ae_control = 0;
        return 1;
      }
    }
  }
  return 0;
}

static
void AWB_compute_gains_static(isp_params_t *isp_params){
  isp_params->channel_gain[0] = AWB_gain_RED;
  isp_params->channel_gain[1] = AWB_gain_GREEN;
  isp_params->channel_gain[2] = AWB_gain_BLUE;
}

/* Intended for future use
static
float AWB_clip_value(float tmp){
  if (tmp > AWB_MAX){
    tmp = AWB_MAX;
  }
  else if (tmp < AWB_MIN){
    tmp = AWB_MIN;
  }
  return tmp;
}

static
void AWB_compute_gains_percentile(statistics_t* global_stats, isp_params_t *isp_params){
  // Adjust AWB 
  float tmp0=1;
  float tmp1=1;
  float tmp2=1; 
  
  uint8_t red_p   = global_stats->stats_red.percentile;
  uint8_t green_p = global_stats->stats_green.percentile;
  uint8_t blue_p  = global_stats->stats_blue.percentile;

  tmp0 = green_p/(float)red_p;
  tmp1 = 1;
  tmp2 = green_p/(float)blue_p;
  
  uint32_t r_per_count = global_stats->stats_red.per_count;
  uint32_t g_per_count = global_stats->stats_green.per_count;
  uint32_t b_per_count = global_stats->stats_blue.per_count;

  float tmpA = (float)g_per_count/(float)r_per_count;
  float tmpC = (float)g_per_count/(float)b_per_count;
  
  tmp0 = (tmp0 + tmpA)/2.0;
  tmp2 = (tmp2 + tmpC)/2.0;

  tmp0  = AWB_clip_value(tmp0);
  tmp1  = AWB_clip_value(tmp1);
  tmp2  = AWB_clip_value(tmp2);

  isp_params->channel_gain[0] = tmp0;
  isp_params->channel_gain[1] = tmp1;
  isp_params->channel_gain[2] = tmp2;
}
*/

// ------------- Core functions -----------------------

static
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
void send_row_camera(
    const int8_t pix_out[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS],
    row_info_t* info)
{
  camera_new_row_decimated(pix_out, info->state_ptr->out_line_number);
  info->state_ptr->out_line_number++;
  stats_compute_histograms(&histograms, APP_IMAGE_WIDTH_PIXELS, pix_out);
}

static
void hfilter(
  const uint8_t channel,
  int8_t hf_row[APP_IMAGE_WIDTH_PIXELS],
  int8_t* input,
  hfilter_state_t hf_state[APP_IMAGE_CHANNEL_COUNT])
{
  pixel_hfilter(
    hf_row,
    input,
    &hf_state[channel].coef[0],
    hf_state[channel].acc_init,
    hf_state[channel].shift,
    APP_DECIMATION_FACTOR,
    APP_IMAGE_WIDTH_PIXELS);
}

static
void process_row(chanend_t c_isp){
    
    // Tmp buffer for horizontal filter
    int8_t hfilt_row[APP_IMAGE_WIDTH_PIXELS];

    // recieve the row pointers
    row_info_t info = isp_recieve_row_info(c_isp);
    
    // Obtain pattern
    unsigned ln = info.state_ptr->in_line_number;
    unsigned pattern = ln % 2;

    // First, service any raw requests.
    camera_new_row((int8_t*) info.row_ptr, ln);

    // Print aux info
    //printf("rx_pix[0]=%d\n", (int8_t)info.row_ptr[0]);
    //printf("R=%d\n", info.state_ptr->in_line_number);

    // Apply downsample
    if(pattern == 0){
        // RED
        hfilter(CHAN_RED, hfilt_row, info.row_ptr, hfilter_state);
        image_vfilter_process_row(
            &output_buff[out_dex][CHAN_RED][0],
            &vfilter_accs[CHAN_RED][0],
            &hfilt_row[0]);

        // GREEN
        hfilter(CHAN_GREEN, hfilt_row, info.row_ptr, hfilter_state);
        image_vfilter_process_row(
            &output_buff[out_dex][CHAN_GREEN][0],
            &vfilter_accs[CHAN_GREEN][0],
            &hfilt_row[0]);

    } else{ // GB_PATTERN

        // BLUE
        hfilter(CHAN_BLUE, hfilt_row, info.row_ptr, hfilter_state);
        unsigned new_row = image_vfilter_process_row(
            &output_buff[out_dex][CHAN_BLUE][0],
            &vfilter_accs[CHAN_BLUE][0],
            &hfilt_row[0]);

        if (new_row) {
            send_row_camera(output_buff[out_dex], &info);
            out_dex ^= 1;
        }
    }

    // Send response
    isp_signal(c_isp);

    // Reset stats if ROW(0) (do not need sync here)
    if(ln == 0){
        stats_reset(&histograms, &statistics);
    }
}

static
void filter_drain(chanend_t c_isp)
{
    row_info_t info = isp_recieve_row_info(c_isp);
    image_vfilter_drain(&output_buff[out_dex][CHAN_RED][0], &vfilter_accs[CHAN_RED][0]);
    image_vfilter_drain(&output_buff[out_dex][CHAN_GREEN][0], &vfilter_accs[CHAN_GREEN][0]);
    image_vfilter_drain(&output_buff[out_dex][CHAN_BLUE][0], &vfilter_accs[CHAN_BLUE][0]);
    send_row_camera(output_buff[out_dex], &info);
    out_dex ^= 1;
}

static
void process_end_of_frame(chanend_t c_isp, chanend_t c_control)
{
    // Constants definitions
    const size_t img_size = W*H;
    const float inv_img_size = 1.0f / img_size;
    static unsigned run_once = 0;
    uint8_t ae_done = 0;

    //const size_t row_size = W;
    //const float inv_row_size = 1.0f / row_size;

    // Compute stats
    stats_compute_stats(&statistics, &histograms, inv_img_size);

    // AE control exposure
    if (run_once == 0){
      ae_done = AE_control_exposure(&statistics, c_control);
    }

    // Adjust AWB
    if (ae_done == 1 && run_once == 0) 
    {
    AWB_compute_gains_static(&isp_params);
    run_once = 1; // Set to 1 to run only once
    }

}

// ------------- ISP thread -----------------------
void isp_thread(chanend_t c_isp, chanend_t c_control){
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
            case PROCESS_EOF:
                process_end_of_frame(c_isp, c_control);
                break;
            case ISP_STOP:
                return;
            default:
                xassert(0 && "Invalid command");
                break;
            
        }
    }
}
