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
    and r_mval, x_mval, y_mval                                    \n\t \
    orn temp, x_mval, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
    and r_mask, x_mask, y_mval                                    \n\t \
	mov temp, 0                                                   \n\t /* <- corrected leakage */ \
    orn temp, x_mask, y_mask                                      \n\t \
    eor r_mask, r_mask, temp                                      \n\t

#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    and temp, x_mval, y_mval                                       \n\t \
    orn x_mval, x_mval, y_mask                                     \n\t \
    eor x_mval, x_mval, temp                                       \n\t \
	mov temp, 0                                                    \n\t /* <- corrected leakage */ \
    and temp, x_mask, y_mval                                       \n\t \
    orn r_mask, x_mask, y_mask                                     \n\t \
    eor r_mask, r_mask, temp                                       \n\t

#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val) \
    lsl r_mval, x_mval, val                        \n\t \
    lsl r_mask, x_mask, val                        \n\t

#define REMASK_(x, new_mask, old_mask) \
    eor x, x, new_mask            \r\n \
    eor x, x, old_mask            \r\n

#define SEC_ADD_BEGIN_(l_ctr) \
    mov l_ctr, CV1       \n\t \
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

#define CLEAR_REG_(reg) mov reg, 0 \n\t

#define CLEAR_REG(reg) STR(CLEAR_REG_(reg))


#define SEC_ADD(x_mval, x_mask, y_mval, y_mask, a, b, p, g, h, u, t, l_ctr) \
        /* p(mval, mask) = (p, x_mask) */                                \
        /*SEC_XOR(p, x_mval, x_mask, y_mval, y_mask)*/                   \
        SEC_XOR(p, y_mval, y_mask, x_mval, x_mask)                       \
        /* g(mval, mask) = (g, a) */                                     \
        SEC_AND(g, a, x_mval, x_mask, y_mval, y_mask, l_ctr)             \
        /* g(mval, mask) = (g, x_mask) */                                \
        REMASK(g, x_mask, a)                                             \
                                                                         \
        SEC_ADD_BEGIN(l_ctr)                                             \
                                                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, g, x_mask, l_ctr)                                \
        /* u(mval, mask) = (u, a) */                                     \
		CLEAR_REG(t)                                                     /* <- correct leakage */ \
		CLEAR_REG(a)                                                     /* <- correct leakage */ \
        SEC_AND(u, a, p, y_mask, h, b, t)                                \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, p, y_mask, l_ctr)                                \
        /* h(mval, mask) = (h, x_mask) */                                \
        REMASK(h, x_mask, b)                                             \
        /* p(mval, mask) = (p, a) */                                     \
        SEC_AND_IN_PLACE(a, p, y_mask, h, x_mask, b)                     \
        /* p(mval, mask) = (p, y_mask) */                                \
        REMASK(p, y_mask, a)                                             \
                                                                         \
        SEC_ADD_END(l_ctr)                                               \
                                                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, g, x_mask, l_ctr)                                \
        /* u(mval, mask) = (u, a) */                                     \
        SEC_AND(u, a, p, y_mask, h, b, l_ctr)                            \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
        /* z(mval, mask) = (u, y_mask) */                                \
        SEC_XOR(u, y_mval, y_mask, x_mval, x_mask)                       \
																		 \
        SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g)


#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_SHIFT(r_mval, r_mask, x_mval, x_mask, val) \
    STR(SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val))

#define REMASK(x, new_mask, old_mask) \
    STR(REMASK_(x, new_mask, old_mask))

#define SEC_ADD_BEGIN(l_ctr) \
    STR(SEC_ADD_BEGIN_(l_ctr))

#define SEC_ADD_END(l_ctr) \
    STR(SEC_ADD_END_(l_ctr))

#define SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g) \
    STR(SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g))

#endif /* ARM_MACROS_H */
