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
 * Cpu
 *
 ******************************************************************************/

#ifndef __CPU_H__
#define __CPU_H__

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>

#include "register.h"
#include "memory.h"
#include "alu.h"
#include "tracer.h"

#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define SP 13
#define LR 14
#define PC 15


class Cpu
{
	private:
		Alu alu;
		Memory ram;
		Tracer tracer;
		unsigned long int instruction_count;
		bool with_gdb;

		void report_error(const char *msg, const char *location) __attribute__ ((noreturn));

		std::string trace_index_filename;
		bool generate_trace_index;
		bool trace_index_done;
		FILE *trace_index_file;

		uint32_t generate_modified_immediate(uint16_t ins16, uint16_t ins16_b);
		uint32_t decode_imm_shift(unsigned int op, unsigned int imm, unsigned int s, uint32_t a);
		unsigned int conditional_branch(unsigned int cond, int32_t offset, bool is_ins32, unsigned int p_addr);

	public:
		Register regs[16];

		Cpu(bool with_gdb, std::string trace_index_filename = "");
		~Cpu();
		void reset(void);
		int load(const char *filename);
		void write_register(unsigned int reg_idx, uint32_t value);
		uint32_t read_register(unsigned int reg_idx);
		uint32_t read_apsr(void);
		void write8_ram(unsigned int addr, uint8_t value);
		void write16_ram(unsigned int addr, uint16_t value);
		void write32_ram(unsigned int addr, uint32_t value);
		uint8_t read8_ram(unsigned int addr);
		uint8_t read16_ram(unsigned int addr);
		uint8_t read32_ram(unsigned int addr);
		int step(void);
		unsigned long int run(unsigned int from, unsigned int until, unsigned long int limit = 0);

		void dump_memory(unsigned int start, unsigned int len);
		void dump_regs(void);

		void copy_array_to_target(uint32_t *buffer, unsigned int len, unsigned int target_addr);
		void copy_array_from_target(uint32_t *buffer, unsigned int len, unsigned int target_addr);

		void reset_pwr_trace(void);
		std::vector<unsigned int> get_pwr_trace(void);
};

#endif
