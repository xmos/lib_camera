#include <assert.h>
#include <stdio.h>

#include <xcore/channel.h> // includes streaming channel and channend
#include <xcore/channel_streaming.h>
#include "xccompat.h"

#include "isp.h"

#define INCLUDE_ABS 0

// ---------------------------------- utils ------------------------------
static
int8_t csign(float x) {
    return (x > 0) - (x < 0);
}

#if INCLUDE_ABS
static 
float cabs(float x) {
    return x * csign(x);
}
#endif
// ---------------------------------- AE / AGC ------------------------------
#if ENABLE_PRINT_STATS
static
void print_info_exposure(uint8_t val)
{
    static uint8_t printf_info = 1;
    // reset
    if (val == 0) {
        printf_info = 1;
    }
    else {
        // set
        if (printf_info) {
            printf("-----> adjustement done\n");
            printf_info = 0;
        }
    }
}
#endif


uint8_t AE_control_exposure(
    global_stats_t *global_stats,
    chanend chan)
{
    // Initial exposure
    static uint8_t new_exp = AE_INITIAL_EXPOSURE;
    static uint8_t skip_ae_control = 0; // if too dark for a ceertain frames, skip AE control

    // Compute skewness and adjust exposure if needed
    float sk = AE_compute_mean_skewness(global_stats);
    if (AE_is_adjusted(sk)){
        #if ENABLE_PRINT_STATS
            print_info_exposure(1);
        #endif
        return 1;
    }
    else{ // Adjust exposure
        new_exp = AE_compute_new_exposure((float)new_exp, sk);
        chan_out_word(chan, (uint32_t)new_exp);
        #if ENABLE_PRINT_STATS
            print_info_exposure(0);
        #endif
        if (new_exp > 70){
            skip_ae_control++;
            if (skip_ae_control > 5){
                skip_ae_control = 0;
                return 1;
            }
        }
    }
    return 0;
}


void AE_print_skewness(global_stats_t *gstats){
      printf("skewness:%f,%f,%f\n",
          (*gstats)[0].skewness,
          (*gstats)[1].skewness,
          (*gstats)[2].skewness);
}

float AE_compute_mean_skewness(global_stats_t *gstats){
    float mean = (
        (*gstats)[0].skewness + \
        (*gstats)[1].skewness + \
        (*gstats)[2].skewness)/3;
    return mean;
}

inline uint8_t AE_is_adjusted(float sk) {
    return (sk < AE_MARGIN && sk > -AE_MARGIN) ? 1 : 0;
}

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
        count = 0;
        a = 0;
        fa = -1;
        b = 80;
        fb = 1;
    }
    return c;
}


// ---------------------------------- AWB ------------------------------
isp_params_t isp_params = {
  .channel_gain = {
    AWB_gain_RED,
    AWB_gain_GREEN,
    AWB_gain_BLUE
  }
};

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

