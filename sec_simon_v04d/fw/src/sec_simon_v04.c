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

#define CR "\n\t"

void sec_simon_v04(uint32_t *buffer, uint32_t rk_masked)
{
	asm volatile (
		/* r2: rv, r3: rm, r4: lv, r5: lm
		 */
		"ldm %[buffer], {r2-r5}" CR


        /* set the pointer to round keys */
        "add %[rk_masked], #352" "\n\t"


        /* initialize loop counter */
        "mov r10, 44" "\n\t"
        "dec_step:" "\n\t"


        /* process left branch */
        "ror r6, r2, 31" "\n\t"
        "ror r7, r3, 31" "\n\t"

        /* SecAnd - begin: (x_mval, x_mask) = (r6, r7); (y_mval, y_mask) = (r2, r3) */
        "and r8, r6, r2, ror 24" "\n\t"
        "orn r6, r6, r3, ror 24" "\n\t"
        "eor r6, r6, r8"         "\n\t"

        "mov r8, 0" "\n\t" /* prevent HD leakage */
        "and r8, r7, r2, ror 24" "\n\t"
        "orn r9, r7, r3, ror 24" "\n\t"
        "eor r7, r9, r8"         "\n\t"
        /* SecAnd - end */

        "eor r4, r6" "\n\t"
        "eor r5, r7" "\n\t"

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
        "subs r10, r10, #1" "\n\t"
        "bne dec_step" "\n\t"



		"stm %[buffer], {r2-r5}" CR
	:
	: [buffer] "r" (buffer), [rk_masked] "r" (rk_masked)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
	);
}
