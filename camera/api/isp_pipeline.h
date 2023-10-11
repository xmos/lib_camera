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
    ISP_STOP,
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

/**
 * @brief     Send a command form the Packet Handler (PH) to
 *            the Image Signal Processing thread (ISP)
 * @param ch  Channel to send the command
 * @param cmd Command to send
 * @return    unsigned response from the ISP
 */
unsigned isp_send_cmd(chanend ch, isp_cmd_t cmd);

/**
 * @brief     Recieve a command form the Packet Handler (PH) to
 *            the Image Signal Processing thread (ISP)
 * @param ch  Channel to recieve the command
 * @return    isp_cmd_t command sent by the PH
 */
isp_cmd_t isp_recieve_cmd(chanend ch);

/**
 * @brief       Send a row of data from the Packet Handler (PH) to
 *              the ISP
 * @param ch    Channel to send the row
 * @param info  row_info_t struct with the row pointer and the frame state
 */
void isp_send_row_info(chanend ch, row_info_t* info);

/**
 * @brief     Recieve a row of data from the PH.
 * @param ch  Channel to recieve the row
 * @return    row_info_t struct with the row pointer and the frame state
 */
row_info_t isp_recieve_row_info(chanend ch);

/**
 * @brief     Wait to revieve a command from the ISP
 * @param ch  channel to wait for the command
 * @return    isp_cmd_t  command sent by the ISP
 */
isp_cmd_t isp_wait(chanend ch);
void isp_signal(chanend ch);

/**
 * @brief ISP thread it recieves raw data and process it
 * 
 * @param c_isp 
 * @param c_control 
 */
void isp_thread(chanend c_isp, chanend c_control);

// Gamma
extern const int8_t  gamma_int8[256];

#if defined(__XC__)
}
#endif
