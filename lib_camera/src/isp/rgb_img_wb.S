// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* *****************************************************
void wb_scale_image(
    image_cfg_t* image,
    const int8_t vk[32], 
    const int8_t vc[32],
    const int16_t vsat[16]
);

Descr:

    This function scales the image by applying a white balance correction.
    The image is processed in blocks of 12 pixels at a time, and the scaling
    is done using vector operations. 

Pseudo Operation:
    
    pos = 0;
    for i in range(0, img_size - 32, 1):
        img[i] = (vc[pos] * img[i] + vc[pos] * vk[pos]) >> sat[pos];
        pos = (pos + 1) % 3;

    (in reallity it is done in blocks of 12 pixels)

Where:
    - pos = 0 if Red, 1 if Green, 2 if Blue
    - vk = (128 - 128/factor) (uint16_t)
    - vc = factor << sat (uint16_t)
    - factor = scaling factor for the white balance correction (float32) [1-2]
    - Default values are [1.538, 1.0, 1.587] for RGB

********************************************************/

#define FUNCTION_NAME   wb_scale_image
#define NSTACKWORDS     (4)            

#define img             r0
#define vk              r1
#define loops           r2
#define vsat            r3

#define rem             r4    
#define msk             r5

#define _12             r11

.text
.issue_mode dual
.align 4
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

FUNCTION_NAME:
    dualentsp   NSTACKWORDS
    std r4, r5, sp[0]
    ldw r5, r0[0]                   // height
    ldw r4, r0[1]                   // width 
    mul r4, r4, r5                  // size
    {shr loops, r4, 2; vldc r2[0]}     
    {zext r4, 2; ldw img, r0[4]}    // img_ptr
    ldc     r11, 0x200
    {shl r5, r4, 1; vclrdr}
    {ldc r11, 12; vsetc   r11}
    {mkmsk   msk, r11; add  r4, r4, r5}
.do_wb_loop:
    {vlmacc  img[0]; sub loops, loops, 1}
     vlmacc  vk[0]
     vlsat   vsat[0]   
     vstrpv  img[0], msk
    {vclrdr; add img, img, _12} 
    bt loops, .do_wb_loop
// tail
    {mkmsk r2, rem; bf rem, .end}
    vlmacc  img[0]
    vlmacc  vk[0]
    vlsat   vsat[0]   
    vstrpv  img[0], r2
.end:
    ldd   r4, r5, sp[0]
    retsp   NSTACKWORDS

.cc_bottom FUNCTION_NAME.func
.set    FUNCTION_NAME.nstackwords, NSTACKWORDS; .globl  FUNCTION_NAME.nstackwords
.set    FUNCTION_NAME.maxcores,1; .global FUNCTION_NAME.maxcores
.set	FUNCTION_NAME.maxtimers,0; .globl FUNCTION_NAME.maxtimers
.set	FUNCTION_NAME.maxchanends,0; .globl	FUNCTION_NAME.maxchanends
.size FUNCTION_NAME, .-FUNCTION_NAME
    
