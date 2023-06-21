// Copyright (c) 2020, XMOS Ltd, All rights reserved

#include <xs1.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <platform.h>
#include <string.h>
#include <math.h>
#include "i2c.h"
#include "mipi.h"
#include "app.h"
#include "sensor.h"

#include "mipi.h"
#include "xccompat.h"

#include <xs1.h>
#include <platform.h>
#include <xscope.h>

#include "i2c.h"

// Sensor
#define MSG_SUCCESS "Stream start OK\n"
#define MSG_FAIL "Stream start Failed\n"

////////////////////////////////////////////////////////////////
// if true, print warnings when unexpected packet sequences are observed
#ifndef SEQUENCE_WARNINGS
# define SEQUENCE_WARNINGS (0)
#endif

// if true, don't assume we started listening for packets outside of a frame,
// so suppress any warnings until we see an SoF.
#ifndef IGNORE_SEQ_WARN_BEFORE_SOF
# define IGNORE_SEQ_WARN_BEFORE_SOF (0)
#endif

/*
  astew: I'm just trying out the stuff below to decide whether I think it's
         awful
*/
#define WARNING(COND) for(int fgsfds = 0; (COND) && (fgsfds < 1); fgsfds++) 
#define ONLY_IF(COND) if(!(COND)) break
#define NOT_IF(COND)  if((COND)) break


#ifndef REBASE_TIMESTAMPS
# define REBASE_TIMESTAMPS (1)
#endif

#ifndef PRINT_LOG_SUMMARY
# define PRINT_LOG_SUMMARY (1)
#endif

#ifndef PRINT_TIMING_STATS
# define PRINT_TIMING_STATS (1)
#endif

#ifndef WRITE_LOG_TO_FILE
# define WRITE_LOG_TO_FILE (1)
#endif


#ifndef PACKET_LOG_FILE
# define PACKET_LOG_FILE  "mipi_packet_log.csv"
#endif

 
#define TABLE_ROWS  (12020)

#define CSV_FORMATTING      "0x%08X,0x%08X,0x%08X\n"
#define CSV_FORMATTING_LEN  33
#define CSV_HEADER          "HEADER,START,END\n"
#define CSV_HEADER_LEN      17

////////////////////////////////////////////////////////////////

// Start port declarations
/* Declaration of the MIPI interface ports:
 * Clock, receiver active, receiver data valid, and receiver data
 */
on tile[MIPI_TILE]:         in port    p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE]:         in port    p_mipi_rxa = XS1_PORT_1E;
on tile[MIPI_TILE]:         in port    p_mipi_rxv = XS1_PORT_1I;
on tile[MIPI_TILE]:buffered in port:32 p_mipi_rxd = XS1_PORT_8A;

on tile[MIPI_TILE]:clock               clk_mipi   = MIPI_CLKBLK;


// Inclusive between
static inline
unsigned between(unsigned low, unsigned x, unsigned high){
  return (x >= low) && (x <= high);
}



typedef struct {
  uint32_t header;
  uint32_t start_time;
  uint32_t end_time;
} packet_timing_t;

// Function to grab timing info
void MipiGatherTiming(
    buffered in port:32 p_mipi_rxd,
    in port p_mipi_rxa,
    packet_timing_t table[],
    const unsigned N);



// Basically just subtract the first start time from every timestamp so that
// the first packet says it starts at 0.
static 
uint32_t rebaseTimestamps(
    packet_timing_t packet[],
    unsigned N)
{
  const uint32_t time_offset = packet[0].start_time;
  for(int k = 0; k < N; k++){
    packet[k].start_time -= time_offset;
    packet[k].end_time -= time_offset;
  }

  return time_offset;
}


typedef struct {
  uint32_t min;
  uint32_t max;
  uint64_t total;
  uint32_t count;
} timing_stats_t;



static
void updateTimingStats(
    timing_stats_t* stats,
    uint32_t timespan)
{
  if(stats->count == 0){
    stats->min = stats->max = timespan;
  } else {
    stats->min = (timespan < stats->min)? timespan : stats->min;
    stats->max = (timespan > stats->max)? timespan : stats->max;
  }

  stats->count++;
  stats->total += timespan;
}



typedef struct {
  struct {
    uint32_t start_of_frame;
    uint32_t end_of_frame;
    uint32_t yuv_data;
    uint32_t raw_data;
    uint32_t generic_long;
    uint32_t other_short;
    uint32_t other_long;
  } packet_count;

  struct {
    // Time between Start of Frame packet and End of Frame packet
    timing_stats_t SOF_to_EOF;
    // Time between the beginning of Start of Frame packet and the next
    // Start of Frame packet
    timing_stats_t SOF_to_SOF;
    // Time between the beginning of one packet of line data and the 
    // beginning of the next packet of line data
    timing_stats_t SOL_to_SOL;
    // Time between line data packet header and end of line data packet
    timing_stats_t SOL_to_EOL;
  } timing;
} mipi_timing_info_t;


