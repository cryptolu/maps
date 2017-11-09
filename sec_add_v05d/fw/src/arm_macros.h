/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * arm_v7m_leakage simulator
 *
 * Copyright (C) 2017 University of Luxembourg
 *
 * Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu>
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

/*
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


#define SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    and r_mval, x_mval, y_mval                                    \n\t \
    orn temp, x_mval, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
                                                                       \
    and r_mask, x_mask, y_mval                                    \n\t \
	mov temp, 0                                                   \n\t /* <- remove leakage */ \
    orn temp, x_mask, y_mask                                      \n\t \
    eor r_mask, r_mask, temp                                      \n\t

#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    and temp, x_mval, y_mval                                       \n\t \
    orn x_mval, x_mval, y_mask                                     \n\t \
    eor x_mval, x_mval, temp                                       \n\t \
                                                                        \
	mov temp, 0                                                    \n\t /* <- remove leakage */ \
    and temp, x_mask, y_mval                                       \n\t \
    orn r_mask, x_mask, y_mask                                     \n\t \
    eor r_mask, r_mask, temp                                       \n\t

#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val) \
    lsl r_mval, x_mval, val                        \n\t \
    lsl r_mask, x_mask, val                        \n\t

// --- SUB ---
#define SEC_NOT_(x_mval, x_mask) \
    mvn x_mval, x_mval      \n\t

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

#define SEC_OR_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    orr temp, x_mval, y_mask                                      \n\t \
    and x_mval, x_mval, y_mval                                    \n\t \
    eor x_mval, x_mval, temp                                      \n\t \
                                                                       \
    orr temp, x_mask, y_mval                                      \n\t \
    and r_mask, x_mask, y_mask                                    \n\t \
    eor r_mask, r_mask, temp                                      \n\t




#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_NOT(x_mval, x_mask) \
    STR(SEC_NOT_(x_mval, x_mask))


#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_SHIFT(r_mval, r_mask, x_mval, x_mask, val) \
    STR(SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val))

#define SEC_SHIFT_1S(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2) \
    STR(SEC_SHIFT_1S_(r_mval, r_mask, x_mval, x_mask, val, temp1, temp2))

#define SEC_OR_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_OR_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))



#endif /* ARM_MACROS_H */
