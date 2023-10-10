
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

#define AWB_gain_RED    1.538
#define AWB_gain_GREEN  1.0
#define AWB_gain_BLUE   1.587

#define AWB_MAX         1.7
#define AWB_MIN         0.8
#define APPLY_GAMMA     1
#define ENABLE_PRINT_STATS  0

#define HFILTER_INPUT_STRIDE  (APP_DECIMATION_FACTOR)


// ISP cmd responses
#if defined(__XC__)
extern "C" {
#endif

typedef enum{
    RESP_OK = 0,
    RESP_NOK,
    FILTER_UPDATE,
    PROCESS_ROW,
    FILTER_DRAIN,
    EOF_ADJUST,
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
} frame_state_t;

typedef struct{
  int8_t *row_ptr;
  frame_state_t* state_ptr;
} row_info_t;


// Filter global state
extern isp_params_t isp_params;

// Mesage functions
unsigned isp_send_cmd(chanend ch, isp_cmd_t cmd);
isp_cmd_t isp_recieve_cmd(chanend ch);

void isp_send_row_info(chanend ch, row_info_t* info);
row_info_t isp_recieve_row_info(chanend ch);

isp_cmd_t isp_wait(chanend ch);
void isp_signal(chanend ch);

// Main thread
void isp_thread(chanend c_ph_isp, chanend c_control);

// Gamma
extern const int8_t  gamma_int8[256];

#if defined(__XC__)
}
#endif
