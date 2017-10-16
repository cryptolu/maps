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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <cstdint>

#define MEM_SIZE 8*1024

class Memory
{
	private:
		uint8_t mem[MEM_SIZE];

	public:
		Memory();
		~Memory();
		void write32(unsigned int addr, uint32_t val);
		void write16(unsigned int addr, uint16_t val);
		void write8(unsigned int addr, uint8_t val);
		uint32_t read32(unsigned int addr);
		uint16_t read16(unsigned int addr);
		uint8_t read8(unsigned int addr);
		int load(const char *filename);
		void dump(unsigned int start, unsigned int len);
		unsigned int get_size(void);
};

#endif
