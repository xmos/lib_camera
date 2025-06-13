// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/******************************************************

 void raw8_block_to_yuv422(
    int8_t *dst,
    int8_t *src,
    unsigned line_size
 );

Descr:
 Converts a block of raw8 data to YUV422 format.
 Each two horizontal RAW8 pixels RGGB are converted to a single YUYV pixel pair (Y0 U Y1 V). 

Parameters:
   dst       - destination pointer for YUV422 data (1x8x2 YU, YV block)
   src       - input raw8 source pointer (2x16x1 raw8 block)
   line_size - size of the line in bytes

Note:
    VPU has to be set to 8-bit mode before calling this function.
    Coefficient derive from (BT.601) in fix point, adding compensation for white balancing. 

********************************************************/


#define FUNCTION_NAME   raw8_block_to_yuv422
#define NSTACKWORDS     2
#define dst             r0
#define src             r1
#define lns             r2 // line size
#define msk             r3

#define _16             r4      
#define _32             r5

.section .text
.issue_mode dual
.align 4
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

FUNCTION_NAME:
    DUALENTSP_lu6   NSTACKWORDS
    // init
    std r4, r5, sp[0]
    // make block 2x16 in dst, load dst to vc
    {ldc _32, 32; ldc _16, 16}
    mkmsk msk, _16
    mov r11, src
    vldr r11[0]
    vstrpv dst[0], msk
    {add dst, dst, _16; add r11, r11, lns}
    vldr r11[0]
    vstrpv dst[0], msk
    {vclrdr; sub dst, dst, _16}
    vldc dst[0]    
    // loop
    ldaw r11, cp[kernels_yuv422]
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    {vlmaccr  r11[0]; add r11, r11, _32}
    ldaw r11, cp[kernels_yuv422_sat]
    vlsat r11[0]
    ldaw r11, cp[kernels_yuv422_adds]
    vladd r11[0]
    vstrpv dst[0], msk  // result
    // restore regs
    ldd  r4, r5, sp[0]
    RETSP_lu6 NSTACKWORDS

.cc_bottom FUNCTION_NAME.func
.set    FUNCTION_NAME.nstackwords, NSTACKWORDS; .globl  FUNCTION_NAME.nstackwords
.set    FUNCTION_NAME.maxcores,1; .global FUNCTION_NAME.maxcores
.set	FUNCTION_NAME.maxtimers,0; .globl FUNCTION_NAME.maxtimers
.set	FUNCTION_NAME.maxchanends,0; .globl	FUNCTION_NAME.maxchanends
.size FUNCTION_NAME, .-FUNCTION_NAME
