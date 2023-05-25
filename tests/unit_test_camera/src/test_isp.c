#include <stdint.h>

#include "unity.h"
#include "isp.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_yuv_to_rgb(void) {
    int Y = 76;
    int U = 84;
    int V = 255;
    Y -= 127;
    U -= 127;
    V -= 127;

    uint32_t result = yuv_to_rgb(Y,U,V);

    // Printing the extracted bytes   
    TEST_ASSERT_EQUAL_UINT8(222, (uint8_t)(GET_R(result)+127));
    TEST_ASSERT_EQUAL_UINT8(19,  (uint8_t)(GET_G(result)+127));
    TEST_ASSERT_EQUAL_UINT8(0,   (uint8_t)(GET_B(result)+127));
}

void test_rgb_to_yuv(void) {
    int R = 222;
    int G = 19;
    int B = 0;
    R -= 127;
    G -= 127;
    B -= 127;

    uint32_t result = rgb_to_yuv(R,G,B);

    // Printing the extracted bytes   
    TEST_ASSERT_EQUAL_UINT8(78,  (uint8_t)(GET_Y(result)+127));
    TEST_ASSERT_EQUAL_UINT8(83,  (uint8_t)(GET_U(result)+127));
    TEST_ASSERT_EQUAL_UINT8(230, (uint8_t)(GET_V(result)+127));
}


// ----------------------------------------------------------------
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_rgb_to_yuv);
    RUN_TEST(test_yuv_to_rgb);
    return UNITY_END();
}