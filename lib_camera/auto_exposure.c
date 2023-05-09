#include "auto_exposure.h"

uint8_t sign(float x) {
    return (x > 0) - (x < 0);
}

uint8_t false_position_step(float c, float fc){
    static float a = 0;
    static float fa = -1;
    static float b = 80;
    static float fb = 1;
    static int count = 0;

    if(sign(fc) == sign(fa)){
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


uint8_t old_false_position_step(float c, float fc){
    static float a = 0;
    static float fa = -0.6;
    static float b = 80;
    static float fb = 0.6;

    static uint8_t last_skew = 0;
    if (c == last_skew) 
        return c;
        
    if(sign(fc) == sign(fa)){
        a = c; fa= fc;
    }
    else{
        b = c; fb = fc;
    }
    c  = b - fb*((b -a)/(fb -fa));

    last_skew = c;
    return c;
}


/*
Detect if two integers have opposite signs
int x, y;               // input values to compare signs
bool f = ((x ^ y) < 0); // true iff x and y have opposite signs
*/