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
    mov r_mask , 0                                                /* prevent HD leakage */ \n\t \
    and r_mask, x_mask, y_mval                                    \n\t \
    mov temp, 0                                                   \n\t /* <- prevent HD leakage */ \
    orn temp, x_mask, y_mask                                      \n\t \
    eor r_mask, r_mask, temp                                      \n\t

#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    and temp, x_mval, y_mval                                       \n\t \
    orn x_mval, x_mval, y_mask                                     \n\t \
    eor x_mval, x_mval, temp                                       \n\t \
	mov temp, 0                                                    \n\t /* <- prevent HD leakage */ \
    and temp, x_mask, y_mval                                       \n\t \
    orn r_mask, x_mask, y_mask                                     \n\t \
    eor r_mask, r_mask, temp                                       \n\t

#define SEC_OR_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    mov temp , 0                                                /* prevent HD leakage */ \n\t \
    orr temp, x_mval, y_mask                                      \n\t \
    and x_mval, x_mval, y_mval                                    \n\t \
    eor x_mval, x_mval, temp                                      \n\t \
                                                                       \
    mov temp , 0                                                /* prevent HD leakage */ \n\t \
    orr temp, x_mask, y_mval                                      \n\t \
    mov r_mask , 0                                                /* prevent HD leakage */ \n\t \
    and r_mask, x_mask, y_mask                                    \n\t \
    eor r_mask, r_mask, temp                                      \n\t


#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val) \
    lsl r_mval, x_mval, val                        \n\t \
    lsl r_mask, x_mask, val                        \n\t

#define REMASK_(x, new_mask, old_mask) \
    eor x, x, new_mask            \r\n \
    eor x, x, old_mask            \r\n

#define CLEAR_REG_(reg) mov reg, 0 \n\t

#define CLEAR_REG(reg) STR(CLEAR_REG_(reg))


#define PUSH_REGISTER_(x) \
    push {x}          \n\t

#define POP_REGISTER_(x) \
    pop {x}          \n\t


#define SEC_SUB_BEGIN_(l_ctr) \
    mov l_ctr, CV1       \n\t \
    sec_sub_iteration:   \n\t


#define SEC_SUB_END_(l_ctr)    \
    lsl l_ctr, l_ctr, CV1 \n\t \
                               \
    cmp l_ctr, CV16       \n\t \
    bne sec_sub_iteration \n\t


#define SEC_SUB_INITIALIZE_(x) \
    stmdb sp!, {x}        \n\t \
    mvn x, x              \n\t


#define SEC_SUB_FINALIZE_(x_mval, x_mask, y_mval, y_mask, u, g, l_ctr) \
    /* z(mval, mask) = (u, y_mask) */                             \n\t \
    lsl l_ctr, g, CV1                                             \n\t \
    orr l_ctr, CV1                                                \n\t \
    eor u, u, l_ctr                                               \n\t \
    eor u, u, x_mask, lsl CV1                                     \n\t \
                                                                  \n\t \
    /* Change mask */                                             \n\t \
    /* z(mval, mask) = (x_mval, x_mask) */                        \n\t \
    eor x_mval, u, x_mask                                         \n\t \
    eor x_mval, x_mval, y_mask                                    \n\t \
                                                                  \n\t \
    ldmia sp!, {y_mval}                                           \n\t


