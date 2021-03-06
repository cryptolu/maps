/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Daniel Dinu <dumitru-daniel.dinu@uni.lu>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ARM_MACROS_H
#define ARM_MACROS_H


#include "stringify.h"


#define CV1 #1
#define CV2 #2
#define CV4 #4
#define CV8 #8
#define CV16 #16


#define SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
	mov temp, 0                                                   \n\t /* prevent leakage for next instruction */ \
    and temp, x_mval, y_mval                                      \n\t \
    eor r_mval, r_mask, temp                                      \n\t \
	mov temp, 0                                                   \n\t /* prevent leakage for next instruction */ \
    and temp, x_mval, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
	mov temp, 0                                                   \n\t /* prevent leakage for next instruction */ \
    and temp, x_mask, y_mval                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
	mov temp, 0                                                   \n\t /* prevent leakage for next instruction */ \
    and temp, x_mask, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t

#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2) \
    and temp1, x_mval, y_mval                                              \n\t \
	mov temp2, 0                                                           \n\t /* prevent leakage for next instruction */ \
    and temp2, x_mval, y_mask                                              \n\t \
    eor x_mval, r_mask, temp1                                              \n\t \
    eor x_mval, x_mval, temp2                                              \n\t \
	mov temp1, 0                                                           \n\t /* prevent leakage for next instruction */ \
    and temp1, x_mask, y_mval                                              \n\t \
    eor x_mval, x_mval, temp1                                              \n\t \
	mov temp1, 0                                                           \n\t /* prevent leakage for next instruction */ \
    and temp1, x_mask, y_mask                                              \n\t \
    eor x_mval, x_mval, temp1                                              \n\t

#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define REMASK_(x, new_mask, old_mask) \
    eor x, x, new_mask            \r\n \
    eor x, x, old_mask            \r\n

#define SEC_SHIFT_1_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    eor r_mval, r_mask, x_mval, lsl CV1                   \n\t \
    eor r_mval, r_mval, x_mask, lsl CV1                   \n\t

#define SEC_SHIFT_2_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    eor r_mval, r_mask, x_mval, lsl CV2                   \n\t \
    eor r_mval, r_mval, x_mask, lsl CV2                   \n\t

#define SEC_SHIFT_3_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    eor r_mval, r_mask, x_mval, lsl CV4                   \n\t \
    eor r_mval, r_mval, x_mask, lsl CV4                   \n\t

#define SEC_SHIFT_4_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    eor r_mval, r_mask, x_mval, lsl CV8                   \n\t \
    eor r_mval, r_mval, x_mask, lsl CV8                   \n\t

#define SEC_SHIFT_5_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    eor r_mval, r_mask, x_mval, lsl CV16                  \n\t \
    eor r_mval, r_mval, x_mask, lsl CV16                  \n\t

#define SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g) \
    /* z(mval, mask) = (u, y_mask) */              \n\t \
    eor u, u, g, lsl CV1                           \n\t \
    eor u, u, x_mask, lsl CV1                      \n\t \
    /* Change mask */                              \n\t \
    /* z(mval, mask) = (u, x_mask) */              \n\t \
    eor x_mval, u, x_mask                          \n\t \
    eor x_mval, x_mval, y_mask                     \n\t

#define CLEAR_REGISTER_(x) \
	mov x, 0         \n\t


