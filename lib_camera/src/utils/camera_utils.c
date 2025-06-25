// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <xcore/hwtimer.h>

#include "camera_utils.h"

// -------------------- Delays -----------------------
void delay_ticks_cpp(unsigned ticks){
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, ticks);
  hwtimer_free(tmr);
}

void delay_milliseconds_cpp(unsigned delay) {
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, delay * XS1_TIMER_MHZ * 1000);
  hwtimer_free(tmr);
}

void delay_seconds_cpp(unsigned int delay) {
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, delay * XS1_TIMER_HZ);
  hwtimer_free(tmr);
}
