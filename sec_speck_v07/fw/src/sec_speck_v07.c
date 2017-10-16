#include <stdint.h>
#include "arm_macros.h"

#define CR "\n\t"

void sec_speck_v07(uint32_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: rv, [1]: rm
		[2]: lv, [3]: lm
	   rk_masked defined as follow:
		[2*i]: rkv[i], [2*i + 1]: rkm[i]
	*/
	
	asm volatile (
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

	/* initialize loop counter */
	"mov r12, #27" CR
	"enc_step:" CR

	/* process left branch */
	"ror r4, r4, 8" CR
	"ror r5, r5, 8" CR

	/* secure add using KSA */
	SEC_ADD(r4, r5, r2, r3, r6, r7, r8, r9, r10, r11, r14)

	/* load key */
	"ldm %[rk_masked]!, {r6}" CR
	"eor r4, r4, r6" CR
	"ldm %[rk_masked]!, {r6}" CR
	"eor r5, r5, r6" CR

	/* process right branch */
	"ror r2, r2, 29" CR
	"ror r3, r3, 29" CR
	"eor r2, r2, r4" CR
	"eor r3, r3, r5" CR

	/* loop end */
	"subs r12, r12, #1" CR
	"bne enc_step" CR

	/* store results */
	"pop {%[buffer]}" CR
	"stm %[buffer], {r2-r5}" CR

	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
	);
}
