
#include <xs1.h>
#include <xs3a_registers.h>

.issue_mode dual

#define FUNCTION_NAME   pixel_hfilter
#define NSTACKWORDS     8

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

void pixel_hfilter(
    int8_t output[],
    const int8_t input[],
    const int8_t coef[32],
    const int16_t acc_hi[16],
    const int16_t acc_lo[16],
    const int16_t acc_shr[16],
    const int32_t input_stride,
    const unsigned output_count);

 ****************************************************
 ****************************************************
*/

#define STK_ACC_LO    (NSTACKWORDS+1)
#define STK_OUT_SHR   (NSTACKWORDS+2)
#define STK_IN_STR    (NSTACKWORDS+3)
#define STK_OUT_LEN   (NSTACKWORDS+4)

#define output    r0
#define input     r1
#define coef      r2
#define acc_hi    r3
// #define acc_lo    r4
#define out_shr   r5
#define in_str    r6
#define len       r7
#define _16       r8
#define mask      r9


.align 4
.skip 0
FUNCTION_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[1]
  std r6, r7, sp[2]
  std r8, r9, sp[3]

  ldc r11, 0x200
{ ldc _16, 16                 ; vsetc r11                   }
{ mkmsk mask, 4               ; ldw len, sp[STK_OUT_LEN]    }
{ and mask, len, mask         ; ldw r11, sp[STK_ACC_LO]     }
// if len isn't a multiple of 16 we're gonna do a bad thing. Fix this later. 
{ ecallt mask                 ; ldw out_shr, sp[STK_OUT_SHR]}
{ mkmsk mask, 16              ; ldw in_str, sp[STK_IN_STR]  }
// Start at the end so we can proceed monotonically
//    input <-- input + (len*in_str) 
//    output <-- output + (len)
{ add output, output, len     ; vldc coef[0]                }
maccu coef, input, len, in_str

// We subtract at the beginning of the loop so it should work out correctly.

.L_loop_top:
  { sub input, input, in_str    ; vldd acc_hi[0]              }
  { sub output, output, _16     ; vldr r11[0]                 }
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            }     
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            }     
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            }     
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  { sub input, input, in_str    ; vlmaccr input[0]            } 
  {                             ; vlmaccr input[0]            }
  { sub len, len, _16           ; vlsat out_shr[0]            }
    vstrpv output[0], mask
  {                             ; bt len, .L_loop_top         }



  ldd r8, r9, sp[3]
  ldd r6, r7, sp[2]
  ldd r4, r5, sp[1]
  retsp NSTACKWORDS

.size FUNCTION_NAME, .-FUNCTION_NAME
.cc_bottom FUNCTION_NAME.func



