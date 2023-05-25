#include "statistics.h"

#define BINS 64                 // changing the number of bins leads to change in implementation
#define PERCENTILE_VALUE 0.05   // value selected for the percentile distribution

#define row(x,w) (x / w)
#define col(x,w) (x % w)

#define RED   0
#define GREEN 1
#define BLUE  2

Statistics *Statistics_alloc(void) {
    Statistics *point;
    point = malloc(sizeof(*point));
    if (point == NULL) {
        return NULL;
    }
    memset(point, 0, sizeof(*point));
    return point;
}

void Statistics_free(Statistics *self) {
    free(self);
}

Statistics Statistics_initialize(void) {
    Statistics s = {{0}};
    return s;
}

void Statistics_compute_histogram(const uint32_t buffsize, const uint8_t step, uint8_t *buffer, Statistics *statistics)
{
    // fill the histogram
    for (uint32_t i=0; i< buffsize; i = i + step){
        statistics->histogram[(buffer[i] >> 2)] += 1; // because 255/4 = 64
    }

    // normalize
    float inv_factor = (float)step / (float)buffsize;
    for (uint8_t j=0; j < BINS; j++){
        statistics->histogram[j] *= inv_factor; 
    }
}

void Statistics_compute_skewness(Statistics *statistics)
{
    const float zk_values[] = {
        -1.0,-0.907753,-0.821362,-0.740633,-0.665375,-0.595396,-0.530504,-0.470508,-0.415214,-0.364431,
        -0.317968,-0.275632,-0.237231,-0.202574,-0.171468,-0.143721,-0.119142,-0.097538,-0.078717,-0.062488,
        -0.048659,-0.037037,-0.027431,-0.019648,-0.013497,-0.008786,-0.005323,-0.002915,-0.001372,-0.0005,
        -0.000108,-4e-06,4e-06,0.000108,0.0005,0.001372,0.002915,0.005323,0.008786,0.013497,
        0.019648,0.027431,0.037037,0.048659,0.062488,0.078717,0.097538,0.119142,0.143721,0.171468,
        0.202574,0.237231,0.275632,0.317968,0.364431,0.415214,0.470508,0.530504,0.595396,0.665375,
        0.740633,0.821362,0.907753,1.0};
    
    float skew = 0.0;
    for (int k = 0; k < BINS; k++)
    {
        float pzk = statistics->histogram[k]; // we asssumed values are normalized
        skew += zk_values[k] * pzk;
    }
    statistics -> skewness = skew;
}


void Statistics_compute_minmaxavg(Statistics *statistics){
    
    float temp_mean = 0;
    uint8_t temp_min = 0;
    uint8_t temp_max = 0;

    // mean
    for (uint8_t k = 1; k < BINS; k++) // k=0 does not contribute to mean value
    {
        temp_mean += statistics->histogram[k] * k; // assuming histogram is normalized
    }
    
    // min 
    for (uint8_t k = 0; k < BINS; k++)
    {
        if (statistics->histogram[k]){
            temp_min = k;
            break;
        }
    }

    // max 
    for (uint8_t k = BINS -1; k > 0; k--)
    {
        if (statistics->histogram[k]){
            temp_max = k;
            break;
        }
    }

    // Values *4 to return to 0-255
    statistics -> mean = (uint8_t) (temp_mean+0.5) << 2; // +0.5 to avoid ceiling
    statistics -> min = (uint8_t) temp_min << 2;
    statistics -> max = (uint8_t) temp_max << 2; 
}

void Statistics_compute_percentile(Statistics *statistics){
    float sump = 0.0;
    uint8_t k = 0;
    // percentile
    for (k = BINS - 1; k > 0; k--)
    {
        sump += statistics->histogram[k];
        if (sump > PERCENTILE_VALUE){   // I assume histogram is normalized to 0-1
            break;
        }
    }
    statistics -> percentile = (k << 2); // Values *4 to return to 0-255
}

uint16_t Statistics_compute_variance(Statistics *statistics){
    uint8_t mean      = statistics->mean >> 2; // /4 to return to histogram range 0-64
    float   diff      = 0.0;
    double  variance  = 0;

    for (uint8_t k = 0; k < BINS; k++){
        if (statistics->histogram[k]){
            diff = (k  - mean);
            diff = diff*diff;
            variance += statistics->histogram[k]*diff;
        }
    }
    variance = (uint16_t)(variance * 16); // Values *16 to return to 0-255
    return variance;
}

void Statistics_compute_all(const uint32_t buffsize, const uint8_t step, uint8_t *buffer, Statistics *statistics){
    Statistics_compute_histogram(buffsize, step, buffer, statistics);
    Statistics_compute_skewness(statistics);
    Statistics_compute_minmaxavg(statistics);
    Statistics_compute_percentile(statistics);
}

static char get_rgb_color(uint32_t pos, uint16_t width) {
    static const char color_table[4] = {RED, GREEN, GREEN, BLUE};
    uint32_t x = pos % width;
    uint32_t y = pos / width;
    uint8_t index = ((y & 1) << 1) | (x & 1);
    return color_table[index];
}


void Statistics_print_info(Statistics *st){
    printf("min:%d, max:%d, mean:%d, percentile:%d", 
        st->min, 
        st->max, 
        st->mean,
        st->percentile);
}