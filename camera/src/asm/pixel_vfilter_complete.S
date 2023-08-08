// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <xs3a_registers.h>

.issue_mode dual

#define FUNCTION_NAME   pixel_vfilter_complete
#define NSTACKWORDS     6

.globl FUNCTION_NAME.nstackwords
.globl FUNCTION_NAME.maxthreads
.globl FUNCTION_NAME.maxtimers
.globl FUNCTION_NAME.maxchanends

.linkset FUNCTION_NAME.nstackwords, NSTACKWORDS
.linkset FUNCTION_NAME.maxchanends, 0
.linkset FUNCTION_NAME.maxtimers,   0
.linkset FUNCTION_NAME.maxthreads,  0

.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.text
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

/*
 ****************************************************
 ****************************************************

  Call after accumulation to output a line of pixels.

  pix_count must be a multiple of 4.

  void pixel_vfilter_complete(
      int8_t* pix_out,
      const int16_t* accs,
      const int16_t shifts[16],
      const unsigned pix_count);

 ****************************************************
 ****************************************************
*/

#define pix_out     r0
#define accs        r1
#define shifts      r2
#define len         r3

#define _32         r4
#define _16         r5

#define mask        r6
#define tmp         r7


.align 4
.skip 0
FUNCTION_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[1]
  std r6, r7, sp[2]

  ldc r11, 0x200
{ ldc _32, 32                 ; vsetc r11                   }
{ ldc _16, 16                 ; mov tmp, len                }
{ shr len, len, 4             ; zext tmp, 4                 }
{ mkmsk mask, _16             ; bu .L_loop                  }

// len must be a multiple of 16
  ecallt tmp

.align 16
.L_loop:
  { add r11, accs, _32          ; vldd accs[0]                }
  { add accs, r11, _32          ; vldr r11[0]                 }
  { sub len, len, 1             ; vlsat shifts[0]             }
    vstrpv pix_out[0], mask
  { add pix_out, pix_out, _16   ; bt len, .L_loop             }

  ldd r6, r7, sp[2]
  ldd r4, r5, sp[1]
  retsp NSTACKWORDS

.size FUNCTION_NAME, .-FUNCTION_NAME
.cc_bottom FUNCTION_NAME.func
