#include "stadistics.h"

#define STEP 16
#define BINS 64 // changing the number of bins leads to change in implementation

#define row(x,w) (x / w)
#define col(x,w) (x % w)

#define RED 0
#define GREEN 1
#define BLUE 2

Stadistics *Stadistics_alloc(void) {
    Stadistics *point;
    point = (Stadistics *) malloc(sizeof(*point));
    if (point == NULL) {
        return NULL;
    }
    memset(point, 0, sizeof(*point));
    return point;
}

void Stadistics_free(Stadistics *self) {
    free(self);
}

void Stadistics_compute_histogram(uint32_t buffsize, uint8_t *buffer, Stadistics *stadistics){
    // fill the histogram
    for (uint32_t i=0; i< buffsize; i = i + STEP){
        stadistics->histogram[buffer[i] / 4] += 1; // because 255/4 = 64
    }

    // normalize
    for (uint8_t j=0; j< 64; j++){
        stadistics->histogram[j] /= (buffsize/STEP); 
    }
}

void Stadistics_compute_skewness(Stadistics *stadistics)
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
        float pzk = stadistics->histogram[k]; // we asssumed values are normalized
        skew += zk_values[k] * pzk;
    }
    stadistics -> skewness = skew;
}


void Stadistics_compute_minmaxavg(Stadistics *stadistics){
    
    float temp_mean = 0;
    uint8_t temp_min = 0;
    uint8_t temp_max = 0;

    // mean
    for (int k = 0; k < BINS; k++)
    {
        temp_mean += stadistics->histogram[k] * (k+1); // assuming histogram is normalized
    }
    
    // min 
    for (int k = 0; k < BINS; k++)
    {
        if (stadistics->histogram[k]){
            temp_min = k;
            break;
        }
    }

    // max 
    for (int k = BINS -1; k > 0; k--)
    {
        if (stadistics->histogram[k]){
            temp_max = k;
            break;
        }
    }

    stadistics -> mean = (uint8_t) temp_mean << 2; // Values *4 to return to 0-255
    stadistics -> min = (uint8_t) temp_min << 2;
    stadistics -> max = (uint8_t) temp_max << 2; 
}

void Stadistics_compute_all(uint32_t buffsize, uint8_t *buffer, Stadistics *stadistics){
    Stadistics_compute_histogram(buffsize, buffer, stadistics);
    Stadistics_compute_skewness(stadistics);
    Stadistics_compute_minmaxavg(stadistics);
}

static char get_rgb_color(uint32_t pos, uint16_t width) {
    static const char color_table[4] = {RED, GREEN, GREEN, BLUE};
    uint32_t x = pos % width;
    uint32_t y = pos / width;
    uint8_t index = ((y & 1) << 1) | (x & 1);
    return color_table[index];
}