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

#define SEC_SHIFT_1_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    lsl r_mval, x_mval, CV1                               \n\t \
    lsl r_mask, x_mask, CV1                               \n\t

#define SEC_SHIFT_2_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    lsl r_mval, x_mval, CV2                               \n\t \
    lsl r_mask, x_mask, CV2                               \n\t

#define SEC_SHIFT_3_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    lsl r_mval, x_mval, CV4                               \n\t \
    lsl r_mask, x_mask, CV4                               \n\t

#define SEC_SHIFT_4_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    lsl r_mval, x_mval, CV8                               \n\t \
    lsl r_mask, x_mask, CV8                               \n\t

#define SEC_SHIFT_5_ITERATION_(r_mval, r_mask, x_mval, x_mask) \
	mov r_mval, 0                                         \n\t /* prevent leakage for next instruction */ \
    lsl r_mval, x_mval, CV16                              \n\t \
    lsl r_mask, x_mask, CV16                              \n\t



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


#endif /* ARM_MACROS_H */
