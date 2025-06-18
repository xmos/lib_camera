
/*

void camera_int8_to_uint8_fast(
    uint8_t *dst, 
    const int8_t *src, 
    unsigned size
);

//TODO ensure data is aligned8
//TODO ensure data >= 8

*/

#define FUNCTION_NAME   camera_int8_to_uint8_fast
#define NSTACKWORDS     4
#define dst             r0
#define src             r1
#define len             r2
#define rem             r3

#define tmp1            r4      
#define tmp2            r5
#define idx             r6
#define aux             r7

#define mask            r11

.section .cp.rodata,"ac",@progbits
.issue_mode dual
.align 4
cp_xor: .word 0x80808080

.section .text
.issue_mode dual
.align 4
.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

FUNCTION_NAME:
    DUALENTSP_lu6   NSTACKWORDS
    std r4, r5, sp[0]
    std r6, r7, sp[1]
    {ldc idx, 0x0; ldc aux, 0x7}
    {and rem, len, aux; shr len, len, 3}
    ldw mask, cp[cp_xor]
.do_loop:
    ldd tmp1, tmp2, src[idx]
    xor tmp1, tmp1, mask
    xor tmp2, tmp2, mask
    std tmp1, tmp2, dst[idx]
    {add idx, idx, 1; sub len, len, 1}
    bt  len, .do_loop
// prepare remainder
    ldc aux, 0x80
    shl idx, idx, 3
    bf rem, .end
.do_remainder:
    ld8u tmp2, src[idx]
    xor tmp2, tmp2, aux
    st8 tmp2, dst[idx]
    {sub rem, rem, 1; add idx, idx, 1}
    bt rem, .do_remainder
.end:
    ldd r4, r5, sp[0]
    ldd r6, r7, sp[1]
    RETSP_lu6 NSTACKWORDS

.cc_bottom FUNCTION_NAME.func
.set    FUNCTION_NAME.nstackwords, NSTACKWORDS; .globl  FUNCTION_NAME.nstackwords
.set    FUNCTION_NAME.maxcores,1; .global FUNCTION_NAME.maxcores
.set	FUNCTION_NAME.maxtimers,0; .globl FUNCTION_NAME.maxtimers
.set	FUNCTION_NAME.maxchanends,0; .globl	FUNCTION_NAME.maxchanends
.size FUNCTION_NAME, .-FUNCTION_NAME
