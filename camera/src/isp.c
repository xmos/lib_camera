#include "isp.h"

#define AE_MARGIN 0.1          // default marging for the auto exposure error
#define AE_INITIAL_EXPOSURE 35 // initial exposure value

// ---------------------------------- utils ------------------------------
static
int8_t csign(float x) {
    return (x > 0) - (x < 0);
}

static 
float cabs(float x) {
    return x * csign(x);
}

// ---------------------------------- AE / AGC ------------------------------
uint8_t AE_control_exposure(
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
            return 1;
        }
    }
    else{ // Adjust exposure
        new_exp = AE_compute_new_exposure((float)new_exp, sk);
        sensor_control_set_exposure(sc_if, (uint8_t)new_exp);
        printf_info = 1;
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
const float AWB_MAX = 1.7;
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

void AWB_compute_gains_percentile(global_stats_t *gstats, isp_params_t *isp_params){
    // Adjust AWB 
    float tmp0=1.3;
    float tmp1=0.8;
    float tmp2=1.3; 

    // percentile adjustement
    printf("%d,%d,%d,%d,%d,%d,\n", 
        (*gstats)[0].min,
        (*gstats)[0].max,
        (*gstats)[1].min,
        (*gstats)[1].max,
        (*gstats)[2].min,
        (*gstats)[2].max);
    
    tmp0 =  255.0 / (float)(*gstats)[0].percentile; // RED
    tmp1 =  255.0 / (float)(*gstats)[1].percentile; // GREEN
    tmp2 =  255.0 / (float)(*gstats)[2].percentile; // BLUE

    
    // add skewness contribution
    const float skmin = -1;
    const float skmax = 1;
    const float gmin  = 1;
    const float gmax  = 1.5;
    const float grange = gmax - gmin;
    const float skrange = skmax - skmin;
    const float m = -(grange/skrange);

    float gains[3];

    for (int i=0; i< 3; i++){
        float sk = (*gstats)[i].skewness;
        gains[i] = m*(sk - skmin) + gmax;
    }

    //tmp0 = 0.7*tmp0 + 0.3*gains[0];
    //tmp1 = 0.7*tmp1 + 0.3*gains[1];
    //tmp2 = 0.7*tmp2 + 0.3*gains[2];

    tmp0  = AWB_clip_value(tmp0);
    tmp1  = AWB_clip_value(tmp1);
    tmp2  = AWB_clip_value(tmp2);

    isp_params->channel_gain[0] = tmp0;
    isp_params->channel_gain[1] = tmp1;
    isp_params->channel_gain[2] = tmp2;
}

void AWB_compute_gains_static(global_stats_t *gstats, isp_params_t *isp_params){
    // Adjust AWB 
    float tmp0=1.4;
    float tmp1=1.0;
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
    const float beta  = 0.9;
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

/*
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
246,247,247,247,246,245,244,243,242,241,240,239,238,237,236,235};

const uint8_t gamma_1p8_s1_green[255] =  {
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
246,247,247,247,246,245,244,243,242,241,240,239,238,237,236,235};


void isp_gamma_stride1(const uint32_t buffsize, uint8_t *img){
    // gamma naming: 1p8_s1 = gamma 1.8 , with a stride of 1
    // 1p8_s4 => img^(1/1.8) (in a normalizeed 0-1 image)
    for(uint32_t i=0; i<buffsize; i++){
        img[i] = gamma_1p8_s1[img[i]];
    }
}

void isp_gamma_1p8(
    uint8_t * img_in,
    const size_t height, 
    const size_t width, 
    const size_t channels
    )
{
    for(size_t k = 0; k < height; k++){
        for(size_t j = 0; j < width; j++){
            for(size_t c = 0; c < channels; c++){
                size_t index_in =  c * (height * width)   + k * width    + j;
                uint8_t value = img_in[index_in];
                if (c==1){
                    img_in[index_in] = gamma_1p8_s1_green[value];
                }
                else{
                    img_in[index_in] = gamma_1p8_s1[value];
                }
                
            }
        }
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
