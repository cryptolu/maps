#include <stdint.h>
#include "arm_macros.h"

#define CR "\n\t"

void experiment(uint16_t *buffer, uint32_t *rk_masked)
{
	/* r2: rv, r3: rm, r4: lv, r5: lm */
	asm volatile (
		"ldr pc,=_start" CR
		".space 452" CR
		"_start:" CR
		/* sec_simon_v04 */
        "ldm %[buffer], {r2-r5}" CR
        "mov r10, #44" CR
        "simon_enc_step:" CR
        "ror r6, r4, 31" CR
        "ror r7, r5, 31" CR
        "and r8, r6, r4, ror 24" CR
        "orn r6, r6, r5, ror 24" CR
        "eor r6, r6, r8" CR
        "mov r8, #0" CR
        "and r8, r7, r4, ror 24" CR
        "orn r9, r7, r5, ror 24" CR
        "eor r7, r9, r8" CR
        "eor r2, r6" CR
        "eor r3, r7" CR
        "eor r2, r2, r4, ror 30" CR
        "eor r3, r3, r5, ror 30" CR
		"ldr r7, [%[rk_masked], 4]" CR
        "ldm %[rk_masked]!, {r6, r7}" CR
        "eor r2, r2, r6" CR
        "eor r3, r3, r7" CR
        "eor r2, r2, r4" CR
        "eor r4, r4, r2" CR
        "eor r2, r2, r4" CR
        "eor r3, r3, r5" CR
        "subs r10, r10, #1" CR
        "eor r5, r5, r3" CR
        "eor r3, r3, r5" CR
        "bne simon_enc_step" CR
        "stm %[buffer], {r2-r5}" CR

		/* reset rk_masked to initial value */
		"sub %[rk_masked], #352" CR

		/* sec_rectangle_v04 */
		/* Registers allocation:
			r2 : row0v
			r3 : row1v
			r4 : row2v
			r5 : row3v
			r6 : row0m
			r7 : row1m
			r8 : row2m
			r9 : row3m
			r10: rkv/temp
			r11: rkm/temp
			r12: temp
			r14: loop counter
		 */
		/* load block */
		"ldm %[buffer], {r2, r4, r6, r8}" CR
		"mov r3, r2, lsr #16" CR
		"mov r5, r4, lsr #16" CR
		"mov r7, r6, lsr #16" CR
		"mov r9, r8, lsr #16" CR
		"movt r2, #0" CR
		"movt r4, #0" CR
		"movt r6, #0" CR
		"movt r8, #0" CR
		"push {%[buffer]}" CR
		/* initialize loop */
		"mov r14, 25" CR
		"rectangle_enc_step:" CR
		/* load key */
		"ldm %[rk_masked]!, {r10, r11}" CR
		"eor r2, r2, r10" CR
		"eor r3, r3, r10, lsr #16" CR
		"eor r4, r4, r11" CR
		"eor r5, r5, r11, lsr #16" CR
		"ldm %[rk_masked]!, {r10, r11}" CR
		"eor r6, r6, r10" CR
		"eor r7, r7, r10, lsr #16" CR
		"eor r8, r8, r11" "\n\t"
		"eor r9, r9, r11, lsr #16" CR
		/* S-layer */
		"push {r14}" CR
		"bic r10, r5, r3" CR
		"orr %[buffer], r5, r7" CR
		"eor r10, r10, %[buffer]" CR
		"and r12, r9, r7" CR
		"mov %[buffer], 0" CR /* remove leakage for next instruction */
		"orn %[buffer], r9, r3" CR
		"eor r12, r12, %[buffer]" CR
		"eor r10, r10, r2" CR
		"eor r12, r12, r6" CR
		"bic %[buffer], r2, r3" CR
		"orn r2, r2, r7" CR
		"eor r2, r2, %[buffer]" CR
		"mov %[buffer], 0" CR /* remove leakage for next instruction */
		"bic %[buffer], r6, r3" CR
		"orn r6, r6, r7" CR
		"eor r6, r6, %[buffer]" CR
		"eor r11, r4, r5" CR
		"eor r14, r8, r9" CR
		"eor r2, r2, r11" CR
		"eor r6, r6, r14" CR
		"eor r5, r3, r4" CR
		"eor r9, r7, r8" CR
		"eor r3, r4, r10" CR
		"eor r7, r8, r12" CR
		"push {r11}" CR
		"mov %[buffer], 0" CR /* remove leakage for next instruction */
		"and %[buffer], r10, r11" CR
		"mov r11, 0" CR /* remove leakage for next instruction */
		"orn r11, r10, r14" CR
		"eor r11, r11, %[buffer]" CR
		"mov %[buffer], 0" CR /* remove leakage for next instruction */
		"orn %[buffer], r12, r14" CR
		"mov r14, 0" CR /* remove leakage for next instruction */
		"pop {r14}" CR
		"and r14, r12, r14" CR
		"eor r14, r14, %[buffer]" CR
		"eor r5, r5, r11" CR
		"eor r9, r9, r14" CR
		"mov r4, 0" CR /* remove leakage for next instruction */
		"and r4, r2, r5" CR
		"mov %[buffer], 0" CR /* remove leakage for next instruction */
		"orr %[buffer], r2, r9" CR
		"eor r4, r4, %[buffer]" CR
		"mov r8, 0" CR /* remove leakage for next instruction */
		"and r8, r6, r9" CR
		"mov %[buffer], 0" CR /* remove leakage for next instruction */
		"orr %[buffer], r6, r5" CR
		"eor r8, r8, %[buffer]" CR
		"eor r4, r4, r10" CR
		"eor r8, r8, r12" CR
		"pop {r14}" CR
		/* P-layer */
		"bfi r3, r3, #16, #15" CR
		"ror r3, r3, #15" CR
		"bfi r7, r7, #16, #15" CR
		"ror r7, r7, #15" CR
		"bfi r4, r4, #16, #4" CR
		"ror r4, r4, #4" CR
		"bfi r8, r8, #16, #4" CR
		"ror r8, r8, #4" CR
		"bfi r5, r5, #16, #3" CR
		"ror r5, r5, #3" CR
		"bfi r9, r9, #16, #3" CR
		"ror r9, r9, #3" CR
		/* loop end */
		"subs r14, r14, #1" CR
		"bne rectangle_enc_step" CR
		"pop {%[buffer]}" CR
		/* apply last round key */
		"ldm %[rk_masked]!, {r10, r11}" CR
		"eor r2, r2, r10" CR
		"eor r3, r3, r10, lsr #16" CR
		"eor r4, r4, r11" CR
		"eor r5, r5, r11, lsr #16" CR
		"ldm %[rk_masked]!, {r10, r11}" CR
		"eor r6, r6, r10" CR
		"eor r7, r7, r10, lsr #16" CR
		"eor r8, r8, r11" CR
		"eor r9, r9, r11, lsr #16" CR
		/* store results */
		"bfi r2, r3, #16, #16" CR
		"bfi r4, r5, #16, #16" CR
		"bfi r6, r7, #16, #16" CR
		"bfi r8, r9, #16, #16" CR
		"stm %[buffer], {r2, r4, r6, r8}" CR

		/* reset rk_masked to initial value */
		"sub %[rk_masked], #416" CR

		/* sec_speck_v07d */
		/* Registers allocation:
			r0,r 1: buffer, rk_masked
			r2 : rv, r3: rm, r4: lv, r5: lm
			r6 : rk (m/v
			r7 : temp a, g_mask, u_mask
			r8 : temp b, h_mask
			r9 : p
			r10: g
			r11: h
			r12: loop counter
			r14: LR u/z
		 */
		/* load data */
		"ldm %[buffer], {r2-r5}" CR
		"push {%[buffer]}" CR
        /* set the pointer to round keys */
        "add %[rk_masked], #216" CR
        /* initialize loop counter */
        "mov r12, 27" CR
        "dec_step:" CR
        /* process right branch */
        "eor r2, r2, r4" CR
        "eor r3, r3, r5" CR
        "ror r2, r2, 3" CR
        "ror r3, r3, 3" CR
        /* process left branch */
        /* load key */
        "ldmdb %[rk_masked]!, {r6}" CR
        "eor r5, r5, r6" "\n\t"
        "ldmdb %[rk_masked]!, {r6}" CR
        "eor r4, r4, r6" CR
        /* secure subtraction using CSA */
        SEC_SUB(r4, r5, r2, r3, r6, r7, r8, r9, r10, r11, r14)
        "ror r4, r4, 24" CR
        "ror r5, r5, 24" CR
        /* loop end */
        "subs r12, r12, #1" CR
        "bne dec_step" CR
		/* store results */
		"pop {%[buffer]}" CR
        "str r2, [%[buffer], 0]" CR
        "str r3, [%[buffer], 4]" CR
        "str r4, [%[buffer], 8]" CR
        "str r5, [%[buffer], 12]" CR

        /* test */
		"add r2, r1, r2" CR
		"mov r2, r1" CR
		"subs r2, r1, r3" CR
		"lsls r2, #2" CR
		"adds r2, #2" CR
		"subs r2, #2" CR
		"cmp r2, #2" CR
		"movs r2, #1" CR
		"adds r2, r1, #1" CR
		"subs r2, r1, #1" CR
		"add r3, %[buffer], 8" CR
		"ldmdb r3, {r1, r2}" CR
		"stmdb r3, {r1, r2}" CR
		"mov r4, #4" CR
		"lsl.w r8, r6, r4" CR
		"lsr.w r8, r6, r4" CR
		"asr.w r8, r6, r4" CR
		"ror.w r8, r6, r4" CR
/*
		"ldr r2, [sp, #0]" CR
        "sub r13, r13, #0" CR
        "add r13, r13, #0" CR
		"str r2, [sp, #0]" CR
*/

    :
    : [buffer] "r" (buffer), [rk_masked] "r" (rk_masked)
    : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
	);
}
