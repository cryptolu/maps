/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * arm_v7m_leakage simulator
 *
 * Copyright (C) 2017 University of Luxembourg
 *
 * Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu> and
 * Daniel Dinu <daniel.dinu@uni.lu>
 *
 * This simulator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

/******************************************************************************
 *
 * sec_add_v02 (DDi)
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
void sec_add_v12(uint32_t *a_buf, uint32_t b_buf)
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





        /* y(mval, mask) = (r4, r5) */
        SEC_NOT(r4, r5)

        /* p(mval, mask) = (r8, r3) */
        SEC_XOR(r8, r2, r3, r4, r5)

        /* g(mval, mask) = (r9, r6) */
        SEC_AND(r9, r6, r2, r3, r4, r5, r12)

        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r6" "\r\n"


        "push {r2}" "\n\t"



        /* First iteration - begin */
        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S_1_ITERATION(r10, r7, r9, r3, r2)

        /* u(mval, mask) = (r11, r5) */
        SEC_AND(r11, r5, r8, r3, r10, r7, r2)

        /* g(mval, mask) = (r9, r6) */
        SEC_OR_IN_PLACE(r6, r9, r3, r11, r5, r2)


        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r6" "\r\n"


        /* h(mval, mask) = (r10, r7) */
        // SEC_SHIFT_1S_1_ITERATION(r10, r7, r8, r3, r2)
        SEC_SHIFT_1_ITERATION(r10, r7, r8, r3)

        /* p(mval, mask) = (r8, r6) */
        SEC_AND_IN_PLACE(r6, r8, r3, r10, r7, r2, r12)


        /* p(mval, mask) = (r8, r3) */
        "eor r8, r8, r3" "\r\n"
        "eor r8, r8, r6" "\r\n"
        /* First iteration - end */


        /* Second iteration - begin */
        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S_2_ITERATION(r10, r7, r9, r3, r2)

        /* u(mval, mask) = (r11, r5) */
        SEC_AND(r11, r5, r8, r3, r10, r7, r2)

        /* g(mval, mask) = (r9, r6) */
        SEC_OR_IN_PLACE(r6, r9, r3, r11, r5, r2)


        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r6" "\r\n"


        /* h(mval, mask) = (r10, r7) */
        // SEC_SHIFT_1S_2_ITERATION(r10, r7, r8, r3, r2)
        SEC_SHIFT_2_ITERATION(r10, r7, r8, r3)

        /* p(mval, mask) = (r8, r6) */
        SEC_AND_IN_PLACE(r6, r8, r3, r10, r7, r2, r12)


        /* p(mval, mask) = (r8, r3) */
        "eor r8, r8, r3" "\r\n"
        "eor r8, r8, r6" "\r\n"
        /* Second iteration - end */


        /* Third iteration - begin */
        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S_3_ITERATION(r10, r7, r9, r3, r2)

        /* u(mval, mask) = (r11, r5) */
        SEC_AND(r11, r5, r8, r3, r10, r7, r2)

        /* g(mval, mask) = (r9, r6) */
        SEC_OR_IN_PLACE(r6, r9, r3, r11, r5, r2)


        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r6" "\r\n"


        /* h(mval, mask) = (r10, r7) */
        // SEC_SHIFT_1S_3_ITERATION(r10, r7, r8, r3, r2)
        SEC_SHIFT_3_ITERATION(r10, r7, r8, r3)

        /* p(mval, mask) = (r8, r6) */
        SEC_AND_IN_PLACE(r6, r8, r3, r10, r7, r2, r12)


        /* p(mval, mask) = (r8, r3) */
        "eor r8, r8, r3" "\r\n"
        "eor r8, r8, r6" "\r\n"
        /* Third iteration - end */


        /* Fourth iteration - begin */
        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S_4_ITERATION(r10, r7, r9, r3, r2)

        /* u(mval, mask) = (r11, r5) */
        SEC_AND(r11, r5, r8, r3, r10, r7, r2)

        /* g(mval, mask) = (r9, r6) */
        SEC_OR_IN_PLACE(r6, r9, r3, r11, r5, r2)


        /* g(mval, mask) = (r9, r3) */
        "eor r9, r9, r3" "\r\n"
        "eor r9, r9, r6" "\r\n"


        /* h(mval, mask) = (r10, r7) */
        // SEC_SHIFT_1S_4_ITERATION(r10, r7, r8, r3, r2)
        SEC_SHIFT_4_ITERATION(r10, r7, r8, r3)

        /* p(mval, mask) = (r8, r6) */
        SEC_AND_IN_PLACE(r6, r8, r3, r10, r7, r2, r12)


        /* p(mval, mask) = (r8, r3) */
        "eor r8, r8, r3" "\r\n"
        "eor r8, r8, r6" "\r\n"
        /* Fourth iteration - end */



        /* h(mval, mask) = (r10, r7) */
        SEC_SHIFT_1S_5_ITERATION(r10, r7, r9, r3, r2)

        /* u(mval, mask) = (r11, r5) */
        SEC_AND(r11, r5, r8, r3, r10, r7, r12)

        /* g(mval, mask) = (r9, r7) */
        //SEC_OR_IN_PLACE(r7, r9, r3, r11, r5, r2) // TODO
        // r_mask = r7, x_mval = r9, x_mask = r3, y_mask = r11, y_mval = r5, temp = r2
            "orr r2, r9, r11" "\n\t"

            "and r9, r9, r5" "\n\t"
            //"eor r9, r9, r7" "\n\t" // old
            "eor r9, r6" "\n\t" // new

            "eor r9, r9, r2" "\n\t"

            "mov r2, 0" "\n\t" /* prevent HD leakage */
            "orr r2, r3, r5" "\n\t"
            "eor r9, r9, r2" "\n\t"

            "and r2, r3, r11" "\n\t"
            "eor r9, r9, r2" "\n\t"




        "mov r2, 0" "\n\t" /* prevent HD leakage */
        "pop {r2}" "\n\t"


        /* z(mval, mask) = (r11, r5) */
        SEC_XOR(r11, r4, r5, r2, r3)


        /* z(mval, mask) = (r11, r5) */
        //"mov r12, 0" "\n\t" /* prevent HD leakage */
        "lsl r12, r9, #1" "\r\n"
        "orr r12, #1" "\r\n"
        "eor r11, r11, r12" "\r\n"
        //"eor r11, r11, r7, lsl #1" "\r\n" // old
        "eor r11, r11, r6, lsl #1" "\r\n" // new


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
