#pragma once

#include <cstdint>

#include <xcore/channel_streaming.h>

// #include "camera.h"

#include "image_downsampler.hpp"

#ifdef __cplusplus


template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
class PacketHandler {

  public:

    static constexpr
    unsigned CHANNEL_COUNT = 3;
    
    static constexpr
    unsigned WIDTH_OUT = (WIDTH_IN / DEC_FACTOR);

  private:
    streaming_chanend_t c_pkt;
    streaming_chanend_t c_ctrl;
    streaming_chanend_t c_stats;

    bool wait_for_frame_start = true;
    unsigned in_row_index = 0;
    unsigned out_row_index = 0;
    unsigned frame_number = 0;

    BayeredImageDownsampler<WIDTH_IN, DEC_FACTOR> downsampler;

    float channel_gains[CHANNEL_COUNT] = {1.0f, 1.0f, 1.0f};
    
    /*
    * These buffers store rows of the fully decimated image. They are passed
    * along to the statistics thread once the packet handler thread no longer
    * needs them.
    *
    * Two are needed -- the one  the decimator is currently filling, and the one
    * that the statistics thread is currently using.
    */
    __attribute__((aligned(8)))
    int8_t output_buff[2][CHANNEL_COUNT][WIDTH_OUT];
    unsigned buff_dex = 0;

  public:


    // PacketHandler(
    //     streaming_chanend_t c_pkt,
    //     streaming_chanend_t c_ctrl,
    //     streaming_chanend_t c_stats)
    //       : c_pkt(c_pkt), c_ctrl(c_ctrl), c_stats(c_stats) {}


    void thread_entry(
        streaming_chanend_t c_pkt,
        streaming_chanend_t c_ctrl,
        streaming_chanend_t c_stats);

    void handle_packet(
        const mipi_packet_t* pkt);

    void handle_frame_start(
        const mipi_packet_t* pkt);

    void handle_unknown_packet(
        const mipi_packet_t* pkt);

    bool handle_pixel_data(
        const mipi_packet_t* pkt);
    
    void handle_frame_end(
        const mipi_packet_t* pkt);

    void on_new_output_row();

    void handle_no_expected_lines();

};




template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::thread_entry(
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl,
    streaming_chanend_t c_stats)
{
  this->c_pkt = c_pkt;
  this->c_ctrl = c_ctrl;
  this->c_stats = c_stats;

  /*
   * These buffers will be used to hold received MIPI packets while they're
   * being processed.
   */
  __attribute__((aligned(8)))
  mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];
  unsigned pkt_idx = 0;

  camera_init();
  
  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(this->c_pkt, (unsigned) &packet_buffer[pkt_idx] );

  while(1) {
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT-1);

    mipi_packet_t * pkt = (mipi_packet_t*) s_chan_in_word(this->c_pkt);
    // Check is we are supose to stop or continue
    unsigned stop = camera_check_stop();

    if (stop == 1){
        // send stop to MipiReciever
        s_chan_out_word(c_pkt, (unsigned) NULL);
        // send stop to statistics
        s_chan_out_word(c_stats, (unsigned) 1);
        // end thread
        printf("\n\nMipiPacketHandler: stop\n\n");
        return;
    }
    else{
        // send info to MipiReciever
        s_chan_out_word(c_pkt, (unsigned) &packet_buffer[pkt_idx]);
    }

    // Process the packet
    this->handle_packet(pkt);
  }
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::handle_packet(
    const mipi_packet_t* pkt)
{
  
  // definitions
  const mipi_header_t header = pkt->header;
  const unsigned data_type = MIPI_GET_DATA_TYPE(header);

  // At start-up we usually want to wait for a new frame before processing
  // anything
  if(   this->wait_for_frame_start 
     && data_type != MIPI_DT_FRAME_START ) return;

  /*
    The idea here is that logic that keeps the packet handler in a coherent
    state, like tracking frame and line numbers, should go directly in here, but
    logic that actually interprets, processes or reacts to packet data should go
    into the individual functions.
  */
  switch(data_type)
  {
    case MIPI_DT_FRAME_START: 
      this->wait_for_frame_start = 0;
      this->in_row_index = 0;
      this->out_row_index = 0;
      this->frame_number++;

      this->handle_frame_start(pkt);
      break;

    case MIPI_DT_FRAME_END:   
      this->handle_frame_end(pkt);
      this->buff_dex = 1 - this->buff_dex;
      break;

    case MIPI_EXPECTED_FORMAT:     
      this->handle_no_expected_lines();

      if(this->handle_pixel_data(pkt)){
        this->on_new_output_row();
        this->buff_dex = 1 - this->buff_dex;
      }

      this->in_row_index++;
      break;

    default:              
        // We've received a packet we don't know how to interpret.
      this->handle_unknown_packet(pkt);   
      break;
  }
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::handle_frame_start(
    const mipi_packet_t* pkt)
{
  this->downsampler.update_gains(this->channel_gains);
  this->downsampler.frame_init();
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::handle_unknown_packet(
    const mipi_packet_t* pkt)
{
  //TODO: manage uknown packets
  // uknown packets could be the following:
  // 1 - sensor specific packets (this could be useful for having more information about the frame)
  // 2 - error packets (in this case mipi reciever will raise an exception, but in the future we want to handle them here)
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
bool PacketHandler<WIDTH_IN,DEC_FACTOR>::handle_pixel_data(
    const mipi_packet_t* pkt)
{
  // First, service any raw requests.
  camera_new_row((int8_t*) &pkt->payload[0], this->in_row_index);

  return this->downsampler.process_row(this->output_buff[this->buff_dex],
                                       (const int8_t*) pkt->payload);
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::handle_frame_end(
    const mipi_packet_t* pkt)
{ 
  int8_t* pix_out = &this->output_buff[this->buff_dex][0][0];

  int8_t (*pix_out_2d)[WIDTH_OUT] = (int8_t (*)[WIDTH_OUT]) pix_out;

  if(this->downsampler.frame_end(pix_out_2d))
    this->on_new_output_row();

  s_chan_out_word(this->c_stats, (unsigned) NULL);
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::on_new_output_row()
{
  // printf("0x%08X\n", (unsigned) &this->output_buff[this->buff_dex][0][0]);
  // Pass the output row along for statistics processing
  s_chan_out_word(this->c_stats, 
                  (unsigned) &this->output_buff[this->buff_dex][0][0]);

  // Service and user requests for decimated output
  camera_new_row_decimated(this->output_buff[this->buff_dex], 
                           this->out_row_index);

  this->out_row_index++;
}



template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void PacketHandler<WIDTH_IN,DEC_FACTOR>::handle_no_expected_lines()
{
  #ifdef ASSERT_ON_TOO_MANY_LINES
  assert(this->in_row_index < MAX_LINES);
  #endif
}

#endif // __cplusplus