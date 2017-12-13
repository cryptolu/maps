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
 * Memory
 *
 ******************************************************************************/

#include <fstream>
#include <cstdio>

#include "memory.h"
#include "tracer.h"
#include "utils.h"

#define GET_BYTE(x, n) (((x) >> (8*(n))) & 0xff)

Memory::Memory()
{
	/* intentionally empty */
	this->mem8 = nullptr;
	this->mem16 = nullptr;
	this->mem32 = nullptr;
	this->size = 0;
}

Memory::~Memory()
{
	/* intentionally empty */
	if (this->mem8 != nullptr)
	{
		delete[] this->mem8;
	}
	this->mem8 = nullptr;
	this->mem16 = nullptr;
	this->mem32 = nullptr;
}


void Memory::set_size(uint32_t size)
{
	this->mem8 = new uint8_t[size];
	this->mem16 = (uint16_t *)(this->mem8);
	this->mem32 = (uint32_t *)(this->mem8);
	this->size = size;
}


uint32_t Memory::get_size(void)
{
	return this->size;
}


void Memory::bind_tracer(Tracer *ptr)
{
	this->tracer_ptr = ptr;
}


void Memory::write32(uint32_t addr, uint32_t val)
{
	if (addr >= (this->size - 4))
	{
		fprintf(stderr, "-- ERROR: writing 32-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem32[addr >> 2] = val;
	unsigned int pwr = bit_count(val);
	this->tracer_ptr->update(pwr);
}


void Memory::write32_notrace(uint32_t addr, uint32_t val)
{
	if (addr >= (this->size - 4))
	{
		fprintf(stderr, "-- ERROR: writing 32-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem32[addr >> 2] = val;
}


void Memory::write16(uint32_t addr, uint16_t val)
{
	if (addr >= (this->size - 2))
	{
		fprintf(stderr, "-- ERROR: writing 16-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem16[addr >> 1] = val;
	unsigned int pwr = bit_count(val);
	this->tracer_ptr->update(pwr);
}


void Memory::write16_notrace(uint32_t addr, uint16_t val)
{
	if (addr >= (this->size - 2))
	{
		fprintf(stderr, "-- ERROR: writing 16-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem16[addr >> 1] = val;
}


void Memory::write8(uint32_t addr, uint8_t val)
{
	if (addr >= this->size)
	{
		fprintf(stderr, "-- ERROR: writing 8-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem8[addr] = val;
	unsigned int pwr = bit_count(val);
	this->tracer_ptr->update(pwr);
}


void Memory::write8_notrace(uint32_t addr, uint8_t val)
{
	if (addr >= this->size)
	{
		fprintf(stderr, "-- ERROR: writing 8-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem8[addr] = val;
}


uint32_t Memory::read32(uint32_t addr)
{
	if (addr >= (this->size - 4))
	{
		fprintf(stderr, "-- ERROR: reading 32-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	uint32_t ret = this->mem32[addr >> 2];
	unsigned int pwr = bit_count(ret);
	this->tracer_ptr->update(pwr);
	return ret;
}


uint32_t Memory::read32_notrace(uint32_t addr)
{
	if (addr >= (this->size - 4))
	{
		fprintf(stderr, "-- ERROR: reading 32-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	uint32_t ret = this->mem32[addr >> 2];
	return ret;
}


uint16_t Memory::read16(uint32_t addr)
{
	if (addr >= (this->size - 2))
	{
		fprintf(stderr, "-- ERROR: reading 16-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	uint16_t ret = this->mem16[addr >> 1];
	unsigned int pwr = bit_count(ret);
	this->tracer_ptr->update(pwr);
	return ret;
}


uint16_t Memory::read16_notrace(uint32_t addr)
{
	if (addr >= (this->size - 2))
	{
		fprintf(stderr, "-- ERROR: reading 16-bit value outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	uint16_t ret = this->mem16[addr >> 1];
	return ret;
}


uint8_t Memory::read8(uint32_t addr)
{
	if (addr >= this->size)
	{
		fprintf(stderr, "-- ERROR: reading 8-bit value outside of memory!\n");
		std::exit(EXIT_FAILURE);
	}
	uint8_t ret = this->mem8[addr];
	unsigned int pwr = bit_count(ret);
	this->tracer_ptr->update(pwr);
	return ret;
}


uint8_t Memory::read8_notrace(uint32_t addr)
{
	if (addr >= this->size)
	{
		fprintf(stderr, "-- ERROR: reading 8-bit value outside of memory!\n");
		std::exit(EXIT_FAILURE);
	}
	uint8_t ret = this->mem8[addr];
	return ret;
}


int Memory::load(const char *filename)
{
	unsigned int file_length;

	std::ifstream input(filename, std::ios::in | std::ios::binary);
	if (input)
	{
		/* find file length */
		input.seekg(0, std::ios::end);
		file_length = input.tellg();
		input.seekg(0, std::ios::beg);
		for (unsigned int i = 0; i < file_length; ++i)
		{
			char c;
			input.get(c);
			this->mem8[i] = (uint8_t)c;
		}
		return 0; /* success */
	}
	else
	{
		return -1; /* failure */
	}
}


void Memory::dump(uint32_t start, uint32_t len)
{
	unsigned int addr;

	for (unsigned int i = 0; i < len; ++i)
	{
		addr = start + i;
		if ((i % 16) == 0)
		{
			fprintf(stderr, "0x%08x: ", addr);
		}
		fprintf(stderr, "%02x ", this->mem8[addr]);
		if ((i % 16) == 15)
		{
			fprintf(stderr, "\n");
		}
	}
}
