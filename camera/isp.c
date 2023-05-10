#include "isp.h"

// ---------------------------------- AE / AGC ------------------------------
uint8_t csign(float x) {
    return (x > 0) - (x < 0);
}

uint8_t isp_false_position_step(float exposure, float skewness)
{
    static float a = 0;
    static float fa = -1;
    static float b = 80;
    static float fb = 1;
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
    return c;
}


// ---------------------------------- AWB ------------------------------
void isp_AWB_dynamic(const uint32_t buffsize, 
                    uint8_t *red, 
                    uint8_t *green, 
                    uint8_t *blue)
{
    // compute stadistics by channel
    Stadistics st_red; 
    Stadistics st_green;
    Stadistics st_blue;
    uint8_t step = 1;

    Stadistics_compute_all(buffsize, step, red, &st_red);
    Stadistics_compute_all(buffsize, step, red, &st_green);
    Stadistics_compute_all(buffsize, step, red, &st_blue);

    // apply white balancing
    float alfa = (st_green.mean)/(st_red.mean);
    float beta = (st_green.mean)/(st_blue.mean);
    isp_AWB_static(buffsize, red, green, blue, alfa, beta);
}


void isp_AWB_static(const uint32_t buffsize, 
                    uint8_t *red, 
                    uint8_t *green, 
                    uint8_t *blue, 
                    float alfa, 
                    float beta)
{    
    // apply multiplication across all image    
    for (uint32_t i=0; i < buffsize; i++){
        red[i]  = (uint8_t) alfa*red[i];
        blue[i] = (uint8_t) beta*blue[i];
    }
}