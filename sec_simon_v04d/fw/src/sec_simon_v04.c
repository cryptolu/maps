#include <stdint.h>

#define CR "\n\t"

void sec_simon_v04(uint32_t *buffer, uint32_t rk_masked)
{
	asm volatile (
		/* r2: rv, r3: rm, r4: lv, r5: lm
		 */
		"ldm %[buffer], {r2-r5}" CR


        /* set the pointer to round keys */
        "add %[rk_masked], #352" "\n\t"


        /* initialize loop counter */
        "mov r10, 44" "\n\t"
        "dec_step:" "\n\t"


        /* process left branch */
        "ror r6, r2, 31" "\n\t"
        "ror r7, r3, 31" "\n\t"

        /* SecAnd - begin: (x_mval, x_mask) = (r6, r7); (y_mval, y_mask) = (r2, r3) */
        "and r8, r6, r2, ror 24" "\n\t"
        "orn r6, r6, r3, ror 24" "\n\t"
        "eor r6, r6, r8"         "\n\t"

        "mov r8, 0" "\n\t" /* prevent HD leakage */
        "and r8, r7, r2, ror 24" "\n\t"
        "orn r9, r7, r3, ror 24" "\n\t"
        "eor r7, r9, r8"         "\n\t"
        /* SecAnd - end */

        "eor r4, r6" "\n\t"
        "eor r5, r7" "\n\t"

        "eor r4, r4, r2, ror 30" "\n\t"
        "eor r5, r5, r3, ror 30" "\n\t"


        /* load key */
        "ldmdb %[rk_masked]!, {r6, r7}" "\n\t"

        "eor r4, r4, r6" "\n\t"
        "eor r5, r5, r7" "\n\t"


        /* swap branches */
        "eor r2, r2, r4" "\n\t"
        "eor r4, r4, r2" "\n\t"
        "eor r2, r2, r4" "\n\t"

        "eor r3, r3, r5" "\n\t"
        "eor r5, r5, r3" "\n\t"
        "eor r3, r3, r5" "\n\t"


        /* loop end */
        "subs r10, r10, #1" "\n\t"
        "bne dec_step" "\n\t"



		"stm %[buffer], {r2-r5}" CR
	:
	: [buffer] "r" (buffer), [rk_masked] "r" (rk_masked)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
	);
}
