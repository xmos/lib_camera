// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
void demosaic_2ln_raw8_to_rgb(int8_t * img_ptr, int8_t * tmp_buff, 
unsigned img_ln, unsigned img_width, unsigned tmp_width);
*/

.text
.issue_mode dual

#define FUNCTION_NAME   demosaic_2ln_raw8_to_rgb
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

mul r4, img_width, r11 // row width
ldc r11, 0x200
{ ldc _12, 12               ; vsetc r11                 }

// offset the image pointer, get rid of img_ln
// (img_ptr) + (img_ln * img_width * 3)
mul r2, r2, r4
{ shr r3, img_width, 3      ; add img_ptr, img_ptr, r2  }

// so this api is weird, one iteration of the
// loop writes 2 lines of 8 rgb pixels,
// but the vpu can only hold 16 values after vlsat
// so we have to do 12 values (4 rgb pixels at the time)
// which makes it even more painfull as the loop will
// have to go 4 times and acc the pointer differently each time
// so first after you write 4 pixels, you acc the pointer by 12,
// then you write another 4 pixels, and acc pointer by row_len - 12,
// to end up at the same offset you started from
// but on the next line, write 4 pixels, add 12 again,
// write another 4 pixels, aaaand to set up the next loop iteration
// we need to add - row_len + 12, to get to the next unwritten index
// on the previous line, so saving row_len - 12 on the line below
// hope it makes sense, took me a while to get my head around it
{ sub r7, r4, _12           ; ldw tmp_width, sp[STACK_TMP_WIDTH]}

.width_loop_top:

  // construct the blocks 4x8
  // get samples from tmp_buff an put in the stack
  // accumulate tmp_buff ptr by 8
  { mov r4, tmp_buff          ; add tmp_buff, tmp_buff, 8 }

  ldd r8, r11, r4[0]
  std r8, r11, sp[STACK_TMP_VEC>>1]
  { add r4, r4, tmp_width     ; ldaw r5, sp[STACK_TMP_VEC]}

  ldd r8, r11, r4[0]
  std r8, r11, sp[STACK_TMP_VEC1>>1]
  { add r4, r4, tmp_width     ; sub r3, r3, 1             }

  ldd r8, r11, r4[0]
  std r8, r11, sp[STACK_TMP_VEC2>>1]
  { add r4, r4, tmp_width     ; ldc r6, 4                 }

  ldd r8, r11, r4[0]
  std r8, r11, sp[STACK_TMP_VEC3>>1]

  ldaw r11, cp[kernels_rgb1] // keep long
  { mov r4, r11               ; vldc r5[0]                }
  
  ldaw r11, cp[kernel_rgb1_shifts] // keep long

  .four_l_top:

    { mov r5, _12               ; ldc r8, 32                }
    { sub r6, r6, 1             ; vclrdr                    }

    .twelve_l_top:

      { sub r5, r5, 1             ; vlmaccr r4[0]             }
      { add r4, r4, r8            ; bt r5, .twelve_l_top      }

    // 12 bytes -> 4 rgb pixels
    { mkmsk r8, _12             ; vlsat r11[0]              }

    vstrpv img_ptr[0], r8

    { mov r8, r6                ; bf r6, .end_loop          }

    // need to calculate row len - 12 - 12
    // so we can add 12 and then add row len - 12 - 12
    // to end up with just adding row len - 12
    // done for optimisation purposes only, sorry
    { sub r5, r7, _12           ; zext r8, 1                }

    // if odd only add 12 (4 rgb pixels)
    { add img_ptr, img_ptr, _12 ; bt r8, .four_l_top        }

    // + row len - 4 pixels
    { add img_ptr, img_ptr, r5  ; bu .four_l_top            }

  .end_loop:

  { sub img_ptr, img_ptr, r7  ; bt r3, .width_loop_top    }

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
