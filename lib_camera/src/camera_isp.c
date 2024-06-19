// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include <xcore/assert.h>
#include <xcore/select.h>
#include <print.h>

#include "lib_camera.h"  // in reality image def could be in isp.h

#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_mipi_defines.h"

#include "sensor_control.h"

#define PIPELINE_TIME_MS 100

#define ALIGNED_8 __attribute__((aligned(8)))


// Contains the local state info for the packet handler thread.
static frame_state_t ph_state = {
    1,  // wait_for_frame_start
    0,  // frame_number
    0,  // in_line_number
    0   // out_line_number
};

static void send_delay_start(
  chanend_t c_control,
  unsigned milliseconds) {
  chan_out_word(c_control, ENCODE(SENSOR_STREAM_START, milliseconds));
  chan_in_word(c_control);
}

static void send_stop(
  chanend_t c_control) {
  chan_out_word(c_control, ENCODE(SENSOR_STREAM_STOP, 0));
  chan_in_word(c_control);
}

// -------- Error handling --------
static
void handle_unknown_packet(
  mipi_data_type_t data_type) {
  xassert(data_type < 0x3F && "Packet non valid");
}

static
void handle_no_expected_lines() {
  if (ph_state.in_line_number >= MIPI_IMAGE_HEIGHT_PIXELS) {
    // We've received more lines of image data than we expected.
#ifdef ASSERT_ON_TOO_MANY_LINES
    xassert(0 && "Recieved too many lines");
#endif
  }
}

// -------- Frame handling --------
static
void camera_isp_packet_handler(
  const mipi_packet_t* pkt,
  chanend_t c_control,
  chanend_t c_user,
  Image_t* image) {
  // Definitions
  const mipi_header_t header = pkt->header;
  const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);

  // Wait for a clean frame
  if (ph_state.wait_for_frame_start
    && data_type != MIPI_DT_FRAME_START) return;

  // Handle packets depending on their type
  switch (data_type) {
  case MIPI_DT_FRAME_START:
    printstrln("SOF");
    ph_state.wait_for_frame_start = 0;
    ph_state.in_line_number = 0;
    ph_state.out_line_number = 0;
    ph_state.frame_number++;
    break;

  case CONFIG_MIPI_FORMAT:
    printstr("d,");
    handle_no_expected_lines();
    ph_state.in_line_number++;
    break;

  case MIPI_DT_FRAME_END:
    printstrln("\nEOF");
    send_stop(c_control);
    break;

  default:
    handle_unknown_packet(data_type);
    break;
  }
}

void camera_isp_capture_in_ms(
  chanend_t c_user,
  unsigned ms,
  Image_t* image) {
  chan_out_word(c_user, ms);
  chan_out_buf_byte(c_user, (uint8_t*)image, sizeof(Image_t));
  //TODO dothe same with image configuration
}


// -------- Main packet handler thread --------
void camera_isp_thread(
  streaming_chanend_t c_pkt,
  streaming_chanend_t c_ctrl,
  chanend_t c_control,
  chanend_t c_user) {

  __attribute__((aligned(8)))
  mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];
  mipi_packet_t* pkt;
  unsigned pkt_idx = 0;
  Image_t image;

  delay_milliseconds_cpp(2200); // Wait for the sensor to start

  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);


  SELECT_RES(
    CASE_THEN(c_pkt, on_c_pkt_change),
    CASE_THEN(c_user, on_c_user_change)) {

  on_c_pkt_change: { // attending mipi_packet_rx
    pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);
    s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);
    camera_isp_packet_handler(pkt, c_control, c_user, &image);
    continue;
    }
  on_c_user_change: { // attending user_app
    unsigned miliseconds = chan_in_word(c_user);
    chan_in_buf_byte(c_user, (uint8_t*)&image, sizeof(Image_t));
    unsigned start_delay = miliseconds - PIPELINE_TIME_MS;
    printf("User asked: %d\n", miliseconds);
    printf("Pipeline time: %d\n", PIPELINE_TIME_MS);
    printf("Starting in %d ms\n", miliseconds - PIPELINE_TIME_MS);
    // TODO I should send config first
    send_delay_start(c_control, start_delay); // at this point isp is ready to recieve

    continue;
    }
  }

}
