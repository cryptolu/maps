/******************************************************************************
 *
 * sec_add_v05 (DDi)
 *
 ******************************************************************************/

#include <stdint.h>
#include "arm_macros.h"

//#define SKIP

#define CR "\n\t"

/* Inputs: a_buf and b_buf with the following format:
	- [0] value
	- [1] mask
   Outputs: in a_buf, with the same format
 */
void sec_add_v05(uint32_t *a_buf, uint32_t *b_buf)
{
	asm volatile (
	/* Registers allocation:
		r2, r3: av, am
		r4, r5: bv, bm
		r6: temp, g_mask, u_mask
		r7: temp, h_mask
		r8: p
		r9: g
		r10: h
		r11: u, z
		r12: temp, loop counter
	 */

	/* load a and b */
	"ldm %[a_buf], {r2, r3}" CR
	"ldm %[b_buf], {r4, r5}" CR

	/* p(val, mask) = (r8, r3) */
	/* g(mval, mask) = (r9, r6) */
    "eor r8, r4, r3" CR
	"and r9, r2, r5" CR
    "eor r8, r8, r2" CR
	"orn r12, r2, r4" CR
	"eor r9, r9, r12" CR
	"add r12, r0, r1" CR /* clear pipeline */
	"and r6, r3, r5" CR
	"eor r12, r12, r12" CR /* clear r12 and pipeline */
	"orn r12, r3, r4" CR
	"eor r6, r6, r12" CR

	/* initialize loop counter */
#ifdef SKIP
	"mov r12, 8" CR
#else
	"mov r12, 1" CR
#endif

	/* g(val, mask) = (r9, r3) */
	"eor r9, r9, r3" CR
	"eor r9, r9, r6" CR

	/* save r2 */
	"push {r2}" CR

"iteration_enc:" CR

	/* h(val, mask) = (r10, r7) */
	/* u(val, mask) = (r11, r6) */
	// SEC_AND(r11, r6, r8, r5, r10, r7, r2)
	"eor r2, r0, r0" CR /* clear r2 and pipeline */
	"lsl r7, r3, r12" CR
	"and r11, r8, r7" CR
	"lsl r10, r9, r12" CR
	"orn r2, r8, r10" CR
	"eor r11, r11, r2" CR
	"eor r6, r0, r0" CR /* clear r6 and pipeline */
	"and r6, r5, r7" CR
	"eor r2, r0, r0" CR /* clear r2 and pipeline */
	"orn r2, r5, r10" CR
	"eor r6, r6, r2" CR

	/* g(val, mask) = (r9, r3) */
	/* h(mval, mask) = (r10, r7) */
	"eor r9, r9, r6" CR
	"lsl r10, r8, r12" CR
	"eor r9, r9, r11" CR
	"lsl r7, r5, r12" CR

	/* h(val, mask) = (r10, r3) */
	"eor r10, r10, r3" CR
	"eor r10, r10, r7" CR

	/* p(val, mask) = (r8, r6) */
	//SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)
	"and r7, r3, r8" CR
	"orn r8, r8, r10" CR
	"eor r8, r7, r8" CR
	"eor r7, r0, r0" CR /* clear r7 and pipeline */
	"and r7, r3, r5" CR
	"orn r6, r5, r10" CR
	"eor r6, r7, r6" CR

	/* p(val, mask) = (r8, r5) */
	"orr r0, r0, r0" CR /* clear pipeline */
	"eor r8, r8, r5" CR
	"eor r8, r8, r6" CR

	"lsl r12, r12, #1" CR
	"cmp r12, 16" CR
	"bne iteration_enc" CR

	/* h(val, mask) = (r10, r7) */
	/* u(val, mask) = (r11, r6) */
	// SEC_AND(r11, r6, r8, r5, r10, r7, r12)
	"lsl r7, r3, r12" CR
	"and r11, r7, r8" CR
	"lsl r10, r9, r12" CR
	"orn r12, r8, r10" CR
	"eor r11, r11, r12" CR

	"eor r12, r0, r0" CR /* clear r12 and pipeline */
	"and r6, r7, r5" CR
	"orn r12, r5, r10" CR
	"eor r6, r6, r12" CR

	/* g(val, mask) = (r9, r3) */
	"eor r9, r9, r6" CR
	"eor r9, r11, r9" CR

	/* restore r2 */
	"pop {r2}" "\n\t"

	/* z(val, mask) = (r8, r5) */
	"eor r8, r3, r4" CR
	"eor r8, r8, r2" CR

	/* z(val, mask) = (r11, r5) */
	"eor r11, r0, r0" CR /* clear pipeline */
	"eor r11, r8, r9, lsl #1" CR
	"orr r0, r0, r0" CR /* clear pipeline */
	"eor r11, r11, r3, lsl #1" CR

	/* Change mask */
	/* z(val, mask) = (r2, r3) */
	/* store results */
	"str r3, [%[a_buf], 4]" CR
	"eor r2, r11, r3" CR
	"eor r2, r2, r5" CR
	"str r2, [%[a_buf], 0]" CR
	:
	: [b_buf] "r" (b_buf), [a_buf] "r" (a_buf)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
	);
}
