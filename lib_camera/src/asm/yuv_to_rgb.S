// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
Converts signed yuv (-127..127, -127..127, -127..127) into signed rgb. 
Returns an int with 3 x int8_t

  int yuv_to_rgb(
      int y, 
      int u, 
      int v);
*/

.cc_top yuv_to_rgb.func, yuv_to_rgb
.align 4
.globl yuv_to_rgb
.globl yuv_to_rgb.nstackwords
.linkset yuv_to_rgb.nstackwords, 4
.issue_mode dual


#define y   r0
#define u   r1
#define v   r2
#define _22 r3
#define nstackwords 8

yuv_to_rgb:

// r4 - r10 are not used here

{ ldc r11, 0                  ; dualentsp nstackwords       }   // Enable dual instructions
{ ldc _22, 22                 ; vsetc r11                   }   // Load 22 in R3 ; control register and the headroom in the vector unit. Signed 32-bit integer
{ shl y, y, _22               ; ldaw r11, sp[0]             }   // Multiply by 2^22; load word in the Sp

{ shl u, u, _22               ; stw y, r11[0]               }   // Multiply by 2^22; store y in the vpu
{ shl v, v, _22               ; stw u, r11[1]               }   // Multiply by 2^22; store u in the vpu
{ ldc r3, 32                  ; stw v, r11[2]               }   // Load constant;   store v in the vpu

#undef _22
#define _32 r3
#define _24 r1

#undef y
#undef u
#undef v

{ ldc _24, 24                 ; vclrdr                      }   // load in R1 24 ; Sets the contents of vD and vR in the vector unit to all zeroes.
{ neg _24, _24                ; vldc r11[0]                 }   // signed negation ; load first opdfand
ldap r11, bluConv                                               // Load effective address relative to the program counter.
{ add r11, r11, _32           ; vlmaccr r11[0]              }   
{ add r11, r11, _32           ; vlmaccr r11[0]              }
{ ldaw r11, sp[0]             ; vlmaccr r11[0]              }

{                             ; vstr r11[0]                 }
  vlashr r11[0], _24
{                             ; vdepth8                     }
{                             ; vstr r11[0]                 }
{                             ; ldw r0, sp[0]               }
retsp nstackwords

// the values correspond to the matrix values *255
// https://en.wikipedia.org/wiki/YUV
bluConv: .word  256,  520,    0, 0, 0, 0, 0, 0      
grnConv: .word  256, -101, -149, 0, 0, 0, 0, 0
redConv: .word  256,    0,  292, 0, 0, 0, 0, 0

.cc_bottom yuv_to_rgb.func
