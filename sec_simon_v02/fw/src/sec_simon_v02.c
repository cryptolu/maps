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
		"ldm %[buffer], {r2, r3, r4, r5, r10}" CR
		"mov r11, #44" CR
		"enc_step:" CR
		/* process left branch */
        "ror r6, r4, 31" CR
        "ror r7, r5, 31" CR
        /* SecAnd - begin: (x_mval, x_mask) = (r6, r7); (y_mval, y_mask) = (r4, r5) */
        "and r8, r6, r4, ror 24" CR
        "and r9, r6, r5, ror 24" CR
        "eor r6, r10, r8" CR
        "eor r6, r6, r9" CR
		"mov r8, 0" CR /* correct leakage from next instruction */
        "and r8, r7, r4, ror 24" CR
        "eor r6, r6, r8" CR
		"mov r8, 0" CR /* correct leakage from next instruction */
        "and r8, r7, r5, ror 24" CR
        "eor r6, r6, r8" CR
        /* SecAnd - end */
        "eor r2, r6" CR
        "eor r3, r10" CR
        "eor r2, r2, r4, ror 30" CR
        "eor r3, r3, r5, ror 30" CR
        /* load key */
        "ldm %[rk_masked]!, {r6, r7}" CR
        "eor r2, r2, r6" CR
        "eor r3, r3, r7" CR
        /* swap branches */
        "eor r2, r2, r4" CR
        "eor r4, r4, r2" CR
        "eor r2, r2, r4" CR
        "eor r3, r3, r5" CR
        "eor r5, r5, r3" CR
        "eor r3, r3, r5" CR
        /* loop end */
        "subs r11, r11, #1" CR
        "bne enc_step" CR
		"stm %[buffer], {r2-r5}" CR
	:
	: [buffer] "r" (buffer), [rk_masked] "r" (rk_masked)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
	);
}
