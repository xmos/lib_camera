
// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#pragma once

#include <stdint.h>
#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdlib.h> // free, alloc
#include <stdint.h>
#include <stdbool.h>

#include <xccompat.h>

#include "sensor.h"
#include "image_hfilter.h"
#include "image_vfilter.h"

#include "camera_utils.h" // for time measure

// ISP settings
#define AE_MARGIN 0.1 // default marging for the auto exposure error
#define AE_INITIAL_EXPOSURE 35 // initial exposure value
#define AWB_gain_RED 1.538
#define AWB_gain_GREEN  1.0
#define AWB_gain_BLUE   1.587
#define AWB_MAX         1.7
#define AWB_MIN         0.8
#define APPLY_GAMMA     1
#define ENABLE_PRINT_STATS  0
#define HISTOGRAM_BIN_COUNT 64
#define HFILTER_INPUT_STRIDE  (APP_DECIMATION_FACTOR)


// ISP cmd responses
#define RESP_OK  0x0
#define RESP_ERR 0x1

#if defined(__XC__)
extern "C" {
#endif

typedef enum{
    FILTER_UPDATE,
    PROCESS_ROW,
    FILTER_DRAIN
} isp_cmd_t;

// Isp structs
typedef struct {
  float channel_gain[APP_IMAGE_CHANNEL_COUNT];
} isp_params_t;

typedef struct {
  int8_t pixels[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];
} low_res_image_row_t;

typedef struct {
    unsigned wait_for_frame_start;
    unsigned frame_number;
    unsigned in_line_number;
    unsigned out_line_number;
} frame_state;


typedef struct{
  int8_t *row_ptr;
  frame_state* state_ptr;
} row_info_t;


// Filter global state
extern vfilter_acc_t vfilter_accs[APP_IMAGE_CHANNEL_COUNT][VFILTER_ACC_COUNT];
extern hfilter_state_t hfilter_state[APP_IMAGE_CHANNEL_COUNT];
extern int8_t output_buff[2][APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];
extern uint8_t out_dex;
extern frame_state ph_state;

unsigned isp_send_cmd(chanend ch, isp_cmd_t cmd);
isp_cmd_t isp_recieve_cmd(chanend ch);

void isp_send_row_info(chanend ch, row_info_t* info);
row_info_t isp_recieve_row_info(chanend ch);

void filter_update();
void isp_thread(chanend c_ph_isp, chanend c_control);

#if defined(__XC__)
}
#endif
