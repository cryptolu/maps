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

/******************************************************************************
 *
 * Primitives
 *
 ******************************************************************************/

#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include <cstdint>

typedef enum
{
	SRType_LSL,
	SRType_LSR,
	SRType_ASR,
	SRType_ROR,
	SRType_RRX
} SRType;


void lsl_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n);
void lsl(uint32_t *y, uint32_t a, unsigned int n);
void lsr_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n);
void lsr(uint32_t *y, uint32_t a, unsigned int n);
void asr_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n);
void asr(uint32_t *y, uint32_t a, unsigned int n);
void ror_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n);
void ror(uint32_t *y, uint32_t a, unsigned int n);
void rrx_c(uint32_t *y, unsigned int *c_out, uint32_t a, unsigned int c_in);
void rrx(uint32_t *y, uint32_t a, unsigned int c_in);
void add_c(uint32_t *y, unsigned int *c_out, unsigned int *v_out, uint32_t a, uint32_t b, unsigned int c_in);
void add(uint32_t *y, unsigned int *c_out, unsigned int *v_out, uint32_t a, uint32_t b);
void bw_and(uint32_t *y, uint32_t a, uint32_t b);
void bw_orr(uint32_t *y, uint32_t a, uint32_t b);
void bw_eor(uint32_t *y, uint32_t a, uint32_t b);

void decode_imm_shift(SRType *srtype, unsigned int *n, unsigned int type, unsigned int imm5);
void decode_reg_shift(SRType *srtype, unsigned int type);
void shift_c(uint32_t *y, unsigned int *c_out,  uint32_t a, SRType srtype, unsigned int n, unsigned int c_in);
void shift(uint32_t *y, uint32_t a, SRType srtype, unsigned int n, unsigned int c_in);
void thumb_expand_imm_c(uint32_t *y, uint32_t *c_out, unsigned int imm12, unsigned int c_in);


#endif
