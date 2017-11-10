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
#define CV32 #32


#define SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    nop                                                   \n\t /* prevent HD leakage */ \
    nop                                                   \n\t /* prevent HD leakage */ \
    nop                                                   \n\t /* prevent HD leakage */ \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    and temp, x_mval, y_mval                                      \n\t \
    eor r_mval, r_mask, temp                                      \n\t \
                                                                       \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    and temp, x_mval, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
                                                                       \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    and temp, x_mask, y_mval                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
                                                                       \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    and temp, x_mask, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t


#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2) \
    and temp1, x_mval, y_mval                                              \n\t \
    and temp2, x_mval, y_mask                                              \n\t \
                                                                                \
    eor x_mval, r_mask, temp1                                              \n\t \
    eor x_mval, x_mval, temp2                                              \n\t \
                                                                                \
    mov temp1, 0                                                   \n\t /* prevent HD leakage */ \
    and temp1, x_mask, y_mval                                              \n\t \
    eor x_mval, x_mval, temp1                                              \n\t \
                                                                                \
    mov temp1, 0                                                   \n\t /* prevent HD leakage */ \
    and temp1, x_mask, y_mask                                              \n\t \
    eor x_mval, x_mval, temp1                                              \n\t


#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define SEC_OR_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    orr temp, x_mval, y_mask                                      \n\t \
                                                                  \n\t \
    and x_mval, x_mval, y_mval                                    \n\t \
    eor x_mval, x_mval, r_mask                                    \n\t \
                                                                       \
    eor x_mval, x_mval, temp                                      \n\t \
                                                                       \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    orr temp, x_mask, y_mval                                      \n\t \
    eor x_mval, x_mval, temp                                      \n\t \
                                                                       \
    and temp, x_mask, y_mask                                      \n\t \
    eor x_mval, x_mval, temp                                      \n\t


/* Error: shift must be constant */
//#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val) \
//    eor r_mval, r_mask, x_mval, lsl val            \n\t \
//    eor r_mval, r_mval, x_mask, lsl val            \n\t

#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val, temp) \
    lsl temp, x_mval, val                                \n\t \
    eor r_mval, r_mask, temp                             \n\t \
    mov temp, 0                                                   \n\t /* prevent HD leakage */ \
    lsl temp, x_mask, val                                \n\t \
    eor r_mval, r_mval, temp                             \n\t


/*WRONG!*/
//#define SEC_SHIFT_1S_(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2) \
//    lsl temp1, x_mval, val                                          \n\t \
//    mul temp2, val, val                                             \n\t \
//    sub temp2, temp2, CV1                                           \n\t \
//    orr temp2, temp2, CV1 /* fix case val=1, sh=1 */                \n\t \
//    orr temp1, temp1, temp2                                         \n\t \
//                                                                         \
//    eor r_mval, r_mask, temp1                                       \n\t \
//    lsl temp1, x_mask, val                                          \n\t \
//    eor r_mval, r_mval, temp1                                       \n\t

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
    eor r_mval, r_mask, r_mval                                      \n\t \
    lsl temp1, x_mask, val                                          \n\t \
    eor r_mval, r_mval, temp1                                       \n\t


#define SEC_NOT_(x_mval, x_mask) \
    mvn x_mval, x_mval      \n\t


#define REMASK_(x, new_mask, old_mask) \
    eor x, x, new_mask            \r\n \
    eor x, x, old_mask            \r\n


#define PUSH_REGISTER_(x) \
    push {x}          \n\t

#define POP_REGISTER_(x) \
    pop {x}          \n\t


#define PUSH_REGISTERS_(x, y) \
    push {x, y}          \n\t

#define POP_REGISTERS_(x, y) \
    pop {x, y}          \n\t



#define SEC_ADD_BEGIN_(l_ctr) \
    mov l_ctr, CV1       \n\t \
    sec_add_iteration:   \n\t


#define SEC_ADD_END_(l_ctr)    \
    lsl l_ctr, l_ctr, CV1 \n\t \
                               \
    cmp l_ctr, CV16       \n\t \
    bne sec_add_iteration \n\t


