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
		"mov r10, #44" CR
		"enc_step:" CR
		"ror r6, r4, 31" CR
        "ror r7, r5, 31" CR
		"and r8, r6, r4, ror 24" CR
        "orn r6, r6, r5, ror 24" CR
        "eor r6, r6, r8" CR
		"mov r8, #0" CR
        "and r8, r7, r4, ror 24" CR
        "orn r9, r7, r5, ror 24" CR
        "eor r7, r9, r8" CR
		"eor r2, r6" CR
        "eor r3, r7" CR
        "eor r2, r2, r4, ror 30" CR
        "eor r3, r3, r5, ror 30" CR
		"ldm %[rk_masked]!, {r6, r7}" CR
		"eor r2, r2, r6" CR
		"eor r3, r3, r7" CR
		"eor r2, r2, r4" CR
		"eor r4, r4, r2" CR
		"eor r2, r2, r4" CR
		"eor r3, r3, r5" CR
		"eor r5, r5, r3" CR
		"eor r3, r3, r5" CR
        "subs r10, r10, #1" "\n\t"
        "bne enc_step" "\n\t"
		"stm %[buffer], {r2-r5}" CR
	:
	: [buffer] "r" (buffer), [rk_masked] "r" (rk_masked)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
	);
}
