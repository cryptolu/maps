#include <stdint.h>
#include "arm_macros.h"

#define CR "\n\t"


void sec_speck_v02(uint32_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: rv, [1]: rm
		[2]: lv, [3]: lm
		[4]: random
		[5]: random
	   rk_masked defined as follow:
		[2*i]: rkv[i], [2*i + 1]: rkm[i]
	*/
	
	asm volatile (
	/* Registers allocation:
		r0,r 1: buffer, rk_masked
		r2 : rv, r3: rm, r4: lv, r5: lm
		r6 : rk (m/v)
		r7 : random a
		r8 : random b
		r9 : p
		r10: g
		r11: h
		r12: loop counter
		r14: LR u/z
	 */
	/* load data */
	"ldm %[buffer], {r2, r3, r4, r5, r7, r8}" CR
	"push {%[buffer]}" CR


        /* set the pointer to round keys */
        "add %[rk_masked], #216" "\n\t"



        /* initialize loop counter */
        "mov r12, 27" "\n\t"
        "dec_step:" "\n\t"


        /* process right branch */
        "eor r2, r2, r4" "\n\t"
        "eor r3, r3, r5" "\n\t"

        "ror r2, r2, 3" "\n\t"
        "ror r3, r3, 3" "\n\t"


        /* process left branch */
        /* load key */
        "ldmdb %[rk_masked]!, {r6}" "\n\t"
        "eor r5, r5, r6" "\n\t"

        "ldmdb %[rk_masked]!, {r6}" "\n\t"
        "eor r4, r4, r6" "\n\t"

        /* secure subtraction using KSA */
        SEC_SUB(r4, r5, r2, r3, r6, r7, r8, r9, r10, r11, r14, r0)

        "ror r4, r4, 24" "\n\t"
        "ror r5, r5, 24" "\n\t"


        /* loop end */
        "subs r12, r12, #1" "\n\t"
        "bne dec_step" "\n\t"


	/* store results */
	"pop {%[buffer]}" CR
	"stm %[buffer], {r2-r5}" CR

	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14"
	);
}
