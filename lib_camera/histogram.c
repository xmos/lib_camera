#include "histogram.h"

#define STEP 16
void compute_hist_64_norm(uint32_t buffsize, uint8_t *buffer, float histogram[64]){
    // fill the histogram
    for (uint32_t i=0; i< buffsize; i = i + STEP){
        histogram[buffer[i] / 4] += 1; // because 255/4 = 64
    }

    // normalize
    for (uint8_t j=0; j< 64; j++){
        histogram[j] /= (buffsize/STEP); 
    }
}

// for the moment this is the winner
void compute_hist_64_norm_pointer(uint32_t buffsize, uint8_t *buffer, float histogram[64]){
    // define a step
    const uint8_t step = 4*STEP; // because uint32

    // fill the histogram
    uint32_t* img_ptr = (uint32_t*)&buffer[0];      // point where we want to start
    for (uint32_t i=0; i < buffsize; i = i + step){
        uint32_t value = *img_ptr++;
        histogram[((value >> 24) & 0xFF) >> 2] += 1;
        histogram[((value >> 16) & 0xFF) >> 2] += 1;
        histogram[((value >> 8) & 0xFF) >> 2] += 1;
        histogram[(value & 0xFF) >> 2] += 1;
    }

    // normalize
    for (uint8_t j=0; j< 64; j++){
        histogram[j] /= buffsize; 
    }
}

void compute_hist_64_norm_pointer2(uint32_t buffsize, uint8_t *buffer, float histogram[64]){
    // fill the histogram
    uint32_t* img_ptr = (uint32_t*)&buffer[0]; // point where we want
    uint32_t i = -1;
    uint32_t max = buffsize >> 2;
    
    while (++i <= max){
        uint32_t value = *img_ptr++;
        uint8_t byte1 = (value >> 24) & 0xFF;
        uint8_t byte2 = (value >> 16) & 0xFF;
        uint8_t byte3 = (value >> 8) & 0xFF;
        uint8_t byte4 = value & 0xFF;
        histogram[byte1 >> 2] += 1;
        histogram[byte2 >> 2] += 1;
        histogram[byte3 >> 2] += 1;
        histogram[byte4 >> 2] += 1;
    }

    // normalize
    for (uint8_t j=0; j< 64; j++){
        histogram[j] /= buffsize; 
    }

}

void compute_hist_64_norm_pointer3(uint32_t buffsize, uint8_t *buffer, float histogram[64]){
    uint64_t* img_ptr = (uint64_t*)&buffer[0]; // point where we want
    uint32_t i = -1;
    uint32_t max = buffsize / 8; 
    while (++i <= max){
        uint64_t value = *img_ptr++;
        uint8_t byte1 = (value >> 56) & 0xFF;
        uint8_t byte2 = (value >> 48) & 0xFF;
        uint8_t byte3 = (value >> 40) & 0xFF;
        uint8_t byte4 = (value >> 32) & 0xFF;
        uint8_t byte5 = (value >> 24) & 0xFF;
        uint8_t byte6 = (value >> 16) & 0xFF;
        uint8_t byte7 = (value >> 8) & 0xFF;
        uint8_t byte8 = value & 0xFF;

        histogram[byte1 >> 2] += 1;
        histogram[byte2 >> 2] += 1;
        histogram[byte3 >> 2] += 1;
        histogram[byte4 >> 2] += 1;
        histogram[byte5 >> 2] += 1;
        histogram[byte6 >> 2] += 1;
        histogram[byte7 >> 2] += 1;
        histogram[byte8 >> 2] += 1;
    }

    // normalize
    for (uint8_t j=0; j< 64; j++){
        histogram[j] /= buffsize; 
    }

}

void compute_hist_64_doub(uint32_t buffsize, uint8_t *buffer, float histogram[64]){
    // fill the histogram
    uint32_t i=0;
    uint32_t j=buffsize;

    for (i,j; i < buffsize/2; i++, j--){
        histogram[buffer[i] >> 2] += 1;
        histogram[buffer[j] >> 2] += 1;
    }

    // normalize
    for (uint8_t j=0; j< 64; j++){
        histogram[j] /= buffsize; 
    }

}


void compute_hist_32(uint32_t buffsize, uint8_t *buffer, float histogram[32])
{
    // fill the histogram
    for (uint32_t i = 0; i < buffsize; i++){
        histogram[buffer[i] >> 3] += 1; // by 8 because 255/32 = 8
    }

    // normalize
    for (uint8_t j = 0; j < 32; j++){
        histogram[j] /= buffsize;
    }
}


// Compute the skew with a 64 bins histogram
float skew_fast(float *hist)
{
    const float zk_values[] = {
        -1.000000, -0.909149, -0.823975, -0.744293, -0.669922, -0.600677, -0.536377, -0.476837, 
        -0.421875, -0.371307, -0.324951, -0.282623, -0.244141, -0.209320, -0.177979, -0.149933, 
        -0.125000, -0.102997, -0.083740, -0.067047, -0.052734, -0.040619, -0.030518, -0.022247, 
        -0.015625, -0.010468, -0.006592, -0.003815, -0.001953, -0.000824, -0.000244, -0.000031, 
        0.000000,  0.000031,  0.000244,  0.000824,  0.001953,  0.003815,  0.006592,  0.010468, 
        0.015625,  0.022247,  0.030518,  0.040619,  0.052734,  0.067047,  0.083740,  0.102997, 
        0.125000,  0.149933,  0.177979,  0.209320,  0.244141,  0.282623,  0.324951,  0.371307, 
        0.421875,  0.476837,  0.536377,  0.600677,  0.669922,  0.744293,  0.823975,  0.909149};

    float skew = 0.0;
    for (int k = 0; k < 64; k++)
    {
        float pzk = hist[k]; // we asssumed values are normalized
        skew += zk_values[k] * pzk;
    }
    return skew;
}