void AWB_compute_gains_percentile(global_stats_t *gstats, isp_params_t *isp_params){
    // Adjust AWB 
    float tmp0=1;
    float tmp1=1;
    float tmp2=1; 
    
    uint8_t red_p   = (*gstats)[0].percentile;
    uint8_t green_p = (*gstats)[1].percentile;
    uint8_t blue_p  = (*gstats)[2].percentile;

    tmp0 = green_p/(float)red_p;
    tmp1 = 1;
    tmp2 = green_p/(float)blue_p;
    
    uint32_t r_per_count = (*gstats)[0].per_count;
    uint32_t g_per_count = (*gstats)[1].per_count;
    uint32_t b_per_count = (*gstats)[2].per_count;

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

void AWB_compute_gains_static(isp_params_t *isp_params){
    // Adjust AWB 
    float tmp0=1.4;
    float tmp1=1.1;
    float tmp2=1.4; 

    isp_params->channel_gain[0] = tmp0;
    isp_params->channel_gain[1] = tmp1;
    isp_params->channel_gain[2] = tmp2;
}

void AWB_compute_gains_white_patch(global_stats_t *gstats, isp_params_t *isp_params){
    float Rmax = (*gstats)[0].max; // RED
    float Gmax = (*gstats)[1].max; // GREEN
    float Bmax = (*gstats)[2].max; // BLUE

    float alfa = Gmax/Rmax;
    const float beta   = 0.9;
    float gamma = Gmax/Bmax;

    alfa   = AWB_clip_value(alfa);
    gamma  = AWB_clip_value(gamma);

    // apply params
    isp_params->channel_gain[0] = alfa;
    isp_params->channel_gain[1] = beta;
    isp_params->channel_gain[2] = gamma;
}

void AWB_compute_gains_white_max(global_stats_t *gstats, isp_params_t *isp_params){    
    // we assume green constant
    const float beta = 1.0;
    float alfa = 1.3;
    float gamma = 1.3;

    // 1 - Grey world
    float Ravg = (*gstats)[0].mean; // RED
    float Gavg = (*gstats)[1].mean; // GREEN
    float Bavg = (*gstats)[2].mean; // BLUE
    
    if(Gavg > Ravg)
        alfa = Gavg/Ravg;
    if(Gavg > Bavg)
        gamma = Gavg/Bavg;

    // 2 - Percentile volumne
    uint32_t r_per_count = (*gstats)[0].per_count;
    uint32_t g_per_count = (*gstats)[1].per_count;
    uint32_t b_per_count = (*gstats)[2].per_count;

    float alfa2 = (float)g_per_count/(float)r_per_count;
    float gamma2 = (float)g_per_count/(float)b_per_count;

    // Weighted mean
    float gww = 0.5; // grey world weight
    alfa  = gww*alfa  + (1-gww)*alfa2;
    gamma = gww*gamma + (1-gww)*gamma2;

    // clip the values
    alfa   = AWB_clip_value(alfa);
    gamma  = AWB_clip_value(gamma);

    // apply params
    isp_params->channel_gain[0] = alfa;
    isp_params->channel_gain[1] = beta;
    isp_params->channel_gain[2] = gamma;
}

void AWB_compute_gains_gray_world(global_stats_t *gstats, isp_params_t *isp_params){
    printf("AWB --->");
    float Ravg = (*gstats)[0].mean; // RED
    float Gavg = (*gstats)[1].mean; // GREEN
    float Bavg = (*gstats)[2].mean; // BLUE

    float alfa = Gavg/Ravg;
    const float beta  = 1;
    float gamma = Gavg/Bavg;

    alfa   = AWB_clip_value(alfa);
    gamma  = AWB_clip_value(gamma);

    isp_params->channel_gain[0] = alfa;
    isp_params->channel_gain[1] = beta;
    isp_params->channel_gain[2] = gamma;
}

void AWB_print_gains(isp_params_t *isp_params){
    printf("awb:%f,%f,%f\n",
    isp_params->channel_gain[0],
    isp_params->channel_gain[1],
    isp_params->channel_gain[2]);
}

// ---------------------------------- GAMMA ------------------------------

// gamma 1.8, with substract 10 and 1.05 multiplier
const uint8_t gamma_uint8[256] = {
0,   0,   0,   0,   0,   0,   0,   2,   5,   8,  11,  14,  16,
19,  21,  23,  26,  28,  30,  32,  34,  36,  38,  40,  42,  43,
45,  47,  49,  50,  52,  54,  55,  57,  59,  60,  62,  63,  65,
66,  68,  69,  71,  72,  73,  75,  76,  77,  79,  80,  82,  83,
84,  85,  87,  88,  89,  91,  92,  93,  94,  95,  97,  98,  99,
100, 101, 103, 104, 105, 106, 107, 108, 109, 110, 112, 113, 114,
115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
128, 130, 131, 132, 133, 134, 134, 135, 136, 137, 138, 139, 140,
141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 152,
153, 154, 155, 156, 157, 158, 159, 160, 161, 161, 162, 163, 164,
165, 166, 167, 167, 168, 169, 170, 171, 172, 173, 173, 174, 175,
176, 177, 178, 178, 179, 180, 181, 182, 182, 183, 184, 185, 186,
186, 187, 188, 189, 190, 190, 191, 192, 193, 194, 194, 195, 196,
197, 197, 198, 199, 200, 200, 201, 202, 203, 203, 204, 205, 206,
206, 207, 208, 209, 209, 210, 211, 212, 212, 213, 214, 215, 215,
216, 217, 217, 218, 219, 220, 220, 221, 222, 222, 223, 224, 225,
225, 226, 227, 227, 228, 229, 229, 230, 231, 232, 232, 233, 234,
234, 235, 236, 236, 237, 238, 238, 239, 240, 240, 241, 242, 242,
243, 244, 244, 245, 246, 246, 247, 248, 248, 249, 250, 250, 251,
252, 252, 253, 254, 254, 255, 255, 255, 255};

// gamma 1.8, with substract 10 and 1.05 multiplier (int8 version)
const int8_t gamma_int8[256] = {
-128,   -128,   -128,   -128,   -128,   -128,   -128,   -126,   -123,   -120,   -117,   -114,   -112,   -109,   -107,   -105,
-102,   -100,   -98,    -96,    -94,    -92,    -90,    -88,    -86,    -85,    -83,    -81,    -79,    -78,    -76,    -74,
-73,    -71,    -69,    -68,    -66,    -65,    -63,    -62,    -60,    -59,    -57,    -56,    -55,    -53,    -52,    -51,
-49,    -48,    -46,    -45,    -44,    -43,    -41,    -40,    -39,    -37,    -36,    -35,    -34,    -33,    -31,    -30,
-29,    -28,    -27,    -25,    -24,    -23,    -22,    -21,    -20,    -19,    -18,    -16,    -15,    -14,    -13,    -12,
-11,    -10,    -9,     -8,     -7,     -6,     -5,     -4,     -3,     -2,     -1,     0,      2,      3,      4,      5,
6,      6,      7,      8,      9,      10,     11,     12,     13,     14,     15,     16,     17,     18,     19,     20,
21,     22,     23,     24,     24,     25,     26,     27,     28,     29,     30,     31,     32,     33,     33,     34,
35,     36,     37,     38,     39,     39,     40,     41,     42,     43,     44,     45,     45,     46,     47,     48,
49,     50,     50,     51,     52,     53,     54,     54,     55,     56,     57,     58,     58,     59,     60,     61,
62,     62,     63,     64,     65,     66,     66,     67,     68,     69,     69,     70,     71,     72,     72,     73,
74,     75,     75,     76,     77,     78,     78,     79,     80,     81,     81,     82,     83,     84,     84,     85,
86,     87,     87,     88,     89,     89,     90,     91,     92,     92,     93,     94,     94,     95,     96,     97,
97,     98,     99,     99,     100,    101,    101,    102,    103,    104,    104,    105,    106,    106,    107,    108,
108,    109,    110,    110,    111,    112,    112,    113,    114,    114,    115,    116,    116,    117,    118,    118,
119,    120,    120,    121,    122,    122,    123,    124,    124,    125,    126,    126,    127,    127,    127,    127,
};

void isp_gamma(
    uint8_t * img_in,
    const uint8_t *gamma_curve,
    const size_t height, 
    const size_t width, 
    const size_t channels)
{
    assert(gamma_curve[255] != 0); // ensure all values are filles up
    size_t buffsize = height * width * channels;
    for(size_t idx = 0; idx < buffsize; idx++){
            img_in[idx] = gamma_curve[img_in[idx]];
    }
}

// -------------------------- ROTATE/RESIZE -------------------------------------
#define img(row, col, WIDTH) img[(WIDTH) * (row) + (col)]
#define out_img(row, col, WIDTH) out_img[(WIDTH) * (row) + (col)]

static void xmodf(float a, int *b, float *c, int *bp)
{
    // split integer and decimal part
    *b = (int)(a);
    *c = a - *b;
    // last operand for convinience 
    *bp = *b + 1;
}

void isp_bilinear_resize(
    const uint16_t in_width,
    const uint16_t in_height,
    uint8_t *img,
    const uint16_t out_width,
    const uint16_t out_height,
    uint8_t *out_img)
{
    // https://chao-ji.github.io/jekyll/update/2018/07/19/BilinearResize.html
    const float x_ratio = ((in_width - 1) / (float)(out_width - 1));
    const float y_ratio = ((in_height - 1) / (float)(out_height - 1));

    int x_l, y_l, x_h, y_h;
    float xw, yw;
    uint8_t a,b,c,d;

    for (uint16_t i = 0; i < out_height; i++)
    {
        for (uint16_t j = 0; j < out_width; j++)
        {

            float incrx = (x_ratio * j);
            float incry = (y_ratio * i);

            xmodf(incrx, &x_l, &xw, &x_h);
            xmodf(incry, &y_l, &yw, &y_h);

            a = img(y_l, x_l, in_width);
            b = img(y_l, x_h, in_width);
            c = img(y_h, x_l, in_width);
            d = img(y_h, x_h, in_width);

            uint8_t pixel = (uint8_t)(a * (1 - xw) * (1 - yw) +
                                      b * xw * (1 - yw) +
                                      c * yw * (1 - xw) +
                                      d * xw * yw);

            out_img(i, j, out_width) = pixel;
            printf("%d,", pixel);
        }
    }
}

void rotate_image_90(uint8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS])
{
  for(int c = 0; c < APP_IMAGE_CHANNEL_COUNT; c++) {
    for(int k = 0; k < APP_IMAGE_HEIGHT_PIXELS/2; k++) {
      for(int j = 0; j < APP_IMAGE_WIDTH_PIXELS; j++) {
        uint8_t a = image_buffer[c][k][j];
        uint8_t b = image_buffer[c][APP_IMAGE_HEIGHT_PIXELS-k-1][APP_IMAGE_WIDTH_PIXELS-j-1];
        image_buffer[c][k][j] = b;
        image_buffer[c][APP_IMAGE_HEIGHT_PIXELS-k-1][APP_IMAGE_WIDTH_PIXELS-j-1] = a;
      }
    } 
  }
}
