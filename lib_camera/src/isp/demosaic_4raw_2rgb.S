// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
void demosaic_4raw_2rgb(int8_t * img_ptr, int8_t * tmp_buff, 
unsigned img_ln, unsigned img_width, unsigned tmp_width);
*/

.text
.issue_mode dual

#define FUNCTION_NAME   demosaic_4raw_2rgb
#define NSTACKWORDS     20

#define STACK_TMP_WIDTH (NSTACKWORDS + 1)
#define STACK_TMP_VEC   (NSTACKWORDS - 8)
#define STACK_TMP_VEC1  (STACK_TMP_VEC + 2)
#define STACK_TMP_VEC2  (STACK_TMP_VEC + 4)
#define STACK_TMP_VEC3  (STACK_TMP_VEC + 6)
#define STACK_TMP_OUTS  (STACK_TMP_VEC - 4)
#define STACK_TMP_OUTS1 (STACK_TMP_OUTS + 2)

#define img_ptr         r0
#define tmp_buff        r1
#define tmp_width       r2
#define img_width       r3
//#define row_len         r4
#define out_ptr_arr     r8
#define _12             r9

.cc_top FUNCTION_NAME.func, FUNCTION_NAME
.align 4

FUNCTION_NAME:

dualentsp NSTACKWORDS

std r4, r5, sp[0]
std r6, r7, sp[1]
std r8, r9, sp[2]

ldc _12, 12

// offset the image pointer, get rid of img_ln
ldc r11, 3
mul r4, img_width, r11 // row width
mul r2, r2, r4
add img_ptr, img_ptr, r2 // (img_ptr) + (img_ln * img_width * 3)

ldw tmp_width, sp[STACK_TMP_WIDTH]

// prep output array of pointers
add r4, img_ptr, r4 // this is for next row
add r11, img_ptr, _12 // 3 * 4 which is 4 rgb pixels
std r11, img_ptr, sp[STACK_TMP_OUTS>>1]
add r11, r4, _12
std r11, r4, sp[STACK_TMP_OUTS1>>1]
ldaw out_ptr_arr, sp[STACK_TMP_OUTS]

// prep vpu
ldc r11, 0x200
vsetc r11
vclrdr

// prep main loop
shr r3, img_width, 3
// need to keep r3 after this point

.width_loop_top:

  // construct the blocks 4x8
  // get samples from tmp_buff an put in the stack
  // accumulate tmp_buff ptr by 8
  //{ mov r4, tmp_buff          ; add tmp_buff, tmp_buff, 8 }
  mov r4, tmp_buff
  add tmp_buff, tmp_buff, 8

  ldd r0, r11, r4[0]
  std r0, r11, sp[STACK_TMP_VEC>>1]
  add r4, r4, tmp_width

  ldd r0, r11, r4[0]
  std r0, r11, sp[STACK_TMP_VEC1>>1]
  add r4, r4, tmp_width

  ldd r0, r11, r4[0]
  std r0, r11, sp[STACK_TMP_VEC2>>1]
  add r4, r4, tmp_width

  ldd r0, r11, r4[0]
  std r0, r11, sp[STACK_TMP_VEC3>>1]

  ldaw r4, sp[STACK_TMP_VEC]
  vldc r4[0]

  ldaw r11, cp[kernels12]
  mov r4, r11
  ldaw r11, cp[kernel_shifts]
  // need to keep r11, r4 after this point

  ldc r0, 0
  .four_l_top:
    mov r5, _12
    ldc r6, 32
    .twelve_l_top:

      { sub r5, r5, 1             ; vlmaccr r4[0]             }
      { add r4, r4, r6            ; bt r5, .twelve_l_top      }

    // 12 bytes -> 4 rgb pixels
    { mkmsk r6, _12             ; vlsat r11[0]              }

    ldw r5, out_ptr_arr[r0]
    vstrpv r5[0], r6
    // acc by 8 rgb pixels
    ldaw r5, r5[6]
    stw r5, out_ptr_arr[r0]

    { add r0, r0, 1             ; vclrdr                    }

    eq r5, r0, 4
    bf r5, .four_l_top

  sub r3, r3, 1
  bt r3, .width_loop_top

ldd r4, r5, sp[0]
ldd r6, r7, sp[1]
ldd r8, r9, sp[2]

retsp NSTACKWORDS

.cc_bottom FUNCTION_NAME.func


.size FUNCTION_NAME, .-FUNCTION_NAME
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.set FUNCTION_NAME.nstackwords, NSTACKWORDS
.globl FUNCTION_NAME.nstackwords
