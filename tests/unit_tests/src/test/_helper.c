#include "_helper.h"

void fill_array_rand_uint8(uint8_t *image, size_t size){
    for(size_t idx = 0; idx < size; idx++){
        uint8_t random_number = (rand() % 256);
        image[idx] = random_number;
    }
}

void fill_array_rand_int8(int8_t *image, size_t size){
    for(size_t idx = 0; idx < size; idx++){
        int8_t random_number = (rand() % 256) - 128;
        image[idx] = random_number;
    }
}
