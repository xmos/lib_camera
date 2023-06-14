#include "isp.h"

#define AE_MARGIN 0.1          // default marging for the auto exposure error
#define AE_INITIAL_EXPOSURE 35 // initial exposure value

// ---------------------------------- utils ------------------------------
static
uint8_t csign(float x) {
    return (x > 0) - (x < 0);
}

// ---------------------------------- AE / AGC ------------------------------
void AE_control_exposure(
    global_stats_t *global_stats,
    CLIENT_INTERFACE(sensor_control_if, sc_if))
{
    // Initial exposure
    static uint8_t new_exp = AE_INITIAL_EXPOSURE;
    static uint8_t printf_info = 1;

    // Compute skewness and adjust exposure if needed
    float sk = AE_compute_mean_skewness(global_stats);
    if (AE_is_adjusted(sk)){
        if (printf_info){
            printf("-----> adjustement done\n");
            printf_info = 0;
        }
    }
    else{ // Adjust exposure
        new_exp = AE_compute_new_exposure((float)new_exp, sk);
        sensor_control_set_exposure(sc_if, (uint8_t)new_exp);
        printf_info = 1;
    }
}

void AE_print_skewness(global_stats_t *gstats){
      printf("skewness:%f,%f,%f\n",
          (*gstats)[0].skewness,
          (*gstats)[1].skewness,
          (*gstats)[2].skewness);
}

float AE_compute_mean_skewness(global_stats_t *gstats){
    float sk = 0.0;
    sk += (*gstats)[0].skewness;
    sk += (*gstats)[1].skewness;
    sk += (*gstats)[2].skewness;
    sk = sk / 3.0;
    return sk;
}

uint8_t AE_is_adjusted(float sk) {
    return (sk < AE_MARGIN && sk > -AE_MARGIN) ? 1 : 0;
}

uint8_t AE_compute_new_exposure(float exposure, float skewness)
{
    static float a = 0;     // minimum value for exposure
    static float fa = -1;   // minimimum skewness
    static float b = 80;    // maximum value for exposure
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
const float AWB_ceil = 255.0;
const float AWB_MAX = 1.6;
const float AWB_MIN = 0.8;

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

static
void AWB_compute_gains_percentile(global_stats_t *gstats, isp_params_t *isp_params){
    // Adjust AWB 
    float tmp1=1.3;
    float tmp2=1.0;
    float tmp3=1.3; 

    // percentile adjustement
    printf("%d,%d,%d,%d,%d,%d,\n", 
        (*gstats)[0].min,
        (*gstats)[0].max,
        (*gstats)[1].min,
        (*gstats)[1].max,
        (*gstats)[2].min,
        (*gstats)[2].max);
    
    tmp1 =  254 / (float)(*gstats)[0].percentile; // RED
    tmp2 =  254 / (float)(*gstats)[1].percentile; // GREEN
    tmp3 =  254 / (float)(*gstats)[2].percentile; // BLUE

    tmp1 = 0.2*(1.3-tmp1)   + 1.3;
    tmp2 = 0.2*(1-tmp2)     + 1;
    tmp3 = 0.2*(1.3-tmp3)   + 1.3;

    tmp1  = AWB_clip_value(tmp1);
    tmp2  = AWB_clip_value(tmp2);
    tmp3  = AWB_clip_value(tmp3);

    isp_params->channel_gain[0] = tmp1;
    isp_params->channel_gain[1] = tmp2;
    isp_params->channel_gain[2] = tmp3;
}

void AWB_compute_gains(global_stats_t *gstats, isp_params_t *isp_params){
    // Adjust AWB 
    float tmp0=1.35;
    float tmp1=1.0;
    float tmp2=1.35; 

    isp_params->channel_gain[0] = tmp0;
    isp_params->channel_gain[1] = tmp1;
    isp_params->channel_gain[2] = tmp2;
}

void AWB_print_gains(isp_params_t *isp_params){
    printf("awb:%f,%f,%f\n",
    isp_params->channel_gain[0],
    isp_params->channel_gain[1],
    isp_params->channel_gain[2]);
}



// ---------------------------------- GAMMA ------------------------------
/**
* Apply 1.8 gamma to each pixel in an image using stride 1
* it take 255 bytes of memory, 0MUL, 0DIV, 2MEM ACCESS 
* @param buffsize - Size of the buffer to operate on.
* @param img - * Pointer to the image to operate on. Modified
*/
const uint8_t gamma_1p8_s1[255] =  {
0,12,17,22,25,29,32,35,37,40,42,44,47,49,51,53,55,57,58,60,62,64,65,67,69,
70,72,73,75,76,78,79,80,82,83,85,86,87,89,90,91,92,94,95,96,97,98,100,101,
102,103,104,105,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,139,
140,141,142,143,144,145,146,146,147,148,149,150,151,152,152,153,154,155,156,
157,157,158,159,160,161,161,162,163,164,165,165,166,167,168,169,169,170,171,
172,172,173,174,175,175,176,177,178,178,179,180,181,181,182,183,183,184,185,
186,186,187,188,188,189,190,191,191,192,193,193,194,195,195,196,197,198,198,
199,200,200,201,202,202,203,204,204,205,206,206,207,208,208,209,209,210,211,
211,212,213,213,214,215,215,216,217,217,218,218,219,220,220,221,222,222,223,
223,224,225,225,226,226,227,228,228,229,230,230,231,231,232,233,233,234,234,
235,236,236,237,237,238,238,239,240,240,241,241,242,243,243,244,244,245,245,
246,247,247,248,248,249,249,250,251,251,252,252,253,253,254,255};

void isp_gamma_stride1(const uint32_t buffsize, uint8_t *img){
    // gamma naming: 1p8_s1 = gamma 1.8 , with a stride of 1
    // 1p8_s4 => img^(1/1.8) (in a normalizeed 0-1 image)
    for(uint32_t i=0; i<buffsize; i++){
        img[i] = gamma_1p8_s1[img[i]];
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


void isp_rotate_image(const uint8_t* src, uint8_t* dest, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate the new coordinates after rotation
            int new_x = height - 1 - y;
            int new_y = x;

            // Copy the pixel value to the new position
            dest[new_y * height + new_x] = src[y * width + x];
        }
    }
}


void rotate_image_90(
  const char* filename,
  uint8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS])
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
