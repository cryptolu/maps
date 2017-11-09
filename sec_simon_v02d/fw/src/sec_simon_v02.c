#include <stdint.h>

#define CR "\n\t"

void sec_simon_v02(uint32_t *buffer, uint32_t rk_masked)
{
	/* buffer structure:
		[0]: rv
		[1]: rm
		[2]: lv
		[3]: lm
		[4]: r (random value)
	   rk_masked structure:
		[2*i]: rkv[i]
		[2*i + 1]: rkm[i]
	 */
	asm volatile (
		/* r2: rv, r3: rm, r4: lv, r5: lm
		   r6: rkv
		   r7: rkm
		   r8: temp
		   r9: temp
		   r10: random number (interm. mask)
		   r11: loop counter
		 */


        /* set the pointer to round keys */
        "add %[rk_masked], #352" "\n\t"


        /* load block */
		"ldm %[buffer], {r2, r3, r4, r5, r10}" CR


        /* initialize loop counter */
        "mov r11, 44" "\n\t"
        "dec_step:" "\n\t"


        /* process left branch */
        "ror r6, r2, 31" "\n\t"
        "ror r7, r3, 31" "\n\t"

        /* SecAnd - begin: (x_mval, x_mask) = (r6, r7); (y_mval, y_mask) = (r2, r3) */
        "and r8, r6, r2, ror 24" "\n\t"
        "and r9, r6, r3, ror 24" "\n\t"

        "eor r6, r10, r8" "\n\t"
        "eor r6, r6, r9" "\n\t"

        "mov r8, 0" "\n\t" /* prevent HD leakage */
        "and r8, r7, r2, ror 24" "\n\t"
        "eor r6, r6, r8" "\n\t"

        "mov r8, 0" "\n\t" /* prevent HD leakage */
        "and r8, r7, r3, ror 24" "\n\t"
        "eor r6, r6, r8" "\n\t"
        /* SecAnd - end */

        "eor r4, r6" "\n\t"
        "eor r5, r10" "\n\t"  // r7

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
        "subs r11, r11, #1" "\n\t"
        "bne dec_step" "\n\t"


        /* store block */
        "stm %[buffer], {r2-r5}" "\n\t"


	:
	: [buffer] "r" (buffer), [rk_masked] "r" (rk_masked)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
	);
}
