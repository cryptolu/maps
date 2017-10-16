/******************************************************************************
 *
 * sec_add_v01 (DDi)
 *
 ******************************************************************************/

#include <stdint.h>
#include "arm_macros.h"

#define CR "\n\t"

/* a_buf: input
	- [0] value
	- [1] mask
   b_buf: input
	- [0] value
	- [1] mask
	- [2] random a
	- [3] random b
   Outputs: in a_buf[0] (value) and [1] (mask)
 */
void sec_add_v01(uint32_t *a_buf, uint32_t b_buf)
{
	asm volatile (
	/* Registers allocation:
		r2, r3: av, am
		r4, r5: bv, bm
		r6: random a
		r7: random b
		r8: p
		r9: g
		r10: h
		r11: u, z
		r12: temp, loop counter
	 */

	/* load a and b */
	"ldm %[a_buf], {r2, r3}" CR
	"ldm %[b_buf], {r4, r5, r6, r7}" CR

	/* p(mval, mask) = (r8, r3) */
	SEC_XOR(r8, r2, r3, r4, r5)

	/* g(mval, mask) = (r9, r6) */
	SEC_AND(r9, r6, r2, r3, r4, r5, r12)

	/* g(mval, mask) = (r9, r3) */
	"eor r9, r9, r3" CR
	"eor r9, r9, r6" CR

	"push {r4, r5}" CR

	"mov r12, #1" CR
	"iteration_enc:" CR

	/* h(mval, mask) = (r10, r7) */
	"mov r10, 0" CR /* prevent leakage of next instruction in 2nd interation */
	SEC_SHIFT(r10, r7, r9, r3, r12, r5)

	/* u(mval, mask) = (r11, r6) */
	"mov r11, 0" CR /* prevent leakage of next instruction in 2nd interation */
	SEC_AND(r11, r6, r8, r3, r10, r7, r5)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r8, r3, r12, r5)

	/* p(mval, mask) = (r8, r6) */
	SEC_AND_IN_PLACE(r6, r8, r3, r10, r7, r4, r5)

	/* p(mval, mask) = (r8, r3) */
	"eor r8, r8, r3" CR
	"eor r8, r8, r6" CR

	"lsl r12, r12, #1" CR
	"cmp r12, 16" CR
	"bne iteration_enc" CR

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, r12, r5)

	/* u(mval, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r3, r10, r7, r12)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	"pop {r4, r5}" "\n\t"

	/* z(mval, mask) = (r11, r5) */
	SEC_XOR(r11, r4, r5, r2, r3)

	/* z(mval, mask) = (r11, r5) */
	"eor r11, r11, r9, lsl #1" CR
	"eor r11, r11, r3, lsl #1" CR

	/* Change mask */
	/* z(mval, mask) = (r11, r3) */
	"eor r2, r11, r3" CR
	"eor r2, r2, r5" CR

	/* store results */
	"stm %[a_buf], {r2, r3}" CR
	:
	: [b_buf] "r" (b_buf), [a_buf] "r" (a_buf)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
	);
}