#define SEC_SUB(x_mval, x_mask, y_mval, y_mask, a, b, p, g, h, u, t, l_ctr) \
    SEC_SUB_INITIALIZE(y_mval)                                           \
                                                                         \
    /* p(mval, mask) = (p, x_mask) */                                    \
    /*SEC_XOR(p, x_mval, x_mask, y_mval, y_mask)*/                       \
    SEC_XOR(p, y_mval, y_mask, x_mval, x_mask)                           \
                                                                         \
    /* p(mval, mask) = (p, y_mask) */                                    \
    /*REMASK(p, y_mask, x_mask)*/                                        \
                                                                         \
    /* g(mval, mask) = (g, a) */                                         \
    SEC_AND(g, a, x_mval, x_mask, y_mval, y_mask, l_ctr)                 \
                                                                         \
    /* g(mval, mask) = (g, x_mask) */                                    \
    REMASK(g, x_mask, a)                                                 \
                                                                         \
                                                                         \
    PUSH_REGISTER(y_mval)                                                \
                                                                         \
                                                                         \
    SEC_SUB_BEGIN(l_ctr)                                                 \
                                                                         \
                                                                         \
    /* h(mval, mask) = (h, b) */                                         \
    SEC_SHIFT_1S(h, b, g, x_mask, l_ctr, t, y_mval)                      \
                                                                         \
    /* u(mval, mask) = (u, a) */                                         \
    SEC_AND(u, a, p, y_mask, h, b, t)                                    \
                                                                         \
    /* g(mval, mask) = (g, b) */                                         \
    SEC_OR_IN_PLACE(b, g, x_mask, u, a, t)                               \
                                                                         \
    /* g(mval, mask) = (g, x_mask) */                                    \
    REMASK(g, x_mask, b)                                                 \
                                                                         \
    /* h(mval, mask) = (h, b) */                                         \
    SEC_SHIFT(h, b, p, y_mask, l_ctr)                                    \
                                                                         \
    /* h(mval, mask) = (h, x_mask) */                                    \
    REMASK(h, x_mask, b)                                                 \
                                                                         \
    /* p(mval, mask) = (p, a) */                                         \
    SEC_AND_IN_PLACE(a, p, y_mask, h, x_mask, b)                         \
                                                                         \
    /* p(mval, mask) = (p, y_mask) */                                    \
    REMASK(p, y_mask, a)                                                 \
                                                                         \
                                                                         \
    SEC_SUB_END(l_ctr)                                                   \
                                                                         \
                                                                         \
    /* h(mval, mask) = (h, b) */                                         \
    SEC_SHIFT_1S(h, b, g, x_mask, l_ctr, t, y_mval)                      \
                                                                         \
    /* u(mval, mask) = (u, a) */                                         \
    SEC_AND(u, a, p, y_mask, h, b, l_ctr)                                \
                                                                         \
    /* g(mval, mask) = (g, b) */                                         \
    SEC_OR_IN_PLACE(b, g, x_mask, u, a, t)                               \
                                                                         \
    /* g(mval, mask) = (g, x_mask) */                                    \
    REMASK(g, x_mask, b)                                                 \
                                                                         \
                                                                         \
    POP_REGISTER(y_mval)                                                 \
                                                                         \
                                                                         \
    /* z(mval, mask) = (u, y_mask) */                                    \
    SEC_XOR(u, y_mval, y_mask, x_mval, x_mask)                           \
                                                                         \
                                                                         \
    SEC_SUB_FINALIZE(x_mval, x_mask, y_mval, y_mask, u, g, l_ctr)


#define SEC_SHIFT_1S_(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2) \
    lsl r_mval, x_mval, val                                         \n\t \
                                                                    \n\t \
    ldr temp1, =CV32                                                \n\t \
    sub temp2, temp1, val                                           \n\t \
    ldr temp1, =0xffffffff                                          \n\t \
    lsr temp1, temp1, temp2                                         \n\t \
                                                                    \n\t \
    orr r_mval, r_mval, temp1                                       \n\t \
                                                                         \
    lsl r_mask, x_mask, val                                         \n\t



#define PUSH_REGISTER(x) \
    STR(PUSH_REGISTER_(x))

#define POP_REGISTER(x) \
    STR(POP_REGISTER_(x))

#define SEC_SHIFT_1S(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2) \
    STR(SEC_SHIFT_1S_(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2))

#define SEC_SUB_BEGIN(l_ctr) \
    STR(SEC_SUB_BEGIN_(l_ctr))

#define SEC_SUB_END(l_ctr) \
    STR(SEC_SUB_END_(l_ctr))

#define SEC_SUB_INITIALIZE(x) \
    STR(SEC_SUB_INITIALIZE_(x))

#define SEC_SUB_FINALIZE(x_mval, x_mask, y_mval, y_mask, u, g, l_ctr) \
    STR(SEC_SUB_FINALIZE_(x_mval, x_mask, y_mval, y_mask, u, g, l_ctr))

#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_OR_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_OR_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_SHIFT(r_mval, r_mask, x_mval, x_mask, val) \
    STR(SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val))

#define REMASK(x, new_mask, old_mask) \
    STR(REMASK_(x, new_mask, old_mask))

#endif /* ARM_MACROS_H */