static
mipi_timing_info_t extractTimingInfo(
    packet_timing_t packet[],
    unsigned N)
{
  // Used for tracking inter-packet timing.
  //   If any are 0, that means they haven't been observed yet.
  struct {
    unsigned SoF;
    unsigned EoF;
    unsigned SoL;
    unsigned EoL;
  } last = {0};

  mipi_timing_info_t result;
  memset(&result, 0, sizeof(mipi_timing_info_t));

  // Warn if we're seeing an odd sequence
  unsigned inside_frame = 0;

  for(int k = 0; k < N; k++){

      packet_timing_t* pkt = &packet[k];

      unsigned is_long = MIPI_IS_LONG_PACKET(pkt->header)? 1:0;
      unsigned data_type = MIPI_GET_DATA_TYPE(pkt->header);

      unsigned duration = pkt->end_time - pkt->start_time;

      if(!is_long){
        // Short packet
        assert(duration == 0);

        if(data_type == MIPI_DT_FRAME_START){

          ///// Start of Frame packet
          
          // for(int fgsfds = 0; (inside_frame) && (fgsfds < 1); fgsfds++) {
          WARNING(inside_frame) {
            ONLY_IF (SEQUENCE_WARNINGS);

            printf("Warning: Observed SoF while inside frame."
                    " (Log Index: %u)\n", k);
          }

          if(last.SoF){
            unsigned delta = pkt->start_time - last.SoF;
            updateTimingStats(&result.timing.SOF_to_SOF, delta);
          }

          last.SoF = pkt->start_time;
          result.packet_count.start_of_frame++;
          inside_frame = 1;

        } else if(data_type == MIPI_DT_FRAME_END) {

          ///// End of Frame packet

          WARNING(!inside_frame) {
            ONLY_IF (SEQUENCE_WARNINGS);
            NOT_IF  (last.SoF == 0 && IGNORE_SEQ_WARN_BEFORE_SOF);
            
            printf("Warning: Observed EoF while outside frame."
                    " (Log Index: %u)\n", k);
          }

          if(last.SoF){
            unsigned delta = pkt->start_time - last.SoF;
            updateTimingStats(&result.timing.SOF_to_EOF, delta);
          }

          last.EoF = pkt->start_time;
          result.packet_count.end_of_frame++;
          inside_frame = last.SoL = last.EoL = 0;

        } else {

          ///// Some other short packet
          result.packet_count.other_short++;
        }


      } else {
        // Long packet
        assert(duration);

        WARNING(!inside_frame) {
          ONLY_IF (SEQUENCE_WARNINGS);
          NOT_IF  (last.SoF == 0 && IGNORE_SEQ_WARN_BEFORE_SOF);

          printf("Warning: Observed long packet while outside frame."
                  " (Log Index: %u)\n", k);
        }


        if(between(MIPI_DT_YUV420_8BIT, data_type, MIPI_DT_YUV422_10BIT) ){
          // yuv packet
          result.packet_count.yuv_data++;
        } else if(between(MIPI_DT_NULL, data_type, MIPI_DT_RESERVED_0x17)){
          // Generic long packet
          result.packet_count.generic_long++;
        } else if(between(MIPI_DT_RAW24, data_type, MIPI_DT_RAW20)){
          // raw packet
          result.packet_count.raw_data++;
        } else {
          // other long packet
          result.packet_count.other_long++;
        }

        if(last.SoL){
          unsigned delta = pkt->start_time - last.SoL;
          updateTimingStats(&result.timing.SOL_to_SOL, delta);
        }

        updateTimingStats(&result.timing.SOL_to_EOL, duration);

        last.SoL = pkt->start_time;
        last.EoL = pkt->end_time;
      }
  }

  return result;
}


