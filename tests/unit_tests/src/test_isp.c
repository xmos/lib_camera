#include <stdint.h>
#include "test_isp.h"

color_table_t ct_test = {16, 100, 16, 65, 100, 92}; // R G B Y U V 
//TODO include more colors for testing


void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_yuv_to_rgb()
{
    // Define color table 
    color_table_t ct_ref = ct_test;
    color_table_t ct_result;

    // Test the converison
    yuv_to_rgb_ct(&ct_ref, &ct_result);

    printColorTable(&ct_test);
    printColorTable(&ct_result);

    // Ensure conversion is correct
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.R, ct_result.R);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.G, ct_result.G);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.B, ct_result.B);
}

void test_rgb_to_yuv()
{
    // Define color table 
    color_table_t ct_ref = ct_test;
    color_table_t ct_result;

    // Test the converison
    rgb_to_yuv_ct(&ct_ref, &ct_result);

    printColorTable(&ct_test);
    printColorTable(&ct_result);

    // Printing the extracted bytes
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.Y, ct_result.Y);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.U, ct_result.U);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.V, ct_result.V);
}


// ----------------------------------------------------------------
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_rgb_to_yuv);
    RUN_TEST(test_yuv_to_rgb);
    return UNITY_END();
}