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


        /* y(mval, mask) = (r4, r5) */
        SEC_NOT(r4, r5)

        /* p(mval, mask) = (r8, r3) */
        //SEC_XOR(r8, r2, r3, r4, r5)
        SEC_XOR(r8, r4, r5, r2, r3)

        /* p(mval, mask) = (r8, r5) */
        //"eor r8, r8, r5" "\r\n"
        //"eor r8, r8, r3" "\r\n"

        /* g(mval, mask) = (r9, r6) */
        SEC_AND(r9, r6, r2, r3, r4, r5, r12)

        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r6" "\r\n"


        "push {r2, r4}" "\n\t"


        "mov r12, 1" "\n\t"
        "iteration_dec:" "\n\t"


        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S(r10, r7, r9, r3, r12, r2, r4)

        "mov r6, 0" "\n\t" /* prevent HD leakage */
        /* u(mval, mask) = (r11, r6) */
        SEC_AND(r11, r6, r8, r5, r10, r7, r2)

        /* g(mval, mask) = (r9, r7) */
        SEC_OR_IN_PLACE(r7, r9, r3, r11, r6, r2)


        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r7" "\r\n"


        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT(r10, r7, r8, r5, r12)

        /* h(mval, mask) = (r10, r3) */
        "eor r10, r10, r3" "\r\n"
        "eor r10, r10, r7" "\r\n"

        /* p(mval, mask) = (r8, r6) */
        SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)


        /* p(mval, mask) = (r8, r5) */
        "eor r8, r8, r5" "\r\n"
        "eor r8, r8, r6" "\r\n"


        "lsl r12, r12, #1" "\n\t"


        "cmp r12, 16" "\n\t"
        "bne iteration_dec" "\n\t"


        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S(r10, r7, r9, r3, r12, r2, r4)

        /* u(mval, mask) = (r11, r6) */
        SEC_AND(r11, r6, r8, r5, r10, r7, r12)

        /* g(mval, mask) = (r9, r7) */
        SEC_OR_IN_PLACE(r7, r9, r3, r11, r6, r2)


        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r7" "\r\n"

        "mov r2, 0" "\n\t" /* prevent HD leakage */
        "pop {r2, r4}" "\n\t"


        /* z(mval, mask) = (r11, r5) */
        SEC_XOR(r11, r4, r5, r2, r3)


        /* z(mval, mask) = (r11, r5) */
        "lsl r12, r9, #1" "\r\n"
        "orr r12, #1" "\r\n"
        "eor r11, r11, r12" "\r\n"
        "eor r11, r11, r3, lsl #1" "\r\n"


        /* Change mask */
        /* z(mval, mask) = (r2, r3) */
        "eor r2, r11, r3" "\r\n"
        "eor r2, r2, r5" "\r\n"


	/* store results */
	"stm %[a_buf], {r2, r3}" CR
	:
	: [b_buf] "r" (b_buf), [a_buf] "r" (a_buf)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
	);
}
