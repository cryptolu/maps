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

#define CORRECT_LEAKAGE

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
		#ifdef CORRECT_LEAKAGE
		"mov r8, 0" CR /* correct leakage from next instruction */
		#endif
        "and r8, r7, r4, ror 24" CR
        "eor r6, r6, r8" CR
		#ifdef CORRECT_LEAKAGE
		"mov r8, 0" CR /* correct leakage from next instruction */
		#endif
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
