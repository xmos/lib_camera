#include <xs1.h>
#include <print.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <platform.h>
#include <string.h>
#include <math.h>
#include "i2c.h"
#include "mipi.h"
#include "mipi_main.h"
#include "extmem_data.h"
#include "mipi/MipiPacket.h"

/*
#include "gc2145.h"
#include "debayer.h"
#include "yuv_to_rgb.h"
*/

void mipi_main(client interface i2c_master_if i2c)
{
    printf("hello world\n");
}