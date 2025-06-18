#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "unity_fixture.h"

#include "camera_isp.h"
#include "camera_conv.h"

#include "helpers.h"

#define MAX_TEST_SIZE (128*128)

// Unity
TEST_GROUP_RUNNER(uint8_conv) {
    RUN_TEST_CASE(uint8_conv, uint8_conv__size);
    RUN_TEST_CASE(uint8_conv, uint8_conv__time);
    RUN_TEST_CASE(uint8_conv, uint8_dst_rewrite);
}
TEST_GROUP(uint8_conv);
TEST_SETUP(uint8_conv) {print_separator("uint8_conv");}
TEST_TEAR_DOWN(uint8_conv) {}

typedef 
void (*conv_func_t)(uint8_t *, const int8_t *, unsigned);

static 
void test_conversion_with_size(unsigned size) {
    int8_t input[MAX_TEST_SIZE] ALIGNED_8;
    uint8_t expected[MAX_TEST_SIZE] ALIGNED_8;
    uint8_t output[MAX_TEST_SIZE] ALIGNED_8;

    TEST_ASSERT_TRUE(size <= MAX_TEST_SIZE);
    fill_array_rand_int8(input, size);
    camera_int8_to_uint8(expected, input, size);
    camera_int8_to_uint8_fast(output, input, size);
    for (unsigned i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(expected[i], output[i]);
    }
}

static
void timeit(conv_func_t func, uint8_t *dst, const int8_t *src, unsigned size) {
    unsigned t = get_reference_time();
    func(dst, src, size);
    t = get_reference_time() - t;
    printf("elapsed ticks: %d\n", t);
}

TEST(uint8_conv, uint8_conv__time){
    const unsigned size = 128;
    int8_t input[size] ALIGNED_8;
    uint8_t ref[size] ALIGNED_8;
    uint8_t out[size] ALIGNED_8;
    fill_array_rand_int8(input, size);
    timeit((conv_func_t)camera_int8_to_uint8, ref, input, size);
    timeit((conv_func_t)camera_int8_to_uint8_fast, out, input, size);
}

TEST(uint8_conv, uint8_conv__size){
    test_conversion_with_size(8);
    test_conversion_with_size(64);

    const unsigned n_runs = 12;
    for (unsigned i = 0; i < n_runs; i++) {
        unsigned size = rand() % 128 + 8;
        test_conversion_with_size(size);
    }
}

TEST(uint8_conv, uint8_dst_rewrite){
    const unsigned size = 128;
    int8_t input_0[size] ALIGNED_8;
    int8_t input_1[size] ALIGNED_8;
    uint8_t *ptr0 = (uint8_t *)input_0;
    uint8_t *ptr1 = (uint8_t *)input_1;
    
    fill_array_rand_int8(input_0, size);
    memcpy(input_1, input_0, size);
    
    camera_int8_to_uint8(ptr0, input_0, size);
    camera_int8_to_uint8_fast(ptr1, input_1, size);
    for (unsigned i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(ptr0[i], ptr1[i]);
    }
}
