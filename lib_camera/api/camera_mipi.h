// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "api.h"
#include "camera_defs.h"

C_API_START

/**
 * @brief Initialize the MIPI camera.
 *
 * @param ctx  The MIPI context to initialize
 */
void camera_mipi_init(camera_mipi_ctx_t* ctx);


C_API_END
