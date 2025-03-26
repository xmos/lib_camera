/* *****************************************************
void wb_scale_image(
    int8_t *img,
    unsigned img_size,
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

********************************************************/

#define FUNCTION_NAME   wb_scale_image
#define NSTACKWORDS     (4)            
#define STACK_VSAT      (NSTACKWORDS + 1)

#define img             r0
#define img_size        r1
#define vk              r2
#define vc              r3

#define vsat            r4
#define _12             r5      
#define _32             r6
#define cnt             r7

#define msk             r11

.text
.issue_mode single
.align 4
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

FUNCTION_NAME:
    ENTSP_lu6   NSTACKWORDS
    std r4, r5, sp[0]
    std r6, r7, sp[1]
    ldw vsat, sp[STACK_VSAT]
// Set Vpu
    vclrdr
    ldc     r11, 0x200
    vsetc   r11
// Constants
    ldc     _32, 32
    ldc     _12, 12
    mkmsk   msk, _12
    sub img_size, img_size, _32
    divu cnt, img_size, _12
    vldc    vc[0]
.do_wb_loop:
    vlmacc  img[0]
    vlmacc  vk[0]
    vlsat   vsat[0]   
    vstrpv  img[0], msk
    vclrdr
    add img, img, _12
    sub cnt, cnt, 1
    bt cnt, .do_wb_loop
// Restore regs
    ldd   r4, r5, sp[0]
    ldd   r6, r7, sp[1]
    retsp   NSTACKWORDS     

.cc_bottom FUNCTION_NAME.func
.set    FUNCTION_NAME.nstackwords, NSTACKWORDS; .globl  FUNCTION_NAME.nstackwords
.set    FUNCTION_NAME.maxcores,1; .global FUNCTION_NAME.maxcores
.set	FUNCTION_NAME.maxtimers,0; .globl FUNCTION_NAME.maxtimers
.set	FUNCTION_NAME.maxchanends,0; .globl	FUNCTION_NAME.maxchanends
.size FUNCTION_NAME, .-FUNCTION_NAME
