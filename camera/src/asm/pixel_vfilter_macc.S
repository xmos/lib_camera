// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <xs3a_registers.h>

.issue_mode dual

#define FUNCTION_NAME   pixel_vfilter_macc
#define NSTACKWORDS     4

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

  void pixel_vfilter_macc(
      int16_t* accs,
      const int8_t* pix_in,
      const int8_t filter[16],
      const unsigned length_bytes);

 ****************************************************
 ****************************************************
*/

#define accs        r0
#define pix_in      r1
#define filter      r2
#define len         r3

#define _32         r4


.align 4
.skip 0
FUNCTION_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[1]

  ldc r11, 0x200              
{ mov r4, len                 ; shr len, len, 4             }
{ zext r4, 4                  ; vsetc r11                   }
//len must be multiple of 16
{ ecallt r4                   ; vldc filter[0]              }
{ ldc _32, 32                 ; bu .L_acc_loop              }

.align 16
.L_acc_loop:
  { add r11, accs, _32          ; vldd accs[0]                }
  { sub len, len, 1             ; vldr r11[0]                 }
  { add pix_in, pix_in, 8       ; vlmacc pix_in[0]            }
  { add accs, r11, _32          ; vstd accs[0]                }
  { add pix_in, pix_in, 8       ; vstr r11[0]                 }
  {                             ; bt len, .L_acc_loop         }

  ldd r4, r5, sp[1]
  retsp NSTACKWORDS

.size FUNCTION_NAME, .-FUNCTION_NAME
.cc_bottom FUNCTION_NAME.func
