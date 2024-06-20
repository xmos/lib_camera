// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stddef.h>

#include <xcore/hwtimer.h>

#include "camera_utils.h"


inline
void delay_ticks_cpp(unsigned ticks){
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, ticks);
  hwtimer_free(tmr);
}

inline
void delay_milliseconds_cpp(unsigned delay) {
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, delay * XS1_TIMER_MHZ * 1000);
  hwtimer_free(tmr);
}

inline
void delay_seconds_cpp(unsigned int delay) {
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, delay * XS1_TIMER_HZ);
  hwtimer_free(tmr);
}

void vect_int8_to_uint8(
    uint8_t output[],
    int8_t input[],
    const size_t length)
{
  for (size_t k = 0; k < length; k++)
    output[k] = input[k] + 128;
}
