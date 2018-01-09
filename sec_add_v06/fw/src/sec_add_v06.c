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
 * sec_add_v06 (DDi)
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
void sec_add_v06(uint32_t *a_buf, uint32_t b_buf)
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
		r12: temp
	 */

	/* load a and b */
	"ldm %[a_buf], {r2, r3}" CR
	"ldm %[b_buf], {r4, r5}" CR

	/* p(mval, mask) = (r8, r3) */
	SEC_XOR(r8, r4, r5, r2, r3)

	/* g(mval, mask) = (r9, r6) */
	SEC_AND(r9, r6, r2, r3, r4, r5, r12)

	/* g(mval, mask) = (r9, r3) */
	"eor r9, r9, r3" CR
	"eor r9, r9, r6" CR

/* First iteration - begin */
	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, 1)

	/* u(mval, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r5, r10, r7, r12)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r8, r5, 1)

	/* h(mval, mask) = (r10, r3) */
	"eor r10, r10, r3" CR
	"eor r10, r10, r7" CR

	/* p(mval, mask) = (r8, r6) */
	SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)

	/* p(mval, mask) = (r8, r5) */
	"eor r8, r8, r5" CR
	"eor r8, r8, r6" CR
/* First iteration - end */


/* Second iteration - begin */
	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, 2)

	/* u(mval, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r5, r10, r7, r12)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r8, r5, 2)

	/* h(mval, mask) = (r10, r3) */
	"eor r10, r10, r3" CR
	"eor r10, r10, r7" CR

	/* p(mval, mask) = (r8, r6) */
	SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)

	/* p(mval, mask) = (r8, r5) */
	"eor r8, r8, r5" CR
	"eor r8, r8, r6" CR
/* Second iteration - end */

/* Third iteration - begin */
	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, 4)

	/* u(mval, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r5, r10, r7, r12)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r8, r5, 4)

	/* h(mval, mask) = (r10, r3) */
	"eor r10, r10, r3" CR
	"eor r10, r10, r7" CR

	/* p(mval, mask) = (r8, r6) */
	SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)

	/* p(mval, mask) = (r8, r5) */
	"eor r8, r8, r5" CR
	"eor r8, r8, r6" CR
/* Third iteration - end */

/* Fourth iteration - begin */
	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, 8)

	/* u(mval, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r5, r10, r7, r12)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r8, r5, 8)

	/* h(mval, mask) = (r10, r3) */
	"eor r10, r10, r3" CR
	"eor r10, r10, r7" CR

	/* p(mval, mask) = (r8, r6) */
	SEC_AND_IN_PLACE(r6, r8, r5, r10, r3, r7)

	/* p(mval, mask) = (r8, r5) */
	"eor r8, r8, r5" CR
	"eor r8, r8, r6" CR
/* Fourth iteration - end */

	/* h(mval, mask) = (r10, r7) */
	SEC_SHIFT(r10, r7, r9, r3, 16)

	/* u(mval, mask) = (r11, r6) */
	SEC_AND(r11, r6, r8, r5, r10, r7, r12)

	/* g(mval, mask) = (r9, r3) */
	SEC_XOR(r9, r9, r3, r11, r6)

	/* z(mval, mask) = (r11, r5) */
	SEC_XOR(r11, r4, r5, r2, r3)

	/* z(mval, mask) = (r11, r5) */
	"eor r11, r11, r9, lsl #1" CR
	"eor r11, r11, r3, lsl #1" CR

	/* Change mask */
	/* z(mval, mask) = (r2, r3) */
	"eor r2, r11, r3" CR
	"eor r2, r2, r5" CR

	/* store results */
	"stm %[a_buf], {r2, r3}" CR
	:
	: [b_buf] "r" (b_buf), [a_buf] "r" (a_buf)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
	);
}