#define SEC_ADD(x_mval, x_mask, y_mval, y_mask, a, b, p, g, h, u, t, l_ctr) \
        /* p(mval, mask) = (p, x_mask) */                                \
        SEC_XOR(p, x_mval, x_mask, y_mval, y_mask)                       \
        /* g(mval, mask) = (g, a) */                                     \
        SEC_AND(g, a, x_mval, x_mask, y_mval, y_mask, l_ctr)             \
        /* g(mval, mask) = (g, x_mask) */                                \
        REMASK(g, x_mask, a)                                             \
        /* First loop - begin */                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_1_ITERATION(h, b, g, x_mask)                           \
        /* u(mval, mask) = (u, a) */                                     \
		CLEAR_REGISTER(u) /* prevent leakage for next instruction */     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_1_ITERATION(h, b, p, x_mask)                           \
        /* p(mval, mask) = (p, a) */                                     \
        SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, l_ctr)                   \
        /* p(mval, mask) = (p, x_mask) */                                \
        REMASK(p, x_mask, a)                                             \
        /* First loop - end */                                           \
        /* Second loop - begin */                                        \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_2_ITERATION(h, b, g, x_mask)                           \
        /* u(mval, mask) = (u, a) */                                     \
		CLEAR_REGISTER(u) /* prevent leakage for next instruction */     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_2_ITERATION(h, b, p, x_mask)                           \
        /* p(mval, mask) = (p, a) */                                     \
		CLEAR_REGISTER(t) /* prevent leakage for next instruction */     \
        SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, l_ctr)                   \
        /* p(mval, mask) = (p, x_mask) */                                \
        REMASK(p, x_mask, a)                                             \
        /* Second loop - end */                                          \
        /* Third loop - begin */                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_3_ITERATION(h, b, g, x_mask)                           \
        /* u(mval, mask) = (u, a) */                                     \
		CLEAR_REGISTER(u) /* prevent leakage for next instruction */     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_3_ITERATION(h, b, p, x_mask)                           \
        /* p(mval, mask) = (p, a) */                                     \
		CLEAR_REGISTER(t) /* prevent leakage for next instruction */     \
        SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, l_ctr)                   \
        /* p(mval, mask) = (p, x_mask) */                                \
        REMASK(p, x_mask, a)                                             \
        /* Third loop - end */                                           \
        /* Fourth loop - begin */                                        \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_4_ITERATION(h, b, g, x_mask)                           \
        /* u(mval, mask) = (u, a) */                                     \
		CLEAR_REGISTER(u) /* prevent leakage for next instruction */     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_4_ITERATION(h, b, p, x_mask)                           \
        /* p(mval, mask) = (p, a) */                                     \
		CLEAR_REGISTER(t) /* prevent leakage for next instruction */     \
        SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, l_ctr)                   \
        /* p(mval, mask) = (p, x_mask) */                                \
        REMASK(p, x_mask, a)                                             \
        /* Fourth loop - end */                                          \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT_5_ITERATION(h, b, g, x_mask)                           \
        /* u(mval, mask) = (u, a) */                                     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* z(mval, mask) = (u, y_mask) */                                \
        SEC_XOR(u, y_mval, y_mask, x_mval, x_mask)                       \
        SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g)


#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2))

#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_SHIFT_1_ITERATION(r_mval, r_mask, x_mval, x_mask) \
    STR(SEC_SHIFT_1_ITERATION_(r_mval, r_mask, x_mval, x_mask))

#define SEC_SHIFT_2_ITERATION(r_mval, r_mask, x_mval, x_mask) \
    STR(SEC_SHIFT_2_ITERATION_(r_mval, r_mask, x_mval, x_mask))

#define SEC_SHIFT_3_ITERATION(r_mval, r_mask, x_mval, x_mask) \
    STR(SEC_SHIFT_3_ITERATION_(r_mval, r_mask, x_mval, x_mask))

#define SEC_SHIFT_4_ITERATION(r_mval, r_mask, x_mval, x_mask) \
    STR(SEC_SHIFT_4_ITERATION_(r_mval, r_mask, x_mval, x_mask))

#define SEC_SHIFT_5_ITERATION(r_mval, r_mask, x_mval, x_mask) \
    STR(SEC_SHIFT_5_ITERATION_(r_mval, r_mask, x_mval, x_mask))

#define REMASK(x, new_mask, old_mask) \
    STR(REMASK_(x, new_mask, old_mask))

#define CLEAR_REGISTER(x) \
    STR(CLEAR_REGISTER_(x))

#define SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g) \
    STR(SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g))

#endif /* ARM_MACROS_H */
