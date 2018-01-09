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

void sec_rectangle_v04(uint16_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: row0v, [1]:row1v, [2]: row2v, [3]: row3v
		[4]: row0m, [5]:row1m, [6]: row2m, [7]: row3m
	*/
	
	asm volatile (
	/* Registers allocation:
		r2 : row0v
		r3 : row1v
		r4 : row2v
		r5 : row3v
		r6 : row0m
		r7 : row1m
		r8 : row2m
		r9 : row3m
		r10: rkv/temp
		r11: rkm/temp
		r12: temp
		r14: loop counter
	 */
	/* load block */
	"ldm %[buffer], {r2, r4, r6, r8}" CR
	"mov r3, r2, lsr #16" CR
	"mov r5, r4, lsr #16" CR
	"mov r7, r6, lsr #16" CR
	"mov r9, r8, lsr #16" CR
	"movt r2, #0" CR
	"movt r4, #0" CR
	"movt r6, #0" CR
	"movt r8, #0" CR
	"push {%[buffer]}" CR
	/* initialize loop */
	"mov r14, 25" CR
	"enc_step:" CR
	/* load key */
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r2, r2, r10" CR
	"eor r3, r3, r10, lsr #16" CR
	"eor r4, r4, r11" CR
	"eor r5, r5, r11, lsr #16" CR
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r6, r6, r10" CR
	"eor r7, r7, r10, lsr #16" CR
	"eor r8, r8, r11" "\n\t"
	"eor r9, r9, r11, lsr #16" CR
	/* S-layer */
	"push {r14}" CR
	"bic r10, r5, r3" CR
	"orr %[buffer], r5, r7" CR
	"eor r10, r10, %[buffer]" CR
	"and r12, r9, r7" CR
	"mov %[buffer], 0" CR /* remove leakage for next instruction */
	"orn %[buffer], r9, r3" CR
	"eor r12, r12, %[buffer]" CR
	"eor r10, r10, r2" CR
	"eor r12, r12, r6" CR
	"bic %[buffer], r2, r3" CR
	"orn r2, r2, r7" CR
	"eor r2, r2, %[buffer]" CR
	"mov %[buffer], 0" CR /* remove leakage for next instruction */
	"bic %[buffer], r6, r3" CR
	"orn r6, r6, r7" CR
	"eor r6, r6, %[buffer]" CR
	"eor r11, r4, r5" CR
	"eor r14, r8, r9" CR
	"eor r2, r2, r11" CR
	"eor r6, r6, r14" CR
	"eor r5, r3, r4" CR
	"eor r9, r7, r8" CR
	"eor r3, r4, r10" CR
	"eor r7, r8, r12" CR
	"push {r11}" CR
	"mov %[buffer], 0" CR /* remove leakage for next instruction */
	"and %[buffer], r10, r11" CR
	"mov r11, 0" CR /* remove leakage for next instruction */
	"orn r11, r10, r14" CR
	"eor r11, r11, %[buffer]" CR
	"mov %[buffer], 0" CR /* remove leakage for next instruction */
	"orn %[buffer], r12, r14" CR

	"mov r14, 0" CR /* remove leakage for next instruction */
	"pop {r14}" CR
	"and r14, r12, r14" CR
	"eor r14, r14, %[buffer]" CR
	"eor r5, r5, r11" CR
	"eor r9, r9, r14" CR
	"mov r4, 0" CR /* remove leakage for next instruction */
	"and r4, r2, r5" CR
	"mov %[buffer], 0" CR /* remove leakage for next instruction */
	"orr %[buffer], r2, r9" CR
	"eor r4, r4, %[buffer]" CR
	"mov r8, 0" CR /* remove leakage for next instruction */
	"and r8, r6, r9" CR
	"mov %[buffer], 0" CR /* remove leakage for next instruction */
	"orr %[buffer], r6, r5" CR
	"eor r8, r8, %[buffer]" CR
	"eor r4, r4, r10" CR
	"eor r8, r8, r12" CR
	"pop {r14}" CR
	/* P-layer */
	"bfi r3, r3, #16, #15" CR
	"ror r3, r3, #15" CR
	"bfi r7, r7, #16, #15" CR
	"ror r7, r7, #15" CR
	"bfi r4, r4, #16, #4" CR
	"ror r4, r4, #4" CR
	"bfi r8, r8, #16, #4" CR
	"ror r8, r8, #4" CR
	"bfi r5, r5, #16, #3" CR
	"ror r5, r5, #3" CR
	"bfi r9, r9, #16, #3" CR
	"ror r9, r9, #3" CR
	/* loop end */
	"subs r14, r14, #1" CR
	"bne enc_step" CR
	"pop {%[buffer]}" CR
	/* apply last round key */
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r2, r2, r10" CR
	"eor r3, r3, r10, lsr #16" CR
	"eor r4, r4, r11" CR
	"eor r5, r5, r11, lsr #16" CR
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r6, r6, r10" CR
	"eor r7, r7, r10, lsr #16" CR
	"eor r8, r8, r11" CR
	"eor r9, r9, r11, lsr #16" CR
	/* store results */
	"bfi r2, r3, #16, #16" CR
	"bfi r4, r5, #16, #16" CR
	"bfi r6, r7, #16, #16" CR
	"bfi r8, r9, #16, #16" CR
	"stm %[buffer], {r2, r4, r6, r8}" CR
	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
	);
}
