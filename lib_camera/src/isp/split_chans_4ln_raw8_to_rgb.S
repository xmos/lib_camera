// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
void split_chans_4ln_raw8_to_rgb(int8_t * img_ptr, int8_t * tmp_buff, 
unsigned img_ln, unsigned img_width, unsigned tmp_width);
*/

.text
.issue_mode dual

#define FUNCTION_NAME   split_chans_4ln_raw8_to_rgb
#define NSTACKWORDS     16

#define STACK_TMP_WIDTH (NSTACKWORDS + 1)
#define STACK_TMP_VEC   (NSTACKWORDS - 8)
#define STACK_TMP_VEC1  (STACK_TMP_VEC + 2)
#define STACK_TMP_VEC2  (STACK_TMP_VEC + 4)
#define STACK_TMP_VEC3  (STACK_TMP_VEC + 6)

#define img_ptr         r0
#define tmp_buff        r1
#define tmp_width       r2
#define img_width       r3
#define _12             r9

.cc_top FUNCTION_NAME.func, FUNCTION_NAME
.align 4

FUNCTION_NAME:
{ ldc r11, 3                ; dualentsp NSTACKWORDS     }

std r4, r5, sp[0]
std r6, r7, sp[1]
std r8, r9, sp[2]

// scale x and y down, keep original img_width
mul r4, img_width, r11
ldc r11, 0x200
{ shr r2, r2, 1             ; vsetc r11                 }

// offset the image pointer, get rid of img_ln
// (img_ptr) + (img_ln * img_width * 3)
mul r2, r2, r4
{ shr r3, img_width, 2      ; add img_ptr, img_ptr, r2  }

{ ldc _12, 12               ; ldw tmp_width, sp[STACK_TMP_WIDTH]}

.width_loop_top:

  // construct the blocks 4x8
  // get samples from tmp_buff an put in the stack
  // accumulate tmp_buff ptr by 8
  { mov r7, tmp_buff          ; add tmp_buff, tmp_buff, 8 }

  ldd r8, r11, r7[0]
  std r8, r11, sp[STACK_TMP_VEC>>1]
  { add r7, r7, tmp_width     ; ldaw r5, sp[STACK_TMP_VEC]}

  ldd r8, r11, r7[0]
  std r8, r11, sp[STACK_TMP_VEC1>>1]
  { add r7, r7, tmp_width     ; sub r3, r3, 1             }

  ldd r8, r11, r7[0]
  std r8, r11, sp[STACK_TMP_VEC2>>1]
  { add r7, r7, tmp_width     ; ldc r6, 2                 }

  ldd r8, r11, r7[0]
  std r8, r11, sp[STACK_TMP_VEC3>>1]

  ldaw r11, cp[kernels_rgb2] // keep long
  { mov r7, r11               ; vldc r5[0]                }
  
  ldaw r11, cp[kernel_rgb2_shifts] // keep long

  .two_l_top:

    { mov r5, _12               ; ldc r8, 32                }
    { sub r6, r6, 1             ; vclrdr                    }

    .twelve_l_top:

      { sub r5, r5, 1             ; vlmaccr r7[0]             }
      { add r7, r7, r8            ; bt r5, .twelve_l_top      }

    // 12 bytes -> 4 rgb pixels
    { mkmsk r8, _12             ; vlsat r11[0]              }

    vstrpv img_ptr[0], r8

    { sub r8, r4, _12           ; bf r6, .end_loop          }

    // + row len
    { add img_ptr, img_ptr, r4  ; bu .two_l_top             }

  .end_loop:

  { sub img_ptr, img_ptr, r8  ; bt r3, .width_loop_top    }

ldd r4, r5, sp[0]
ldd r6, r7, sp[1]
ldd r8, r9, sp[2]

{                           ; retsp NSTACKWORDS         }

.cc_bottom FUNCTION_NAME.func


.size FUNCTION_NAME, .-FUNCTION_NAME
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.set FUNCTION_NAME.nstackwords, NSTACKWORDS
.globl FUNCTION_NAME.nstackwords
