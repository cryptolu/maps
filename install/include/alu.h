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

/******************************************************************************
 *
 * ALU
 *
 ******************************************************************************/

#ifndef __ALU_H__
#define __ALU_H__

#include <cstdint>

class Alu
{
	private:
		unsigned int n;
		unsigned int z;
		unsigned int c;
		unsigned int v;
		unsigned int q;

		void compute_z(uint32_t y);
		void compute_n(uint32_t y);

	public:
		Alu();
		~Alu();
		unsigned int get_n(void);
		unsigned int get_z(void);
		unsigned int get_c(void);
		unsigned int get_v(void);
		unsigned int get_q(void);
		void set_c(unsigned int value);
		uint32_t bw_xor(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_and(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_or(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_orn(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_andn(uint32_t a, uint32_t b, unsigned int s);
		uint32_t add(uint32_t a, uint32_t b, unsigned int s);
		uint32_t addc(uint32_t a, uint32_t b, unsigned int s);
		uint32_t sub(uint32_t a, uint32_t b, unsigned int s);
		uint32_t subc(uint32_t a, uint32_t b, unsigned int s);
		uint32_t lsl(uint32_t a, unsigned int n, unsigned int s);
		uint32_t lsr(uint32_t a, unsigned int n, unsigned int s);
		uint32_t asr(uint32_t a, unsigned int n, unsigned int s);
		uint32_t ror(uint32_t a, unsigned int n, unsigned int s);
};

#endif

