#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <xcore/channel.h>

#include "process_frame.h"
#include "histogram.h"
#include "utils.h" // for measuring time
#include "auto_exposure.h"

#define FINAL_IMAGE_FILENAME "img_raw.bin"
#define AE_MARGIN 0.1
#define ENABLE_AE 1

const uint32_t img_len = MIPI_LINE_WIDTH_BYTES*MIPI_IMAGE_HEIGHT_PIXELS;
float new_exp = 35;

// Write image to disk. This is called by camera main () to do the work
void write_image(uint8_t *image)
{
  static FILE* img_file = NULL;
  img_file = fopen(FINAL_IMAGE_FILENAME, "wb");
  for(uint16_t k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS; k++){
    for(uint16_t j = 0; j < MIPI_IMAGE_WIDTH_BYTES; j++){
      uint32_t pos = k * MIPI_LINE_WIDTH_BYTES + j;
      fwrite(&image[pos], sizeof(uint8_t), 1, img_file);
      }
  }
  fclose(img_file);
  printf("Outfile %s\n", FINAL_IMAGE_FILENAME);
  printf("image size (%dx%d)\n", MIPI_LINE_WIDTH_BYTES, MIPI_IMAGE_HEIGHT_PIXELS);
  free(image);
  exit(1);
}


void process_image(uint8_t *image, chanend_t c){
  static int initial_t = 0;
  if (initial_t == 0){
    initial_t = measure_time();
  }

  static int print_msg = 0;
  // compute histogram
  float hist[64] = {0};
  compute_hist_64_norm(img_len, image, hist);  

  // compute skewness
  float sk = skew_fast(hist);

  // print information
  printf("texp=%f , skewness=%f\n", new_exp, sk);

  // exit condition
  if (sk < AE_MARGIN && sk > -AE_MARGIN){
    if (print_msg == 0){
      printf("-----> adjustement done\n");
      print_msg = 1;
    } 
  }
  else{
      // adjust exposure
      new_exp = false_position_step(new_exp, sk);

      // put exposure
      #if ENABLE_AE
        chan_out_word(c, (unsigned) new_exp);
      #endif
      // cancel output message
      print_msg = 0;
  }

  // write it to a file
  if (measure_time() - initial_t >= 604435538){
      write_image(image);
  }

}


// This is called when want to memcpy from Xc to C
void c_memcpy(
    void* dst,
    void* src,
    size_t size)
{
  memcpy(dst, src, size);
}


/*
void set_exposure(chanend_t c){
  printf("hello world");
  int ret = chan_in_word(c_otp);
  printf("ret  = %d\n", ret);
}
*/


/*
void user_input(void){
  printf("Enter the character 'c' to capture the frame : ");
  int c = getchar();
  if(c != 99){ // == ascii "c"
    exit(0);
  }
}
*/

/*
  // compute histogram
  //int t0 = measure_time();    
  float hist[64] = {0};
  compute_hist_64_norm(img_len, image, hist);  
  //int t1 = measure_time();
  //PRINT_TIME(t0, t1);

  // compute skewness
  //t0 = measure_time();
  float sk = skew_fast(hist);
  //t1 = measure_time();
  //PRINT_TIME(t0, t1);
*/