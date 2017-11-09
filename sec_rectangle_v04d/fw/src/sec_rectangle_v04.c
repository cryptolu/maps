#include <stdint.h>

#define CR "\n\t"

void sec_rectangle_v04(uint16_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: row0v, [1]:row1v, [2]: row2v, [3]: row3v
		[4]: row0m, [5]:row1m, [6]: row2m, [7]: row3m
	*/
	
	asm volatile (
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


        /* set the pointer to round keys */
        "add %[rk_masked], %[rk_masked], #408" "\n\t"


        /* initialize loop counter */
        "mov r14, 25" "\n\t"
        "dec_step:" "\n\t"


        /* load key */
        "ldm %[rk_masked], {r10, r11}" "\n\t"

        "eor r6, r6, r10" "\n\t"
        "eor r7, r7, r10, lsr #16" "\n\t"
        "eor r8, r8, r11" "\n\t"
        "eor r9, r9, r11, lsr #16" "\n\t"

        "sub %[rk_masked], %[rk_masked], #8" "\n\t"

        "ldm %[rk_masked], {r10, r11}" "\n\t"

        "eor r2, r2, r10" "\n\t"
        "eor r3, r3, r10, lsr #16" "\n\t"
        "eor r4, r4, r11" "\n\t"
        "eor r5, r5, r11, lsr #16" "\n\t"

        "sub %[rk_masked], %[rk_masked], #8" "\n\t"


        /* l layer */
        "bfi r3, r3, #16, #1" "\n\t"
        "ror r3, r3, #1" "\n\t"

        "bfi r7, r7, #16, #1" "\n\t"
        "ror r7, r7, #1" "\n\t"

        "bfi r4, r4, #16, #12" "\n\t"
        "ror r4, r4, #12" "\n\t"

        "bfi r8, r8, #16, #12" "\n\t"
        "ror r8, r8, #12" "\n\t"

        "bfi r5, r5, #16, #13" "\n\t"
        "ror r5, r5, #13" "\n\t"

        "bfi r9, r9, #16, #13" "\n\t"
        "ror r9, r9, #13" "\n\t"


        /* s layer */
        "push {%[rk_masked], r14}" "\n\t"


        /*"eor r8, r2, r3" "\n\t"*/ /* (r0, r1) - stack */
        "eor %[buffer], r2, r3" "\n\t"
        "eor %[rk_masked], r6, r7" "\n\t"
        "push {%[buffer]}" "\n\t"


        /*"orr r6, r2, r5" "\n\t"*/ /* (r10, r12) */
        "and r10, r2, r5" "\n\t"
        "orr %[buffer], r2, r9" "\n\t"
        "eor r10, r10, %[buffer]" "\n\t"

        "and r12, r6, r9" "\n\t"
        "mov %[buffer], 0" "\n\t" /* prevent HD leakage*/
        "orr %[buffer], r6, r5" "\n\t"
        "eor r12, r12, %[buffer]" "\n\t"


        /*"eor r6, r6, r4" "\n\t"*/ /* (r10, r12) */
        "eor r10, r10, r4" "\n\t"
        "eor r12, r12, r8" "\n\t"


        /*"eor r4, r3, r6" "\n\t"*/ /* (r4, r8) */
        "eor r4, r3, r10" "\n\t"
        "eor r8, r7, r12" "\n\t"


        /*"and r7, r2, r6" "\n\t"*/ /* (r11, r14) */
        "mov %[buffer], 0" "\n\t" /* prevent HD leakage*/
        "and %[buffer], r2, r10" "\n\t"
        "orn r11, r2, r12" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage*/
        "and %[buffer], r6, r10" "\n\t"
        "orn r14, r6, r12" "\n\t"
        "eor r14, r14, %[buffer]" "\n\t"


        /*"eor r7, r7, r5" "\n\t"*/ /* (r11, r14) */
        "eor r11, r11, r5" "\n\t"
        "eor r14, r14, r9" "\n\t"


        /*"eor r3, r7, r4" "\n\t"*/ /* (r3, r7) */
        "eor r3, r11, r4" "\n\t"
        "eor r7, r14, r8" "\n\t"


        /*"orn r7, r6, r3" "\n\t"*/ /* (r11, r14) */
        "bic r11, r10, r3" "\n\t"
        "orr %[buffer], r10, r7" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "and r14, r12, r7" "\n\t"
        "mov %[buffer], 0" "\n\t" /* prevent HD leakage*/
        "orn %[buffer], r12, r3" "\n\t"
        "eor r14, r14, %[buffer]" "\n\t"


        /*"eor r5, r8, r7" "\n\t"*/ /* (r5, r9) */
        "pop {%[buffer]}" "\n\t"
        "mov r5, 0" "\n\t" /* prevent HD leakage*/
        "eor r5, %[buffer], r11" "\n\t"
        "mov r9, 0" "\n\t" /* prevent HD leakage*/
        "eor r9, %[rk_masked], r14" "\n\t"


        /*"orn r7, r5, r3" "\n\t"*/ /* (r11, r14) */
        "mov r11, 0" "\n\t" /* prevent HD leakage*/
        "bic r11, r5, r3" "\n\t"
        "mov %[buffer], 0" "\n\t" /* prevent HD leakage*/
        "orr %[buffer], r5, r7" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "mov r14, 0" "\n\t" /* prevent HD leakage*/
        "and r14, r9, r7" "\n\t"
        "mov %[buffer], 0" "\n\t" /* prevent HD leakage*/
        "orn %[buffer], r9, r3" "\n\t"
        "eor r14, r14, %[buffer]" "\n\t"


        /*"eor r2, r6, r7" "\n\t"*/ /* (r2, r6) */
        "mov r2, 0" "\n\t" /* prevent HD leakage*/
        "eor r2, r10, r11" "\n\t"
        "mov r6, 0" "\n\t" /* prevent HD leakage*/
        "eor r6, r12, r14" "\n\t"


        "pop {%[rk_masked], r14}" "\n\t"


        /* loop end */
        "subs r14, r14, #1" "\n\t"
        "bne dec_step" "\n\t"


        "pop {%[buffer]}" "\n\t"


        /* load key */
        "ldm %[rk_masked], {r10, r11}" "\n\t"

        "eor r6, r6, r10" "\n\t"
        "eor r7, r7, r10, lsr #16" "\n\t"
        "eor r8, r8, r11" "\n\t"
        "eor r9, r9, r11, lsr #16" "\n\t"

        "sub %[rk_masked], r1, #8" "\n\t"

        "ldm %[rk_masked], {r10, r11}" "\n\t"

        "eor r2, r2, r10" "\n\t"
        "eor r3, r3, r10, lsr #16" "\n\t"
        "eor r4, r4, r11" "\n\t"
        "eor r5, r5, r11, lsr #16" "\n\t"


	/* store results */
	"bfi r2, r3, #16, #16" CR
	"bfi r4, r5, #16, #16" CR
	"bfi r6, r7, #16, #16" CR
	"bfi r8, r9, #16, #16" CR
	"stm %[buffer], {r2, r4, r6, r8}" CR
	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
	);
}
