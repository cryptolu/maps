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

#define GET_BYTE(x, n) (((x) >> (8*(n))) & 0xff)

Memory::Memory()
{
	/* intentionally empty */
}

Memory::~Memory()
{
	/* intentionally empty */
}

void Memory::write32(unsigned int addr, uint32_t val)
{
	/* TODO: check addr alignement */
	this->write8(addr + 0, GET_BYTE(val, 0));
	this->write8(addr + 1, GET_BYTE(val, 1));
	this->write8(addr + 2, GET_BYTE(val, 2));
	this->write8(addr + 3, GET_BYTE(val, 3));
}

void Memory::write16(unsigned int addr, uint16_t val)
{
	/* TODO: check addr alignement */
	this->write8(addr + 0, GET_BYTE(val, 0));
	this->write8(addr + 1, GET_BYTE(val, 1));
}

void Memory::write8(unsigned int addr, uint8_t val)
{
	if (addr >= MEM_SIZE)
	{
		fprintf(stderr, "-- ERROR: writing outside of memory!");
		std::exit(EXIT_FAILURE);
	}
	this->mem[addr + 0] = val;
}

uint32_t Memory::read32(unsigned int addr)
{
	/* TODO: check addr alignement */
	uint32_t ret;

	ret = this->read8(addr);
	ret += this->read8(addr + 1) << 8;
	ret += this->read8(addr + 2) << 16;
	ret += this->read8(addr + 3) << 24;
	return ret;
}

uint16_t Memory::read16(unsigned int addr)
{
	/* TODO: check addr alignement */
	uint16_t ret;

	ret = this->read8(addr);
	ret += this->read8(addr + 1) << 8;
	return ret;
}

uint8_t Memory::read8(unsigned int addr)
{
	if (addr >= MEM_SIZE)
	{
		fprintf(stderr, "-- ERROR: reading outside of memory!\n");
		// std::exit(EXIT_FAILURE); <- gcc might read out of memory
		return 0;
	}
	return this->mem[addr];
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
			this->mem[i] = (uint8_t)c;
		}
		return 0; /* success */
	}
	else
	{
		return -1; /* failure */
	}
}

void Memory::dump(unsigned int start, unsigned int len)
{
	unsigned int addr;

	for (unsigned int i = 0; i < len; ++i)
	{
		addr = start + i;
		if ((i % 16) == 0)
		{
			fprintf(stderr, "0x%08x: ", addr);
		}
		fprintf(stderr, "%02x ", this->mem[addr]);
		if ((i % 16) == 15)
		{
			fprintf(stderr, "\n");
		}
	}
}

unsigned int Memory::get_size(void)
{
	return MEM_SIZE;
}
