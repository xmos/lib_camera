// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.


// the cpu version
/*
void isp_rgb_to_greyscale(int8_t * out_img, int8_t * img, size_t img_size)
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
.gs_sat:     .short 7, 7, 7, 7,      7, 7, 7, 7,     7, 7, 7, 7,      7, 7, 7, 7

#define FUNC_NAME       isp_rgb_to_greyscale16
#define NSTACKWORDS     4

.cc_top FUNC_NAME.func, FUNC_NAME
.align 4

#define gs_img          r0
#define img             r1
#define n_pix           r2
#define msk             r3
#define _24             r4
#define _16             r5
#define _72             r6

FUNC_NAME:
{ add r3, n_pix, 0            ; dualentsp NSTACKWORDS       }
  std r4, r5, sp[0]
{ ldc _24, 24                 ; stw r6, sp[2]               }

  ldc r11, 0x200
  ldc _72, 72
{ ldc _16, 16                 ; vsetc r11                   }
// n_pix must be a multiple of 16
{ add img, img, _24           ; zext r3, 4                  }
{ mkmsk msk, 16               ; ecallt r3                   }

.l_top:

  { sub img, img, _24           ; vldc img[0]                 }
  { ldap r11, .gs_coeffs7       ; vclrdr                      }

  { ldap r11, .gs_coeffs6       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs5       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs4       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs3       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs2       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs1       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs0       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs7       ; vlmaccr r11[0]              }

  { add img, img, _72           ; vldc img[0]                 }
  { ldap r11, .gs_coeffs6       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs5       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs4       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs3       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs2       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs1       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs0       ; vlmaccr r11[0]              }
  { ldap r11, .gs_sat           ; vlmaccr r11[0]              }
  { sub n_pix, n_pix, _16       ; vlsat r11[0]                }

    vstrpv gs_img[0], msk

  { add gs_img, gs_img, _16     ; bt n_pix, .l_top            }


{                             ; ldw r6, sp[2]               }
  ldd r4, r5, sp[0]
  retsp NSTACKWORDS

.size FUNC_NAME, .-FUNC_NAME
.cc_bottom FUNC_NAME.func

.globl FUNC_NAME
.type FUNC_NAME, @function
.set FUNC_NAME.nstackwords, NSTACKWORDS
.globl FUNC_NAME.nstackwords