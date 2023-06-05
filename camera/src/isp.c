#include "isp.h"

#define AE_MARGIN 0.1         // defaukt marging for the auto exposure error


// ---------------------------------- utils ------------------------------
static
uint8_t csign(float x) {
    return (x > 0) - (x < 0);
}

// ---------------------------------- AE / AGC ------------------------------
float AE_compute_mean_skewness(global_stats_t *gstats){
    float sk = 0.0;
    sk += (*gstats)[0].skewness;
    sk += (*gstats)[1].skewness;
    sk += (*gstats)[2].skewness;
    sk = sk / 3;
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
    if (count < 20){
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
    return (uint8_t)c;
}


// ---------------------------------- AWB ------------------------------
void AWB_compute_gains(global_stats_t *gstats, AWB_gains_t *gains){
    // Adjust AWB 
    const float ceil = 254.0;
    gains->alfa  = ceil / (float)(*gstats)[0].percentile; // RED
    gains->beta  = ceil / (float)(*gstats)[1].percentile; // GREEN
    gains->gamma = ceil / (float)(*gstats)[2].percentile; // BLUE
}

void AWB_print_gains(AWB_gains_t *gains){
    printf("awb:%f,%f,%f\n",gains->alfa,gains->beta,gains->gamma);
}