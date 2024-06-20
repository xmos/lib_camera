// Copyright 2020-2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include "api.h"


/**
 * @brief VPU-based memcpy implementation.
 *
 * Same as standard `memcpy()` except for an extra constraint that both `dst and `src` must be
 * word-aligned addresses.
 *
 * @param[out]  dst   Destination address
 * @param[in]   src   Source address
 * @param[in]   bytes Number of bytes to copy
 *
 * @ingroup util_macros
 */
void xs3_memcpy(
    void* dst,
    const void* src,
    unsigned bytes);
