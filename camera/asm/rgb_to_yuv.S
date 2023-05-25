/*
Converts signed yuv (-127..127, -127..127, -127..127) into signed rgb. 
Returns an int with 3 x int8_t
  int rgb_to_yuv(
      int r, 
      int g, 
      int b);

*/

.cc_top rgb_to_yuv.func, rgb_to_yuv
.align 4
.globl rgb_to_yuv
.globl rgb_to_yuv.nstackwords
.linkset rgb_to_yuv.nstackwords, 4
.issue_mode dual


#define r   r0
#define g   r1
#define b   r2

rgb_to_yuv:
{ ldc r11, 0                  ; dualentsp 8                 }   // Enable dual instructions
{ ldc r3, 22                  ; vsetc r11                   }   // Load 22 in R3 ; control register and the headroom in the vector unit. Signed 32-bit integer
{ shl r, r, r3                ; ldaw r11, sp[0]             }   // Multiply by 2^22; load word in the Sp

{ shl g, g, r3                ; stw r, r11[0]               }   // Multiply by 2^22; store y in the vpu
{ shl b, b, r3                ; stw g, r11[1]               }   // Multiply by 2^22; store u in the vpu
{ ldc r3, 32                  ; stw b, r11[2]               }   // Load constant;   store v in the vpu

#undef y
#undef u
#undef v

{ ldc r1, 24                  ; vclrdr                      }   // load in R1 24 ; Sets the contents of vD and vR in the vector unit to all zeroes.
{ neg r1, r1                  ; vldc r11[0]                 }   // signed negation ; load first opdfand
ldap r11, Yconv                                               // Load effective address relative to the program counter.
{ add r11, r11, r3            ; vlmaccr r11[0]              }   
{ add r11, r11, r3            ; vlmaccr r11[0]              }
{ ldaw r11, sp[0]             ; vlmaccr r11[0]              }

{                             ; vstr r11[0]                 }
  vlashr r11[0], r1
{                             ; vdepth8                     }
{                             ; vstr r11[0]                 }
{                             ; ldw r0, sp[0]               }
retsp 8

// the values correspond to the matrix values *255
/*
Yconv: .word 157,  -131,   -26, 0, 0,0,0,0    
grnConv: .word -38,  -74,   111, 0, 0,0,0,0
redConv: .word  76,  150,    29, 0, 0,0,0,0  
*/
Yconv: .word 128,  -107,   -21, 0, 0,0,0,0    
Uconv: .word -43,  -84,   128, 0, 0,0,0,0
Vconv: .word  76,  150,    29, 0, 0,0,0,0  

.cc_bottom rgb_to_yuv.func


// References
// https://en.wikipedia.org/wiki/YUV
// https://softpixel.com/~cwright/programming/colorspace/yuv/
// https://www.mikekohn.net/file_formats/yuv_rgb_converter.php
//