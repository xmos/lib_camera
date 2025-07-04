// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/*
void camera_rgb_to_greyscale16(int8_t * out_img, int8_t * img, size_t img_size)
*/
/*
The conversion formula is gs_pixel = 0.299 * R + 0.587 * G + 0.114 * B
0.299, 0.587 and 0.114 are 38, 75 and 14 in q7.

We start from the pixel 3 and going to pixel 0 by changing the coeffs accordingly,
then shift-sat the 4 pixels by 7.

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
.gs_sat:     .short 7, 7, 7, 7,      7, 7, 7, 7,     7, 7, 7, 7,      7, 7, 7, 7

#define NSTACKWORDS     4

.cc_top camera_rgb_to_greyscale16.func, camera_rgb_to_greyscale16
.align 4

#define gs_img          r0
#define img             r1
#define n_pix           r2
#define msk             r3

#define _24             r4
#define _16             r5
#define _72             r6

camera_rgb_to_greyscale16:
{ add r3, n_pix, 0            ; dualentsp NSTACKWORDS       }
  std r4, r5, sp[0]
{ ldc _24, 24                 ; stw r6, sp[2]               }

  ldc r11, 0x200
  ldc _72, 72
{ ldc _16, 16                 ; vsetc r11                   }
// n_pix must be a multiple of 16
{ add img, img, _24           ; zext r3, 4                  }
{ mkmsk msk, 16               ; ecallt r3                   }

.l16_top:

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

  { add gs_img, gs_img, _16     ; bt n_pix, .l16_top          }


{                             ; ldw r6, sp[2]               }
  ldd r4, r5, sp[0]
{                             ; retsp NSTACKWORDS           }

.cc_bottom camera_rgb_to_greyscale16.func

.size camera_rgb_to_greyscale16, .-camera_rgb_to_greyscale16
.globl camera_rgb_to_greyscale16
.type camera_rgb_to_greyscale16, @function
.set camera_rgb_to_greyscale16.nstackwords, NSTACKWORDS
.globl camera_rgb_to_greyscale16.nstackwords

#undef _24
#undef _16
#undef _72
#undef NSTACKWORDS

#define NSTACKWORDS     2

.cc_top camera_rgb_to_greyscale4.func, camera_rgb_to_greyscale4
.align 4

//      gs_img          r0
//      img             r1
//      n_pix           r2
//      msk             r3

#define _12             r4

camera_rgb_to_greyscale4:
{ add r3, n_pix, 0            ; dualentsp NSTACKWORDS       }
  std r4, r5, sp[0]

  ldc r11, 0x200
{                             ; vsetc r11                   }
  // n_pix must be a multiple of 4
{ ldc _12, 12                 ; zext r3, 2                  }
{ ecallt r3                   ; mkmsk msk, 4                }

.l4_top:

  { add img, img, _12           ; vldc img[0]                 }
  { ldap r11, .gs_coeffs3       ; vclrdr                      }

  { ldap r11, .gs_coeffs2       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs1       ; vlmaccr r11[0]              }
  { ldap r11, .gs_coeffs0       ; vlmaccr r11[0]              }
  { ldap r11, .gs_sat           ; vlmaccr r11[0]              }
  { sub n_pix, n_pix, 4         ; vlsat r11[0]                }

    vstrpv gs_img[0], msk

  { add gs_img, gs_img, 4       ; bt n_pix, .l4_top           }

  ldd r4, r5, sp[0]
{                             ; retsp NSTACKWORDS           }

.cc_bottom camera_rgb_to_greyscale4.func


.size camera_rgb_to_greyscale4, .-camera_rgb_to_greyscale4
.globl camera_rgb_to_greyscale4
.type camera_rgb_to_greyscale4, @function
.set camera_rgb_to_greyscale4.nstackwords, NSTACKWORDS
.globl camera_rgb_to_greyscale4.nstackwords

