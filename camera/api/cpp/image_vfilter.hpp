#pragma once

#include <cstdint>


// The number of non-zero taps in the vertical filter.
#define VFILTER_TAP_COUNT   (5)




#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Initialize a vector of 32-bit split accumulators to a given value.
 * 
 * This function is used to initialize a set of accumulators to an offset value
 * to prepare for filtering.
 * 
 * The `pixel_vfilter_*()` functions use the VPU in 8-bit mode. All accumulators
 * are initialized to the same 32-bit value, with the values split as required
 * by the VPU.
 * 
 * Accordingly, `pix_count` must be a multiple of 16.
 * 
 * @param accs      The vector of accumulators to initialize.
 * @param acc_value The value to initialize the accumulators to.
 * @param pix_count The number of accumulators to initialize.
 */
void pixel_vfilter_acc_init(
    int16_t* accs,
    const int32_t acc_value,
    const unsigned pix_count);

/**
 * Generate output pixel values from a vector of accumulators.
 * 
 * This function is to be called on a row of accumulators after the final filter
 * tap has been applied. The 32-bit accumulators are right-shifted by the values
 * in `shifts[]` (with rounding), saturated to 8-bit symmetric bounds, and
 * dropped from int32_t to int8_t before being written to `pix_out[]`.
 * 
 * `pix_out[]`, `accs` and `shifts` must each be aligned to a 4-byte boundary.
 * 
 * `accs` is expected to be formatted according to the VPU's split 32-bit
 * accumulator format.
 * 
 * `shifts[k]` will be applied to accumulators with indices `k mod 16`. Usually,
 * each `shift[k]` will be the same.
 * 
 * Due to VPU limitations, `pix_count` must be a multiple of 16.
 * 
 * `pix_count` bytes will be written to `pix_out[]`.
 * 
 * @param pix_out   The output pixel values.
 * @param accs      The vector of accumulators to generate output from.
 * @param shifts    The right-shifts to apply to each accumulator.
 * @param pix_count The number of output pixels.
 */
void pixel_vfilter_complete(
    int8_t* pix_out,
    const int16_t* accs,
    const int16_t shifts[16],
    const unsigned pix_count);


/**
 * Apply a filter tap to a vector of accumulators using the supplied input
 * pixels.
 * 
 * This function is used to apply a single filter tap to a vector of
 * accumulators (i.e. `pix_count` separate filters are updated).
 * 
 * If `ACC32[k]` is the 32-bit accumulator associated with output pixel `k`,
 * then this function computes:
 *    
 *    ACC32[k] += (pix_in[k] * filter[k % 16])
 * 
 * Because this function is used for incrementally applying the same vertical
 * filter across many columns of an image, the values `filter[k]` will typically
 * all be the same value, `COEF` for a given call to this function, simplifying
 * to:
 * 
 *    ACC32[k] += (pix_in[k] * COEF)
 * 
 * The `pix_in[]` array is expected to contain `pix_count` 8-bit pixels.
 * 
 * `accs` is expected to be formatted according to the VPU's split 32-bit
 * accumulator format.
 * 
 * `accs`, `pix_in` and `filter[]` must each be 4-byte aligned.
 * 
 * Due to VPU limitations, `pix_count` must be a multiple of 16.
 * 
 * When the final filter tap is applied, `pixel_vfilter_complete()` should be
 * called to generate output pixels from the accumulators.
 * 
 * @param accs      The vector of accumulators to apply the filter tap to.
 * @param pix_in    The input pixels to apply to the filter.
 * @param filter    Vector containing filter coefficients.
 * @param pix_count The number of pixels to apply the filter to.
 */
void pixel_vfilter_macc(
    int16_t* accs,
    const int8_t* pix_in,
    const int8_t filter[16],
    const unsigned pix_count);


#if defined(__XC__) || defined(__cplusplus)
}
#endif

#ifdef __cplusplus

static
const int32_t vfilter_acc_offset = 0;

static
const int8_t vfilter_coef[5][16] = {
  {  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,},
  { 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,},
  {114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,},
  { 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,},
  {  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,},
};

static
const int16_t vfilter_shift[16] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,};




/**
 * 
 * 
 * 
 */
template <unsigned WIDTH, unsigned DEC_FACTOR>
class ImageVFilter {

  public:

    static constexpr
    unsigned ACC_COUNT = ((VFILTER_TAP_COUNT + DEC_FACTOR - 1) / DEC_FACTOR);

    static constexpr
    int RESET_INDEX = VFILTER_TAP_COUNT - (DEC_FACTOR * ACC_COUNT);


  public:

    struct {
      // The next filter tap to use
      unsigned next_tap;
      // There's a 32-bit accumulator for each output pixel
      int16_t buffer[2*WIDTH];
    } accumulator[ACC_COUNT];


  public:

    /**
     * 
     */
    void frame_init();

    /**
     * 
     */
    bool filter(
        int8_t output[WIDTH],
        const int8_t input[WIDTH]);

    /**
     * 
     */
    bool drain(
        int8_t output[WIDTH]);
};



template <unsigned WIDTH, unsigned DEC_FACTOR>
void ImageVFilter<WIDTH, DEC_FACTOR>::frame_init() 
{
  for(int k = 0; k < ACC_COUNT; k++){
    this->accumulator[k].next_tap = -DEC_FACTOR * k + (VFILTER_TAP_COUNT/2);
    pixel_vfilter_acc_init(this->accumulator[k].buffer, 
                           vfilter_acc_offset,
                           WIDTH);
  }
}


template <unsigned WIDTH, unsigned DEC_FACTOR>
bool ImageVFilter<WIDTH, DEC_FACTOR>::filter(
    int8_t output[WIDTH],
    const int8_t input[WIDTH]) 
{
  // Apply a filter tap to each accumulator
  for(int k = 0; k < ACC_COUNT; k++){
    if(this->accumulator[k].next_tap >= 0){
      pixel_vfilter_macc(this->accumulator[k].buffer,
                         input,
                         vfilter_coef[this->accumulator[k].next_tap],          WIDTH);
    }
    this->accumulator[k].next_tap++;
  }

  // Check for completed accumulators
  for(int k = 0; k < ACC_COUNT; k++){
    if(this->accumulator[k].next_tap != VFILTER_TAP_COUNT) continue;

    pixel_vfilter_complete(output,
                           this->accumulator[k].buffer,
                           vfilter_shift,
                           WIDTH);

    this->accumulator[k].next_tap = RESET_INDEX;
    pixel_vfilter_acc_init(this->accumulator[k].buffer, 
                           vfilter_acc_offset,
                           WIDTH);
    
    return 1;
  }

  return 0;
}


template <unsigned WIDTH, unsigned DEC_FACTOR>
bool ImageVFilter<WIDTH, DEC_FACTOR>::drain(
    int8_t output[WIDTH]) 
{
  for(int k = 0; k < ACC_COUNT; k++){
    if(this->accumulator[k].next_tap <= 0) continue;

    pixel_vfilter_complete(output,
                           this->accumulator[k].buffer,
                           vfilter_shift,
                           WIDTH);
    this->accumulator[k].next_tap = 0;

    return 1;
  }

  return 0;
}



#endif // __cplusplus