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
#include "tracer.h"

class Memory
{
	private:
		uint8_t *mem8;
		uint16_t *mem16; /* aliases for mem8 seen as an array of 16-bit numbers */
		uint32_t *mem32; /* aliases for mem8 seen as an array of 32-bit numbers */
		uint32_t size;
		Tracer *tracer_ptr;

	public:
		Memory();
		~Memory();
		void set_size(uint32_t size);
		uint32_t get_size(void);
		void bind_tracer(Tracer *ptr);
		void write32(uint32_t addr, uint32_t val);
		void write16(uint32_t addr, uint16_t val);
		void write8(uint32_t addr, uint8_t val);
		void write32_notrace(uint32_t addr, uint32_t val);
		void write16_notrace(uint32_t addr, uint16_t val);
		void write8_notrace(uint32_t addr, uint8_t val);
		uint32_t read32(uint32_t addr);
		uint16_t read16(uint32_t addr);
		uint8_t read8(uint32_t addr);
		uint32_t read32_notrace(uint32_t addr);
		uint16_t read16_notrace(uint32_t addr);
		uint8_t read8_notrace(uint32_t addr);
		int load(const char *filename);
		void dump(uint32_t start, uint32_t len);
};

#endif