static
void printTimingInfo(
    mipi_timing_info_t* info)
{
  unsigned total_pkt = info->packet_count.start_of_frame
                     + info->packet_count.end_of_frame
                     + info->packet_count.yuv_data
                     + info->packet_count.generic_long
                     + info->packet_count.raw_data
                     + info->packet_count.other_short
                     + info->packet_count.other_long;

  printf("#### Packet Counts ####\n");
  printf("  Total:       % 5u\n", total_pkt);
  printf("\n");
  printf("  ## Short ##\n");
  printf("  Frame Start: % 5u\n", info->packet_count.start_of_frame);
  printf("  Frame End:   % 5u\n", info->packet_count.end_of_frame  );
  printf("  Other Short: % 5u\n", info->packet_count.other_short   );
  printf("\n");
  printf("  ## Long ##\n");
  printf("  YUV Data:    % 5u\n", info->packet_count.yuv_data      );
  printf("  Generic Long:% 5u\n", info->packet_count.generic_long  );
  printf("  RAW Data:    % 5u\n", info->packet_count.raw_data      );
  printf("  Other Long:  % 5u\n", info->packet_count.other_long    );

  printf("\n\n");
  printf("#### Timing Info ####\n\n");

  printf("             | Min (us)     |     Ave (us) |    Max (us)  \n");
  printf("-------------|--------------|--------------|--------------\n");

  #define FMT  "| % 12.2f | % 12.2f | % 12.2f"

  printf("  SoF to SoF " FMT "\n",
      info->timing.SOF_to_SOF.min * 0.01, 
      info->timing.SOF_to_SOF.total / (100.0 * info->timing.SOF_to_SOF.count),
      info->timing.SOF_to_SOF.max * 0.01);

  printf("  SoF to EoF " FMT "\n",
      info->timing.SOF_to_EOF.min * 0.01, 
      info->timing.SOF_to_EOF.total / (100.0 * info->timing.SOF_to_EOF.count),
      info->timing.SOF_to_EOF.max * 0.01);

  printf("  SoL to SoL " FMT "\n",
      info->timing.SOL_to_SOL.min * 0.01, 
      info->timing.SOL_to_SOL.total / (100.0 * info->timing.SOL_to_SOL.count),
      info->timing.SOL_to_SOL.max * 0.01);

  printf("  SoL to EoL " FMT "\n",
      info->timing.SOL_to_EOL.min * 0.01, 
      info->timing.SOL_to_EOL.total / (100.0 * info->timing.SOL_to_EOL.count),
      info->timing.SOL_to_EOL.max * 0.01);
  
  #undef FMT

  printf("\n   Note: Thread MIPS is also the number of instructions per microsecond.\n");
  printf(  "         Scaling above times by thread MIPS gives instruction budget.\n");

}


static 
void writePacketLog(
    const char* filename,
    packet_timing_t packet[],
    unsigned N)
{

  FILE * movable log_file = fopen(filename, "w");

  if(!log_file){
    printf("\n\nWARNING: Couldn't open '%s' to write packet log.\n\n", filename);
    return;
  }

  // WRITE HEADER
  fwrite(CSV_HEADER, CSV_HEADER_LEN, 1, log_file);

  for(int k = 0; k < N; k++){
    fprintf(log_file, CSV_FORMATTING, 
      packet[k].header, packet[k].start_time, packet[k].end_time);
  }
  
  fclose(move(log_file));
}




static inline
unsigned can_aggregate(mipi_header_t a, mipi_header_t b)
{
  return (MIPI_GET_DATA_TYPE(a) == MIPI_GET_DATA_TYPE(b))
      && (MIPI_GET_WORD_COUNT(a) == MIPI_GET_WORD_COUNT(b));
}



