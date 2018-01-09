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

#include <stdint.h>
#include "arm_macros.h"

#define CR "\n\t"

void sec_speck_v06(uint32_t *buffer, uint32_t rk_masked)
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


    /* set the pointer to round keys */
    "add %[rk_masked], #216" "\n\t"


        /* initialize loop counter */
        "mov r12, 27" "\n\t"
        "dec_step:" "\n\t"


        /* process right branch */
        "eor r2, r2, r4" "\n\t"
        "eor r3, r3, r5" "\n\t"

        "ror r2, r2, 3" "\n\t"
        "ror r3, r3, 3" "\n\t"


        /* process left branch */
        /* load key */
        "ldmdb %[rk_masked]!, {r6}" "\n\t"
        "eor r5, r5, r6" "\n\t"

        "ldmdb %[rk_masked]!, {r6}" "\n\t"
        "eor r4, r4, r6" "\n\t"

        /* secure subtraction using KSA */
        SEC_SUB(r4, r5, r2, r3, r6, r7, r8, r9, r10, r11, r14, %[buffer])

        "ror r4, r4, 24" "\n\t"
        "ror r5, r5, 24" "\n\t"


        /* loop end */
        "subs r12, r12, #1" "\n\t"
        "bne dec_step" "\n\t"


	/* store results */
	"pop {%[buffer]}" CR
	"stm %[buffer], {r2-r5}" CR

	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
	);
}
