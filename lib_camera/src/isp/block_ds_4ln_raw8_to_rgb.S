// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
void block_ds_4ln_raw8_to_rgb(int8_t * img_ptr, int8_t * tmp_buff, 
unsigned img_ln, unsigned img_width, unsigned tmp_width);
*/

.text
.issue_mode dual

#define FUNCTION_NAME   block_ds_4ln_raw8_to_rgb
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
#define _32             r4
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
{ shr r2, r2, 2             ; vsetc r11                 }

// offset the image pointer, get rid of img_ln
// (img_ptr) + (img_ln * img_width * 3)
mul r2, r2, r4
{ shr r3, img_width, 2      ; add img_ptr, img_ptr, r2  }

{ ldc _12, 12               ; ldw tmp_width, sp[STACK_TMP_WIDTH]}

{ add tmp_buff, tmp_buff, 8 ; ldc _32, 32               }

.width_loop_top:
  { ldc r6, 2                 ; vclrdr                    }

  // In rgb4, one 4x8 block will result in only 2 rgb pixels.
  // However, we can't just write 6 bytes from the vpu,
  // at least can't do it twice...
  // So will have to run 2 blocks so the vpu has 4 rgb pixels before storing.
  // If that's not complicated enough,
  // we'll have to do it backwards as well.. (because of how vlmaccr works)
  // So we have to start from the index 8, do [8:16] block first and then do the [0:8] one.
  // The inner loop will subtract 8, when the outer loop will add 32 to get it to 24
  // after we went down to -8
  .two_l_top:

    // construct the blocks 4x8
    // get samples from tmp_buff an put in the stack
    { mov r7, tmp_buff          ; sub tmp_buff, tmp_buff, 8 }

    ldd r8, r11, r7[0]
    std r8, r11, sp[STACK_TMP_VEC>>1]
    { add r7, r7, tmp_width     ; ldaw r5, sp[STACK_TMP_VEC]}

    ldd r8, r11, r7[0]
    std r8, r11, sp[STACK_TMP_VEC1>>1]
    { add r7, r7, tmp_width     ;  sub r6, r6, 1            }

    ldd r8, r11, r7[0]
    std r8, r11, sp[STACK_TMP_VEC2>>1]
    { add r7, r7, tmp_width     ; nop                       }

    ldd r8, r11, r7[0]
    std r8, r11, sp[STACK_TMP_VEC3>>1]

    ldaw r11, cp[kernels_rgb4] // keep long
    { mov r7, r11               ; vldc r5[0]                }

    // six vlmaccrs -> 2 rgb pixels
    { add r7, r7, _32           ; vlmaccr r7[0]             }
    { add r7, r7, _32           ; vlmaccr r7[0]             }
    { add r7, r7, _32           ; vlmaccr r7[0]             }
    { add r7, r7, _32           ; vlmaccr r7[0]             }
    { add r7, r7, _32           ; vlmaccr r7[0]             }
    { add r7, r7, _32           ; vlmaccr r7[0]             }

    { bt r6, .two_l_top         ; nop                       }

  ldaw r11, cp[kernel_rgb4_shifts] // keep long
  // 12 bytes -> 4 rgb pixels
  { mkmsk r8, _12             ; vlsat r11[0]              }

  vstrpv img_ptr[0], r8

  { sub r3, r3, 1             ; add tmp_buff, tmp_buff, _32}
  { add img_ptr, img_ptr, _12 ; bt r3, .width_loop_top    }

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
