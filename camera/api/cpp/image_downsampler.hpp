#pragma once

#include <cstdint>

#include "image_hfilter.hpp"
#include "image_vfilter.hpp"

#ifdef __cplusplus




template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
class BayeredImageDownsampler {

  public:

    static constexpr
    unsigned CHANNEL_COUNT = 3;

    static constexpr
    unsigned WIDTH_OUT = (WIDTH_IN / DEC_FACTOR);

    static constexpr
    unsigned VERT_DEC_FACTOR = DEC_FACTOR / 2;

    using TVFilter = ImageVFilter<WIDTH_OUT, VERT_DEC_FACTOR>;

  public:

    unsigned row_toggle = 0;
    
    ImageHFilter hfilter[CHANNEL_COUNT] = {
      ImageHFilter(WIDTH_OUT, DEC_FACTOR, 0, SENSOR_BLACK_LEVEL),
      ImageHFilter(WIDTH_OUT, DEC_FACTOR, 1, SENSOR_BLACK_LEVEL),
      ImageHFilter(WIDTH_OUT, DEC_FACTOR, 1, SENSOR_BLACK_LEVEL)
    };
    TVFilter vfilter[CHANNEL_COUNT];

  public: 

    /**
     * 
     */
    BayeredImageDownsampler(){};

    // /**
    //  * 
    //  */
    // void init();

    /**
     * 
     */
    void update_gains(
        const float gains[CHANNEL_COUNT]);

    /**
     * 
     */
    void frame_init();

    /**
     * 
     */
    bool process_row(
        int8_t output[CHANNEL_COUNT][WIDTH_OUT],
        const int8_t input[WIDTH_IN]);

    /**
     * 
     */
    bool frame_end(
      int8_t output[CHANNEL_COUNT][WIDTH_OUT]);

};


// template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
// void BayeredImageDownsampler<WIDTH_IN,DEC_FACTOR>::init()
// {

// }

template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void BayeredImageDownsampler<WIDTH_IN,DEC_FACTOR>::update_gains(
    const float gains[CHANNEL_COUNT])
{
  for(unsigned i = 0; i < CHANNEL_COUNT; i++){
    this->hfilter[i].update_gain(gains[i]);
  }
}


template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
void BayeredImageDownsampler<WIDTH_IN,DEC_FACTOR>::frame_init()
{
  for(unsigned i = 0; i < CHANNEL_COUNT; i++){
    this->vfilter[i].frame_init();
  }
}

template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
bool BayeredImageDownsampler<WIDTH_IN,DEC_FACTOR>::process_row(
    int8_t output[CHANNEL_COUNT][WIDTH_OUT],
    const int8_t input[WIDTH_IN])
{
  // Buffer to store the horizontally filtered input row
  int8_t hfilt_row[WIDTH_OUT];

  // For a Bayered image, a new row of outputs is ready iff the third
  // channel has an output ready.
  bool res = false;

  if(!this->row_toggle){
    //// RED
    this->hfilter[0].filter(hfilt_row, input);
    this->vfilter[0].filter(output[0], hfilt_row);

    //// GREEN
    this->hfilter[1].filter(hfilt_row, input);
    this->vfilter[1].filter(output[1], hfilt_row);
  } else {
    //// BLUE
    this->hfilter[2].filter(hfilt_row, input);
    res = this->vfilter[2].filter(output[2], hfilt_row);
  }
  
  this->row_toggle = !this->row_toggle;
  return res;
}

template <unsigned WIDTH_IN, unsigned DEC_FACTOR>
bool BayeredImageDownsampler<WIDTH_IN,DEC_FACTOR>::frame_end(
    int8_t output[CHANNEL_COUNT][WIDTH_OUT])
{
  bool res = false;
  for(int k = 0; k < CHANNEL_COUNT; k++){
    res = this->vfilter[k].drain(output[k]);
  }

  return res;
}





#endif // __cplusplus