// Instead of printing full log, summarize it..
//   - roll up a sequence of consecutive "line" packets into a single line of text
static 
void printPacketLogSummary(
    packet_timing_t packet[],
    unsigned N)
{

  printf("#### Packet Log Summary ####\n\n");
  
  printf(" Packet | Data | Word  | Gap        | Start      | Duration   | End        | Packet |     \n");
  printf(" Index  | Type | Count | Time (us)  | Time (us)  |   (us)     | Time (us)  | Count  | Misc\n");
  printf("--------|------|-------|------------|------------|------------|------------|--------|-----\n");

#define FMT  " %5u  | 0x%02X | % 5u | % 10.2f | % 10.2f | % 10.2f | % 10.2f |   % 4u | %s\n"

  double prev_end_time_us = 0;  

  char str_extra[100] = {0};

  int k = 0;
  while(k < N) {

    if(!(MIPI_IS_LONG_PACKET(packet[k].header))) {

      packet_timing_t* pkt = &packet[k];

      unsigned is_long = MIPI_IS_LONG_PACKET(pkt->header)? 1:0;
      unsigned data_type = MIPI_GET_DATA_TYPE(pkt->header);
      unsigned word_count = MIPI_GET_WORD_COUNT(pkt->header);

      double start_time_us = 0.01 * pkt->start_time;
      double end_time_us = 0.01 * pkt->end_time;
      double duration_us = 0;
      double gap_time_us = start_time_us - prev_end_time_us;

      if(data_type == MIPI_DT_FRAME_START)     sprintf(str_extra, "Frame Start" );
      else if(data_type == MIPI_DT_FRAME_END)  sprintf(str_extra, "Frame End" );
      else                                     sprintf(str_extra, "Unknown Short Packet");



      printf(FMT, k, data_type, word_count, gap_time_us, start_time_us, duration_us, end_time_us, 1, str_extra);

      prev_end_time_us = start_time_us;
      k++;
    } else {

      // It's a long packet.
      // Aggregate packets until we encounter one with a different 
      // (data type, payload size) than the current one.

      int i = k + 1;
      while((i<N) && can_aggregate(packet[k].header, packet[i].header))
        i++;
      
      unsigned pkt_count = i - k;
      unsigned dt = MIPI_GET_DATA_TYPE(packet[k].header);
      unsigned wc = MIPI_GET_WORD_COUNT(packet[k].header);
      uint32_t start_time = packet[k].start_time;
      uint32_t end_time = packet[i-1].end_time;
      double start_time_us = 0.01 * start_time;
      double end_time_us = 0.01 * end_time;
      double duration_us = end_time_us - start_time_us;
      double gap_time_us = start_time_us - prev_end_time_us;

      if(     between(0x10, dt, 0x17)) sprintf(str_extra, "Generic Long Packet");
      else if(between(0x18, dt, 0x1F)) sprintf(str_extra, "YUV Data");
      else if(between(0x20, dt, 0x26)) sprintf(str_extra, "RGB Data");
      else if(between(0x27, dt, 0x2F)) sprintf(str_extra, "RAW Data");
      else                             sprintf(str_extra, "Unknown Long Packet");

      printf(FMT, k, dt, wc, gap_time_us, start_time_us, duration_us, end_time_us, 
             pkt_count, str_extra);
      

      prev_end_time_us = end_time_us;
      k = i;
    }

    #undef FMT
  }
}


static
packet_timing_t packet_log[TABLE_ROWS];

#define MIPI_CLK_DIV 1
#define MIPI_CFG_CLK_DIV 2

void mipi_main(
    client interface i2c_master_if i2c)
{

  write_node_config_reg(tile[MIPI_TILE], 
      XS1_SSWITCH_MIPI_DPHY_CFG3_NUM , 0x7E42);

  unsigned mipi_shim_cfg0 = MIPI_SHIM_CFG0_PACK(0,0,0,0,0);

  MipiPacketRx_init(tile[MIPI_TILE],
                    p_mipi_rxd, 
                    p_mipi_rxv, 
                    p_mipi_rxa, 
                    p_mipi_clk, 
                    clk_mipi,
                    mipi_shim_cfg0,
                    MIPI_CLK_DIV, 
                    MIPI_CFG_CLK_DIV);

  // Start camera and its configurations
  int r = 0;
  r |= sensor_initialize(i2c);
  delay_milliseconds(100); //TODO include this inside the function
  r |= sensor_configure(i2c);
  delay_milliseconds(500);
  r |= sensor_start(i2c);
  delay_milliseconds(2000);

  if (r != 0){
    printf(MSG_FAIL);
  }
  else{
    printf(MSG_SUCCESS);
  }

  printf("Waiting for %u MIPI packets...\n", TABLE_ROWS);
  MipiGatherTiming(p_mipi_rxd, p_mipi_rxa, packet_log, TABLE_ROWS);
  printf("  ... done.\n\n");

  if(REBASE_TIMESTAMPS) {
    printf("Rebasing timestamps...\n");
    uint32_t time_offset = rebaseTimestamps(packet_log, TABLE_ROWS);
    printf("  ... time offset is %.2f us.\n\n\n", 0.01 * time_offset);
  }

  if(PRINT_LOG_SUMMARY) {
    printPacketLogSummary(packet_log, TABLE_ROWS);
    printf("\n\n");
  }

  if(PRINT_TIMING_STATS){
    mipi_timing_info_t timing = extractTimingInfo(packet_log, TABLE_ROWS);
    printf("\n\n");

    printTimingInfo(&timing);
    printf("\n\n");
  }

  if(WRITE_LOG_TO_FILE){
    printf("Writing packet log to %s..\n", PACKET_LOG_FILE);
    writePacketLog(PACKET_LOG_FILE, packet_log, TABLE_ROWS);
    printf("  ...done.\n\n");

  }

  exit(0);
  i2c.shutdown();
}