#define SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g) \
    /* z(mval, mask) = (u, y_mask) */              \n\t \
    eor u, u, g, lsl CV1                           \n\t \
    eor u, u, x_mask, lsl CV1                      \n\t \
                                                   \n\t \
    /* Change mask */                              \n\t \
    /* z(mval, mask) = (u, x_mask) */              \n\t \
    eor x_mval, u, x_mask                          \n\t \
    eor x_mval, x_mval, y_mask                     \n\t


#define SEC_ADD(x_mval, x_mask, y_mval, y_mask, a, b, p, g, h, u, t, l_ctr) \
        /* p(mval, mask) = (p, x_mask) */                                \
        SEC_XOR(p, x_mval, x_mask, y_mval, y_mask)                       \
                                                                         \
        /* g(mval, mask) = (g, a) */                                     \
        SEC_AND(g, a, x_mval, x_mask, y_mval, y_mask, l_ctr)             \
                                                                         \
        /* g(mval, mask) = (g, x_mask) */                                \
        REMASK(g, x_mask, a)                                             \
                                                                         \
                                                                         \
        PUSH_REGISTER(y_mask)                                            \
                                                                         \
                                                                         \
        SEC_ADD_BEGIN(l_ctr)                                             \
                                                                         \
                                                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, g, x_mask, l_ctr, y_mask)                        \
                                                                         \
        /* u(mval, mask) = (u, a) */                                     \
        SEC_AND(u, a, p, x_mask, h, b, y_mask)                           \
                                                                         \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
                                                                         \
                                                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, p, x_mask, l_ctr, y_mask)                        \
                                                                         \
        /* p(mval, mask) = (p, a) */                                     \
        SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, y_mask)                  \
                                                                         \
                                                                         \
        /* p(mval, mask) = (p, x_mask) */                                \
        REMASK(p, x_mask, a)                                             \
                                                                         \
                                                                         \
        SEC_ADD_END(l_ctr)                                               \
                                                                         \
                                                                         \
        /* h(mval, mask) = (h, b) */                                     \
        SEC_SHIFT(h, b, g, x_mask, l_ctr, y_mask)                        \
                                                                         \
        /* u(mval, mask) = (u, a) */                                     \
        SEC_AND(u, a, p, x_mask, h, b, l_ctr)                            \
                                                                         \
        /* g(mval, mask) = (g, x_mask) */                                \
        SEC_XOR(g, g, x_mask, u, a)                                      \
                                                                         \
                                                                         \
        POP_REGISTER(y_mask)                                             \
                                                                         \
                                                                         \
        /* z(mval, mask) = (u, y_mask) */                                \
        SEC_XOR(u, y_mval, y_mask, x_mval, x_mask)                       \
                                                                         \
                                                                         \
        SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g)



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


#define SEC_SUB_FINALIZE_(x_mval, x_mask, y_mval, y_mask, u, g, b, l_ctr) \
    /* z(mval, mask) = (u, y_mask) */                                \n\t \
    lsl l_ctr, g, CV1                                                \n\t \
    orr l_ctr, CV1                                                   \n\t \
    eor u, u, l_ctr                                                  \n\t \
    eor u, u, b, lsl CV1                                             \n\t \
                                                                     \n\t \
    /* Change mask */                                                \n\t \
    /* z(mval, mask) = (x_mval, x_mask) */                           \n\t \
    eor x_mval, u, x_mask                                            \n\t \
    eor x_mval, x_mval, y_mask                                       \n\t \
                                                                     \n\t \
    ldmia sp!, {y_mval}                                              \n\t


