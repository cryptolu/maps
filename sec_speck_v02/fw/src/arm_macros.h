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
#define CV16 #16


#define SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    and temp, x_mval, y_mval                                      \n\t \
	mov r_mval, 0                                                 \n\t /* prevent leakage of next instruction */ \
    eor r_mval, r_mask, temp                                      \n\t \
	mov temp, 0                                                   \n\t /* prevent leakage of next instruction */ \
    and temp, x_mval, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
	mov temp, 0                                                   \n\t /* prevent leakage of next instruction */ \
    and temp, x_mask, y_mval                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
	mov temp, 0                                                   \n\t /* prevent leakage of next instruction */ \
    and temp, x_mask, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t

#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2) \
	mov temp1, 0                                                           \n\t /* prevent leakage of next instruction */ \
    and temp1, x_mval, y_mval                                              \n\t \
    and temp2, x_mval, y_mask                                              \n\t \
    eor x_mval, r_mask, temp1                                              \n\t \
    eor x_mval, x_mval, temp2                                              \n\t \
	mov temp1, 0                                                           \n\t /* prevent leakage of next instruction */ \
    and temp1, x_mask, y_mval                                              \n\t \
    eor x_mval, x_mval, temp1                                              \n\t \
	mov temp1, 0                                                           \n\t /* prevent leakage of next instruction */ \
    and temp1, x_mask, y_mask                                              \n\t \
    eor x_mval, x_mval, temp1                                              \n\t

#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val, temp) \
    lsl temp, x_mval, val                                \n\t \
    eor r_mval, r_mask, temp                             \n\t \
	mov temp, 0                                          \n\t /* prevent leakage of next instruction */ \
    lsl temp, x_mask, val                                \n\t \
    eor r_mval, r_mval, temp                             \n\t

#define REMASK_(x, new_mask, old_mask) \
    eor x, x, new_mask            \r\n \
    eor x, x, old_mask            \r\n

#define PUSH_REGISTER_(x) \
    push {x}          \n\t

#define POP_REGISTER_(x) \
    pop {x}          \n\t

#define CLEAR_REGISTER_(x) \
	mov x, 0          \n\t

#define SEC_ADD_BEGIN_(l_ctr) \
    mov l_ctr, 1       \n\t \
    sec_add_iteration:   \n\t

#define SEC_ADD_END_(l_ctr)    \
    lsl l_ctr, l_ctr, CV1 \n\t \
    cmp l_ctr, CV16       \n\t \
    bne sec_add_iteration \n\t

#define SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g) \
    /* z(mval, mask) = (u, y_mask) */              \n\t \
    eor u, u, g, lsl CV1                           \n\t \
    eor u, u, x_mask, lsl CV1                      \n\t \
    /* Change mask */                              \n\t \
    /* z(mval, mask) = (u, x_mask) */              \n\t \
    eor x_mval, u, x_mask                          \n\t \
    eor x_mval, x_mval, y_mask                     \n\t

#define SEC_ADD(x_mval, x_mask, y_mval, y_mask, a, b, p, g, h, u, t, l_ctr) \
        /* p(mval, mask) = (p, x_mask) */                                \
        SEC_XOR(p, x_mval, x_mask, y_mval, y_mask)                       \
        /* g(mval, mask) = (g, a) */                                     \
        SEC_AND(g, a, x_mval, x_mask, y_mval, y_mask, l_ctr)             \
        /* g(mval, mask) = (g, x_mask) */                                \
        REMASK(g, x_mask, a)                                             \
        PUSH_REGISTER(y_mask)                                            \
        SEC_ADD_BEGIN(l_ctr)                                             \
        /* h(mval, mask) = (h, b) */                                     \
		CLEAR_REGISTER(h)                                                \
        SEC_SHIFT(h, b, g, x_mask, l_ctr, y_mask)                        \
        /* u(mval, mask) = (u, a) */                                     \
		CLEAR_REGISTER(u)                                                \
        SEC_AND(u, a, p, x_mask, h, b, y_mask)                           \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, p, x_mask, l_ctr, y_mask)                        \
        /* p(mval, mask) = (p, a) */                                     \
        SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, y_mask)                  \
        /* p(mval, mask) = (p, x_mask) */                                \
        REMASK(p, x_mask, a)                                             \
        SEC_ADD_END(l_ctr)                                               \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, g, x_mask, l_ctr, y_mask)                        \
        /* u(mval, mask) = (u, a) */                                     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        POP_REGISTER(y_mask)                                             \
        /* z(mval, mask) = (u, y_mask) */                                \
        SEC_XOR(u, y_mval, y_mask, x_mval, x_mask)                       \
        SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g)



#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2))

#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_SHIFT(r_mval, r_mask, x_mval, x_mask, val, temp) \
    STR(SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val, temp))

#define REMASK(x, new_mask, old_mask) \
    STR(REMASK_(x, new_mask, old_mask))

#define PUSH_REGISTER(x) \
    STR(PUSH_REGISTER_(x))

#define POP_REGISTER(x) \
    STR(POP_REGISTER_(x))

#define CLEAR_REGISTER(x) \
	STR(CLEAR_REGISTER_(x))

#define SEC_ADD_BEGIN(l_ctr) \
    STR(SEC_ADD_BEGIN_(l_ctr))

#define SEC_ADD_END(l_ctr) \
    STR(SEC_ADD_END_(l_ctr))

#define SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g) \
    STR(SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g))

#endif /* ARM_MACROS_H */
