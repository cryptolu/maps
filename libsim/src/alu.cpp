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

#include <cstdio>
#include "alu.h"

Alu::Alu()
{
	this->n = 0; /* negative */
	this->z = 0; /* zero */
	this->c = 0; /* carry */
	this->v = 0; /* overflow */
	this->q = 0; /* saturation */
}

Alu::~Alu()
{
	/* intentionally empty */
}


void Alu::compute_z(uint32_t y)
{
	if (y == 0)
	{
		this->z = 1;
	}
	else
	{
		this->z = 0;
	}
}

void Alu::compute_n(uint32_t y)
{
	this->n = y >> 31;
}

unsigned int Alu::get_n(void)
{
	return this->n;
}

unsigned int Alu::get_z(void)
{
	return this->z;
}

unsigned int Alu::get_c(void)
{
	return this->c;
}

unsigned int Alu::get_v(void)
{
	return this->v;
}

unsigned int Alu::get_q(void)
{
	return this->q;
}

void Alu::set_c(unsigned int value)
{
	this->c = value;
}

uint32_t Alu::bw_xor(uint32_t a, uint32_t b, unsigned int s)
{
	uint32_t y = a ^ b;
	if (s == 1)
	{
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::bw_and(uint32_t a, uint32_t b, unsigned int s)
{
	uint32_t y = a & b;
	if (s == 1)
	{
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::bw_or(uint32_t a, uint32_t b, unsigned int s)
{
	uint32_t y = a | b;
	if (s == 1)
	{
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::bw_orn(uint32_t a, uint32_t b, unsigned int s)
{
	uint32_t y = a | ~b;
	if (s == 1)
	{
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::bw_andn(uint32_t a, uint32_t b, unsigned int s)
{
	uint32_t y = a & ~b;
	if (s == 1)
	{
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}


uint32_t Alu::addc(uint32_t a, uint32_t b, unsigned int s)
{
	uint64_t yu64 = (uint64_t)a + (uint64_t)b + (uint64_t)this->c;
	int64_t ys64 = (int64_t)a + (int64_t)b + (uint64_t)this->c;
	uint32_t y = yu64 & 0xffffffff;
	if (s == 1)
	{
		if ((uint64_t)y == yu64)
		{
			this->c = 0;
		}
		else
		{
			this->c = 1;
		}
		if ((int64_t)y == ys64)
		{
			this->v = 0;
		}
		else
		{
			this->v = 1;
		}
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::add(uint32_t a, uint32_t b, unsigned int s)
{
	uint64_t yu64 = (uint64_t)a + (uint64_t)b;
	int64_t ys64 = (int64_t)a + (int64_t)b;
	uint32_t y = yu64 & 0xffffffff;
	if (s == 1)
	{
		if ((uint64_t)y == yu64)
		{
			this->c = 0;
		}
		else
		{
			this->c = 1;
		}
		if ((int64_t)y == ys64)
		{
			this->v = 0;
		}
		else
		{
			this->v = 1;
		}
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::subc(uint32_t a, uint32_t b, unsigned int s)
{
	uint64_t yu64 = (uint64_t)a - (uint64_t)b - (uint64_t)(1 - this->c);
	int64_t ys64 = (int64_t)a - (int64_t)b - (uint64_t)(1 - this->c);
	uint32_t y = yu64 & 0xffffffff;
	if (s == 1)
	{
		if ((uint64_t)y == yu64)
		{
			this->c = 0;
		}
		else
		{
			this->c = 1;
		}
		if ((int64_t)y == ys64)
		{
			this->v = 0;
		}
		else
		{
			this->v = 1;
		}
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::sub(uint32_t a, uint32_t b, unsigned int s)
{
	uint64_t yu64 = (uint64_t)a - (uint64_t)b;
	int64_t ys64 = (int64_t)a - (int64_t)b;
	uint32_t y = yu64 & 0xffffffff;
	if (s == 1)
	{
		if ((uint64_t)y == yu64)
		{
			this->c = 0;
		}
		else
		{
			this->c = 1;
		}
		if ((int64_t)y == ys64)
		{
			this->v = 0;
		}
		else
		{
			this->v = 1;
		}
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::lsl(uint32_t a, unsigned int n, unsigned int s)
{
	uint32_t y;
	unsigned int carry;

	if (n == 0)
	{
		y = a;
		carry = 0;
	}
	else
	{
		y = a << (n - 1);
		carry = y >> 31;
		y = y << 1;
	}
	if (s == 1)
	{
		this->c = carry;
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::lsr(uint32_t a, unsigned int n, unsigned int s)
{
	uint32_t y;
	unsigned int carry;

	if (n == 0)
	{
		y = a;
		carry = 0;
	}
	else
	{
		y = a >> (n - 1);
		carry = y & 1;
		y = y >> 1;
	}
	if (s == 1)
	{
		this->c = carry;
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::asr(uint32_t a, unsigned int n, unsigned int s)
{
	uint32_t y;
	uint32_t sign;
	unsigned int carry = this->c;

	y = a;
	sign = a & 0x80000000;
	for (unsigned int i = 0; i < n; ++i)
	{
		carry = y & 1;
		y = (y >> 1) | sign;
	}
	if (s == 1)
	{
		this->c = carry;
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}

uint32_t Alu::ror(uint32_t a, unsigned int n, unsigned int s)
{
	uint32_t y;
	unsigned int carry = this->c;

	y = a;
	for (unsigned int i = 0; i < n; ++i)
	{
		carry = y & 1;
		y = (y >> 1) | (carry << 31);
	}
	if (s == 1)
	{
		this->c = carry;
		this->compute_z(y);
		this->compute_n(y);
	}
	return y;
}
