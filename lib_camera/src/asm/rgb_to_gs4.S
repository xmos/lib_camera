// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.


/*
void isp_rgb_to_greyscale4(int8_t * out_img, int8_t * img, size_t img_size)
*/
/*
The conversion formula is gs_pixel = 0.299 * R + 0.587 * G + 0.114 * B
0.299, 0.587 and 0.114 are 38, 75 and 14 in q7.

We start from the pixel 3 and going to pixel 0 by changing the coeffs accordingly,
then shift-sat the 4 pixels by 7.

They are in the text section so we can use ldap which can be used in a short u10 M+R form, 
which is essential as all VPU instructions are memory lane only.

P.S. On vx4a VPU will not require word-alignment, so can use less memory.
*/

.text
.issue_mode dual

.align 4
.gs_coeffs3: .byte  0, 0, 0, 0,      0, 0, 0, 0,     0, 38, 75, 14,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
.gs_coeffs2: .byte  0, 0, 0, 0,      0, 0, 38, 75,   14, 0, 0, 0,     0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
.gs_coeffs1: .byte  0, 0, 0, 38,     75, 14, 0, 0,   0, 0, 0, 0,      0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
.gs_coeffs0: .byte  38, 75, 14, 0,   0, 0, 0, 0,     0, 0, 0, 0,      0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
.gs_sat:     .short 7, 7, 7, 7,      0, 0, 0, 0,     0, 0, 0, 0,      0, 0, 0, 0

#define FUNC_NAME       isp_rgb_to_greyscale4
#define NSTACKWORDS     2

.cc_top FUNC_NAME.func, FUNC_NAME
.align 4

#define gs_img          r0
#define img             r1
#define n_pix           r2
#define msk             r3
#define _12             r4
#define coeff           r11

FUNC_NAME:
{ add r3, n_pix, 0            ; dualentsp NSTACKWORDS       }
  std r4, r5, sp[0]

  ldc r11, 0x200
{                             ; vsetc r11                   }
  // n_pix must be a multiple of 4
{ ldc _12, 12                 ; zext r3, 2                  }
{ ecallt r3                   ; mkmsk msk, 4                }

.l_top:

  { add img, img, _12           ; vldc img[0]                 }
  { ldap r11, .gs_coeffs3       ; vclrdr                      }

  { ldap r11, .gs_coeffs2       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs1       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs0       ; vlmaccr r11[0]              }
  { ldap r11, .gs_sat           ; vlmaccr r11[0]              }
  { sub n_pix, n_pix, 4         ; vlsat r11[0]                }

    vstrpv gs_img[0], msk

  { add gs_img, gs_img, 4       ; bt n_pix, .l_top            }

  ldd r4, r5, sp[0]
{                             ; retsp NSTACKWORDS           }

.size FUNC_NAME, .-FUNC_NAME
.cc_bottom FUNC_NAME.func

.globl FUNC_NAME
.type FUNC_NAME, @function
.set FUNC_NAME.nstackwords, NSTACKWORDS
.globl FUNC_NAME.nstackwords
