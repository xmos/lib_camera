// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.


// the cpu version
/*
void isp_rgb_to_greyscale8(int8_t * out_img, int8_t * img, size_t img_size)
*/
/*
Sooo, for the coefficients:

They are in the text section so we can use ldap which can be used in a short u10 M+R form, 
which is essential as all VPU instructions are memory lane only.

Coefficients are interleaved with each other for the memory reuse.
P.S. On vx4a VPU will not require word-alignment, so can use even less memory.
*/

.text
.issue_mode dual

.align 4
.gs_coeffs7: .byte  0, 0, 0, 0,      0, 0, 0, 0,     0, 0, 0, 0
.gs_coeffs3: .byte  0, 0, 0, 0,      0, 0, 0, 0,     0, 38, 75, 14,   0, 0, 0, 0,   0, 0, 0, 0
.gs_coeffs6: .byte  0, 0, 0, 0,      0, 0, 0, 0,     0, 0, 0, 0
.gs_coeffs2: .byte  0, 0, 0, 0,      0, 0, 38, 75,   14, 0, 0, 0,     0, 0, 0, 0,   0, 0, 0, 0
.gs_coeffs5: .byte  0, 0, 0, 0,      0, 0, 0, 0,     0, 0, 0, 0
.gs_coeffs1: .byte  0, 0, 0, 38,     75, 14, 0, 0,   0, 0, 0, 0,      0, 0, 0, 0,   0, 0, 0, 0
.gs_coeffs4: .byte  0, 0, 0, 0,      0, 0, 0, 0,     0, 0, 0, 0
.gs_coeffs0: .byte  38, 75, 14, 0,   0, 0, 0, 0,     0, 0, 0, 0,      0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
.gs_sat:     .short 7, 7, 7, 7,      7, 7, 7, 7,     0, 0, 0, 0,      0, 0, 0, 0

#define FUNC_NAME       isp_rgb_to_greyscale8
#define NSTACKWORDS     2

.cc_top FUNC_NAME.func, FUNC_NAME
.align 4

#define gs_img          r0
#define img             r1
#define n_pix           r2
#define msk             r3
#define _24             r4

FUNC_NAME:
{ add r3, n_pix, 0            ; dualentsp NSTACKWORDS       }
  std r4, r5, sp[0]

  ldc r11, 0x200
{                             ; vsetc r11                   }
// n_pix must be a multiple of 8
{ ldc _24, 24                 ; zext r3, 3                  }
{ ecallt r3                   ; mkmsk msk, 8                }

.l_top:

  { add img, img, _24           ; vldc img[0]                 }
  { ldap r11, .gs_coeffs7       ; vclrdr                      }

  { ldap r11, .gs_coeffs6       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs5       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs4       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs3       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs2       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs1       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs0       ; vlmaccr r11[0]              }
  { ldap r11, .gs_sat           ; vlmaccr r11[0]              }
  { sub n_pix, n_pix, 8         ; vlsat r11[0]                }

    vstrpv gs_img[0], msk

  { add gs_img, gs_img, 8       ; bt n_pix, .l_top            }

  ldd r4, r5, sp[0]
{                             ; retsp NSTACKWORDS           }

.size FUNC_NAME, .-FUNC_NAME
.cc_bottom FUNC_NAME.func

.globl FUNC_NAME
.type FUNC_NAME, @function
.set FUNC_NAME.nstackwords, NSTACKWORDS
.globl FUNC_NAME.nstackwords
