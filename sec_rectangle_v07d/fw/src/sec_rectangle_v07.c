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

void sec_rectangle_v07(uint16_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: row0v, [1]:row1v, [2]: row2v, [3]: row3v
		[4]: row0m, [5]:row1m, [6]: row2m, [7]: row3m
		[8]: random, [9]: random
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
		r12: random a
		r14: loop counter
	 */
	/* load block */
	"ldm %[buffer], {r2, r4, r6, r8, r12}" CR
	"mov r3, r2, lsr #16" CR
	"mov r5, r4, lsr #16" CR
	"mov r7, r6, lsr #16" CR
	"mov r9, r8, lsr #16" CR
	"movt r2, #0" CR
	"movt r4, #0" CR
	"movt r6, #0" CR
	"movt r8, #0" CR
	"push {%[buffer]}" CR



        /* set the pointer to round keys */
        "add %[rk_masked], %[rk_masked], #408" "\n\t"


        /* initialize loop counter */
        "mov r14, 25" "\n\t"
        "dec_step:" "\n\t"


        /* load key */
        "ldm %[rk_masked], {r10, r11}" "\n\t"

        "eor r6, r6, r10" "\n\t"
        "eor r7, r7, r10, lsr #16" "\n\t"
        "eor r8, r8, r11" "\n\t"
        "eor r9, r9, r11, lsr #16" "\n\t"

        "sub %[rk_masked], %[rk_masked], #8" "\n\t"

        "ldm %[rk_masked], {r10, r11}" "\n\t"

        "eor r2, r2, r10" "\n\t"
        "eor r3, r3, r10, lsr #16" "\n\t"
        "eor r4, r4, r11" "\n\t"
        "eor r5, r5, r11, lsr #16" "\n\t"

        "sub %[rk_masked], %[rk_masked], #8" "\n\t"


        /* l layer */
        "bfi r3, r3, #16, #1" "\n\t"
        "ror r3, r3, #1" "\n\t"

        "bfi r7, r7, #16, #1" "\n\t"
        "ror r7, r7, #1" "\n\t"

        "bfi r4, r4, #16, #12" "\n\t"
        "ror r4, r4, #12" "\n\t"

        "bfi r8, r8, #16, #12" "\n\t"
        "ror r8, r8, #12" "\n\t"

        "bfi r5, r5, #16, #13" "\n\t"
        "ror r5, r5, #13" "\n\t"

        "bfi r9, r9, #16, #13" "\n\t"
        "ror r9, r9, #13" "\n\t"


        /* s layer */
        "push {%[rk_masked], r14}" "\n\t"


        /*"eor r8, r2, r3" "\n\t"*/ /* (r0, r1) - stack */
        "eor %[buffer], r2, r3" "\n\t"
        "eor %[rk_masked], r6, r7" "\n\t"
        "push {%[buffer], %[rk_masked]}" "\n\t"


        /*"orr r6, r2, r5" "\n\t"*/ /* (r10, r12) */
        "and %[buffer], r2, r5" "\n\t"
        "eor r10, r12, %[buffer]" "\n\t"

        "orr %[buffer], r2, r9" "\n\t"
        "eor r10, r10, %[buffer]" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "orr %[buffer], r6, r5" "\n\t"
        "eor r10, r10, %[buffer]" "\n\t"

        "and %[buffer], r6, r9" "\n\t"
        "eor r10, r10, %[buffer]" "\n\t"


        /*"eor r6, r6, r4" "\n\t"*/ /* (r10, r14) */
        "eor r10, r10, r4" "\n\t"
        "eor r14, r12, r8" "\n\t"


        /*"eor r4, r3, r6" "\n\t"*/ /* (r4, r8) */
        "eor r4, r3, r10" "\n\t"
        "eor r8, r7, r14" "\n\t"


        /*"and r7, r2, r6" "\n\t"*/ /* (r11, r12) */
        "and r11, r2, r10" "\n\t"
        "eor r11, r12, r11" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "and %[buffer], r2, r14" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "and %[buffer], r6, r10" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "and %[buffer], r6, r14" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"


        /*"eor r7, r7, r5" "\n\t"*/ /* (r11, r0) */
        "eor r11, r11, r5" "\n\t"
        "eor %[buffer], r12, r9" "\n\t"


        /*"eor r3, r7, r4" "\n\t"*/ /* (r3, r7) */
        "eor r3, r11, r4" "\n\t"
        "eor r7, %[buffer], r8" "\n\t"


        /*"orn r7, r6, r3" "\n\t"*/ /* (r11, r12) */
        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "bic %[buffer], r10, r3" "\n\t"
        "eor r11, r12, %[buffer]" "\n\t"

        "orr %[buffer], r10, r7" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "orn %[buffer], r14, r3" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "and %[buffer], r14, r7" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"


        /*"eor r5, r8, r7" "\n\t"*/ /* (r5, r9) */
        "pop {%[buffer], %[rk_masked]}" "\n\t"
        "eor r5, %[buffer], r11" "\n\t"
        "eor r9, %[rk_masked], r12" "\n\t"


        /*"orn r7, r5, r3" "\n\t"*/ /* (r11, r12) */
        "bic %[buffer], r5, r3" "\n\t"
        "mov r11, 0" "\n\t" /* prevent HD leakage */
        "eor r11, r12, %[buffer]" "\n\t"

        "orr %[buffer], r5, r7" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "mov %[buffer], 0" "\n\t" /* prevent HD leakage */
        "orn %[buffer], r9, r3" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"

        "and %[buffer], r9, r7" "\n\t"
        "eor r11, r11, %[buffer]" "\n\t"


        /*"eor r2, r6, r7" "\n\t"*/ /* (r2, r6) */
        "eor r2, r10, r11" "\n\t"
        "eor r6, r14, r12" "\n\t"


        "pop {%[rk_masked], r14}" "\n\t"


        /* loop end */
        "subs r14, r14, #1" "\n\t"
        "bne dec_step" "\n\t"





	"pop {%[buffer]}" CR
	/* load key */
	"ldm %[rk_masked], {r10, r11}" CR
	"eor r6, r6, r10" CR
	"eor r7, r7, r10, lsr #16" CR
	"eor r8, r8, r11" CR
	"eor r9, r9, r11, lsr #16" CR
	
	"sub r1, r1, #8" "\n\t"
	
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r2, r2, r10" CR
	"eor r3, r3, r10, lsr #16" CR
	"eor r4, r4, r11" CR
	"eor r5, r5, r11, lsr #16" CR
	/* store results */
	"bfi r2, r3, #16, #16" CR
	"bfi r4, r5, #16, #16" CR
	"bfi r6, r7, #16, #16" CR
	"bfi r8, r9, #16, #16" CR
	"stm %[buffer], {r2, r4, r6, r8}" CR
	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14"
	);
}
