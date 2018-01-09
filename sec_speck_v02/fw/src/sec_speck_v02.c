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


void sec_speck_v02(uint32_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: rv, [1]: rm
		[2]: lv, [3]: lm
		[4]: random
		[5]: random
	   rk_masked defined as follow:
		[2*i]: rkv[i], [2*i + 1]: rkm[i]
	*/
	
	asm volatile (
	/* Registers allocation:
		r0,r 1: buffer, rk_masked
		r2 : rv, r3: rm, r4: lv, r5: lm
		r6 : rk (m/v)
		r7 : random a
		r8 : random b
		r9 : p
		r10: g
		r11: h
		r12: loop counter
		r14: LR u/z
	 */
	/* load data */
	"ldm %[buffer], {r2, r3, r4, r5, r7, r8}" CR
	"push {%[buffer]}" CR

	/* initialize loop counter */
	"mov r12, #27" CR
	"enc_step:" CR

	/* process left branch */
	"ror r4, r4, 8" CR
	"ror r5, r5, 8" CR

	/* secure add using KSA */
	/* Previous original version: 5th parameter was r6 instead of r8. However,
	   SEC_ADD expected a random number as 5th parameter. So r6 was replaced
	   by r8, effectively killing the last remaing leakage
	 SEC_ADD(r4, r5, r2, r3, r6, r7, r8, r9, r10, r11, r14, %[buffer])
	*/
	SEC_ADD(r4, r5, r2, r3, r8, r7, r6, r9, r10, r11, r14, %[buffer])

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
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14"
	);
}