#define SEC_SUB(x_mval, x_mask, y_mval, y_mask, a, b, p, g, h, u, t, l_ctr) \
    SEC_SUB_INITIALIZE(y_mval)                                           \
                                                                         \
    /* p(mval, mask) = (p, x_mask) */                                    \
    SEC_XOR(p, x_mval, x_mask, y_mval, y_mask)                           \
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
    /* u(mval, mask) = (u, y_mask) */                                    \
    SEC_AND(u, y_mask, p, x_mask, h, b, t)                               \
                                                                         \
    /* g(mval, mask) = (g, a) */                                         \
    SEC_OR_IN_PLACE(a, g, x_mask, u, y_mask, t)                          \
                                                                         \
                                                                         \
    /* g(mval, mask) = (g, x_mask) */                                    \
    REMASK(g, x_mask, a)                                                 \
                                                                         \
                                                                         \
    /* h(mval, mask) = (h, b) */                                         \
    SEC_SHIFT(h, b, p, x_mask, l_ctr, t)                                 \
                                                                         \
    /* p(mval, mask) = (p, a) */                                         \
    SEC_AND_IN_PLACE(a, p, x_mask, h, b, t, y_mval)                      \
                                                                         \
                                                                         \
    /* p(mval, mask) = (p, x_mask) */                                    \
    REMASK(p, x_mask, a)                                                 \
                                                                         \
                                                                         \
    SEC_SUB_END(l_ctr)                                                   \
                                                                         \
                                                                         \
    /* h(mval, mask) = (h, b) */                                         \
    SEC_SHIFT_1S(h, b, g, x_mask, l_ctr, t, y_mval)                      \
                                                                         \
    /* u(mval, mask) = (u, y_mask) */                                    \
    SEC_AND(u, y_mask, p, x_mask, h, b, l_ctr)                           \
                                                                         \
    /* g(mval, mask) = (g, b) */                                         \
    SEC_OR_IN_PLACE(b, g, x_mask, u, y_mask, t)                          \
                                                                         \
                                                                         \
    POP_REGISTER(y_mval)                                                 \
                                                                         \
                                                                         \
    /* z(mval, mask) = (u, y_mask) */                                    \
    SEC_XOR(u, y_mval, y_mask, x_mval, x_mask)                           \
                                                                         \
                                                                         \
    SEC_SUB_FINALIZE(x_mval, x_mask, y_mval, y_mask, u, g, b, l_ctr)



#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp1, temp2))

#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_OR_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_OR_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_SHIFT(r_mval, r_mask, x_mval, x_mask, val, temp) \
    STR(SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val, temp))

#define SEC_SHIFT_1S(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2) \
    STR(SEC_SHIFT_1S_(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2))

#define SEC_NOT(x_mval, x_mask) \
    STR(SEC_NOT_(x_mval, x_mask))

#define REMASK(x, new_mask, old_mask) \
    STR(REMASK_(x, new_mask, old_mask))

#define PUSH_REGISTER(x) \
    STR(PUSH_REGISTER_(x))

#define POP_REGISTER(x) \
    STR(POP_REGISTER_(x))

#define PUSH_REGISTERS(x, y) \
    STR(PUSH_REGISTERS_(x, y))

#define POP_REGISTERS(x, y) \
    STR(POP_REGISTERS_(x, y))

#define SEC_ADD_BEGIN(l_ctr) \
    STR(SEC_ADD_BEGIN_(l_ctr))

#define SEC_ADD_END(l_ctr) \
    STR(SEC_ADD_END_(l_ctr))

#define SEC_ADD_FINALIZE(x_mval, x_mask, y_mask, u, g) \
    STR(SEC_ADD_FINALIZE_(x_mval, x_mask, y_mask, u, g))

#define SEC_SUB_BEGIN(l_ctr) \
    STR(SEC_SUB_BEGIN_(l_ctr))

#define SEC_SUB_END(l_ctr) \
    STR(SEC_SUB_END_(l_ctr))

#define SEC_SUB_INITIALIZE(x) \
    STR(SEC_SUB_INITIALIZE_(x))

#define SEC_SUB_FINALIZE(x_mval, x_mask, y_mval, y_mask, u, g, b, l_ctr) \
    STR(SEC_SUB_FINALIZE_(x_mval, x_mask, y_mval, y_mask, u, g, b, l_ctr))


#endif /* ARM_MACROS_H */
