// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*

 void raw8_block_to_yuv422(
    int8_t *dst,
    int8_t *src,
    unsigned img_width
    unsigned line_size
 );

Note: 
Coefficient derive from (BT.601) in fix point, adding compensation for white balancing. 

*/

.text
.issue_mode dual

#define FUNCTION_NAME   raw8_block_to_yuv422
#define NSTACKWORDS     14
#define STACK_VEC       (NSTACKWORDS - 12)
#define dst             r0
#define src             r1
#define img_width       r2
#define lns             r3 // line size

#define _16             r4
#define _32             r5

.align 16
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

FUNCTION_NAME:
{ shr r2, img_width, 3    ; dualentsp NSTACKWORDS   }

std r4, r5, sp[0]

ldc r11, 0x200
{ ldc _16, 16             ; vsetc r11               }
{                         ; vclrdr                  }

.loop_top:

  // make block 2x16 is stack, then load to vC
  { mov r5, src             ; ldaw r11, sp[STACK_VEC] }
  { add r5, r5, lns         ; vldc r5[0]              }
  { add r11, r11, _16       ; vstc r11[0]             }
  { add src, src, _16       ; vldc r5[0]              }
  { sub r11, r11, _16       ; vstc r11[0]             }

  { ldc _32, 32             ; vldc r11[0]             }
  
  // unrolled loop
  ldaw r11, cp[kernels_yuv422]
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  { vlmaccr  r11[0]         ; add r11, r11, _32       }
  ldaw r11, cp[kernels_yuv422_sat]
  {                         ; vlsat r11[0]            }
  ldaw r11, cp[kernels_yuv422_adds]
  { mkmsk r5, _16           ; vladd r11[0]            }
  vstrpv dst[0], r5  // result

  { sub r2, r2, 1           ; vclrdr                  }
  { add dst, dst, _16       ; bt r2, .loop_top        }

ldd r4, r5, sp[0]

retsp NSTACKWORDS

.cc_bottom FUNCTION_NAME.func
.set    FUNCTION_NAME.nstackwords, NSTACKWORDS; .globl  FUNCTION_NAME.nstackwords
.set    FUNCTION_NAME.maxcores,1; .global FUNCTION_NAME.maxcores
.set	FUNCTION_NAME.maxtimers,0; .globl FUNCTION_NAME.maxtimers
.set	FUNCTION_NAME.maxchanends,0; .globl	FUNCTION_NAME.maxchanends
.size FUNCTION_NAME, .-FUNCTION_NAME
