// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcore/hwtimer.h>

inline void delay_ticks_cpp(unsigned ticks){
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, ticks);
  hwtimer_free(tmr);
}

inline void delay_milliseconds_cpp(unsigned delay) {
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, delay * XS1_TIMER_MHZ * 1000);
  hwtimer_free(tmr);
}

inline void delay_seconds_cpp(unsigned int delay) {
  hwtimer_t tmr = hwtimer_alloc();
  hwtimer_delay(tmr, delay * XS1_TIMER_HZ);
  hwtimer_free(tmr);
}





#ifdef __cplusplus
} //extern "C"
#endif
