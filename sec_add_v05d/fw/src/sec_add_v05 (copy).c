/******************************************************************************
 *
 * sec_add_v05 (DDi)
 *
 ******************************************************************************/

#include <stdint.h>
#include "arm_macros.h"

#define CR "\n\t"

/* Inputs: a_buf and b_buf with the following format:
	- [0] value
	- [1] mask
   Outputs: in a_buf, with the same format
 */
void sec_add_v05(uint32_t *a_buf, uint32_t b_buf)
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
	SEC_XOR(r8, r4, r5, r2, r3)
	/* g(mval, mask) = (r9, r6) */
	SEC_AND(r9, r6, r2, r3, r4, r5, r12)

	/* g(val, mask) = (r9, r3) */
	"eor r9, r9, r3" CR
	"eor r9, r9, r6" CR

	/* save r2 */
	"push {r2}" CR

	/* initialize loop counter */
	"mov r12, 1" CR
"iteration_enc:" CR

	/* h(val, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, r12)

	/* u(val, mask) = (r11, r6) */
	"mov r2, #0" CR /* <- remove leakage in second iteration */
	"mov r6, #0" CR /* <- remove leakage in first iteration */
	SEC_AND(r11, r6, r8, r5, r10, r7, r2)

	/* g(val, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r8, r5, r12)

	/* h(val, mask) = (r10, r3) */
	"eor r10, r10, r3" CR
	"eor r10, r10, r7" CR

	/* p(val, mask) = (r8, r6) */
	SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)

	/* p(val, mask) = (r8, r5) */
	"eor r8, r8, r5" CR
	"eor r8, r8, r6" CR

	"lsl r12, r12, #1" CR
	"cmp r12, 16" CR
	"bne iteration_enc" CR

	/* h(val, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, r12)

	/* u(val, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r5, r10, r7, r12)

	/* g(val, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)
	/* restore r2 */
	"pop {r2}" "\n\t"

	/* z(val, mask) = (r11, r5) */
	SEC_XOR(r11, r4, r5, r2, r3)

	/* z(val, mask) = (r11, r5) */
	"eor r11, r11, r9, lsl #1" CR
	"eor r11, r11, r3, lsl #1" CR

	/* Change mask */
	/* z(val, mask) = (r2, r3) */
	"eor r2, r11, r3" CR
	"eor r2, r2, r5" CR

	/* store results */
	"stm %[a_buf], {r2, r3}" CR
	:
	: [b_buf] "r" (b_buf), [a_buf] "r" (a_buf)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
	);
}
