// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
Converts signed rgb (-127..127, -127..127, -127..127) into signed yuv. 
Returns an int with 3 x int8_t

  int camera_rgb_to_yuv(
      int r, 
      int g, 
      int b);
*/

.cc_top camera_rgb_to_yuv.func, camera_rgb_to_yuv
.align 4
.globl camera_rgb_to_yuv
.globl camera_rgb_to_yuv.nstackwords
.linkset camera_rgb_to_yuv.nstackwords, 4
.issue_mode dual


#define r   r0
#define g   r1
#define b   r2
#define _22 r3
#define nstackwords 8

camera_rgb_to_yuv:

// r4 - r10 are not used here

{ ldc r11, 0                  ; dualentsp nstackwords       }   // Enable dual instructions
{ ldc _22, 22                 ; vsetc r11                   }   // Load 22 in R3 ; control register and the headroom in the vector unit. Signed 32-bit integer
{ shl r, r, _22               ; ldaw r11, sp[0]             }   // Multiply by 2^22; load word in the Sp

{ shl g, g, _22               ; stw r, r11[0]               }   // Multiply by 2^22; store y in the vpu
{ shl b, b, _22               ; stw g, r11[1]               }   // Multiply by 2^22; store u in the vpu
{ ldc r3, 32                  ; stw b, r11[2]               }   // Load constant;   store v in the vpu

#undef _22
#define _32 r3
#define _24 r1

#undef y
#undef u
#undef v

{ ldc _24, 24                 ; vclrdr                      }   // load in R1 24 ; Sets the contents of vD and vR in the vector unit to all zeroes.
{ neg _24, _24                ; vldc r11[0]                 }   // signed negation ; load first opdfand
ldap r11, Vconv                                               // Load effective address relative to the program counter.
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
/*
Vconv: .word  157, -132,  -26, 0, 0, 0, 0, 0    
Uconv: .word  -38,  -74,  112, 0, 0, 0, 0, 0
Yconv: .word   77,  150,   29, 0, 0, 0, 0, 0  
*/

Vconv: .word  128, -107,  -21, 0, 0, 0, 0, 0    
Uconv: .word  -43,  -85,  128, 0, 0, 0, 0, 0
Yconv: .word   77,  150,   29, 0, 0, 0, 0, 0  


.cc_bottom camera_rgb_to_yuv.func


// References
// https://en.wikipedia.org/wiki/YUV
// https://softpixel.com/~cwright/programming/colorspace/yuv/
// https://www.mikekohn.net/file_formats/yuv_rgb_converter.php
//
