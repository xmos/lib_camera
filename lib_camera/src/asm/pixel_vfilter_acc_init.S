// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <xs3a_registers.h>

.issue_mode dual

#define FUNCTION_NAME   pixel_vfilter_acc_init
#define NSTACKWORDS     12

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

  Initialize a row of accumulators for vertical decimation.

  void pixel_vfilter_acc_init(
      int16_t* accs,
      const int32_t acc_value,
      const unsigned pix_count);

 ****************************************************
 ****************************************************
*/

#define STK_VEC_TMP       (NSTACKWORDS-8)

#define accs        r0
#define value       r1
#define len         r2
#define tmp         r3

#define _32         r4


.align 4
.skip 0
FUNCTION_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[1]

// broadcast the high and low accumulator initialization values into vectors

{                             ; mov tmp, value              }
  zip tmp, value, 4
{ ldaw r11, sp[STK_VEC_TMP]   ; ldc _32, 32                 }
  std tmp, tmp, r11[0]
  std tmp, tmp, r11[1]
  std tmp, tmp, r11[2]
  std tmp, tmp, r11[3]
{ shr r5, len, 4              ; vldd r11[0]                 }
  std value, value, r11[0]
  std value, value, r11[1]
  std value, value, r11[2]
  std value, value, r11[3]
{ zext len, 4                 ; vldr r11[0]                 }

// for now, assume the len is multiple of 16 and raise an exception if not.
//    Part of the problem with supporting a tail is that if it is supported here
//    it must also be supported everywhere else. Because the high and low half
//    words are split into groups of 16 (required by VPU), if the last group has
//    for example, 4, then how do we place those in memory? like:
//        acc_hi,acc_hi,acc_hi,acc_hi,acc_lo,acc_lo,acc_lo,acc_lo  ?
//    or like:
//        acc_hi,acc_hi,acc_hi,acc_hi,0,0,0,0,0,0,0,0,0,0,0,0,acc_lo,acc_lo,acc_lo,acc_lo,0,0,0,0,0,0,0,0,0,0,0,0,?
//    if the former, then all the other functions need to know that spacing. If
//    the latter, then we're effectively already requiring that space be
//    allocated for a multiple of 16 anyway.
  ecallt len

.L_loop_top:
  { add accs, accs, _32         ; vstd accs[0]                }
  { sub r5, r5, 1               ; vstr accs[0]                }
  { add accs, accs, _32         ; bt r5, .L_loop_top          }


.L_finish:
  ldd r4, r5, sp[1]
  retsp NSTACKWORDS

.size FUNCTION_NAME, .-FUNCTION_NAME
.cc_bottom FUNCTION_NAME.func
