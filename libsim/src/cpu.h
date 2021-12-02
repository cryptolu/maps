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
 * CPU (alt)
 *
 ******************************************************************************/

#ifndef __CPU_H__
#define __CPU_H__

#include <cstdint>
#include <string>

#include "register.h"
#include "memory.h"
#include "flag.h"
#include "options.h"

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


typedef enum
{
	STEP_DONE,
	STEP_BKPT
} Step_status;


class Cpu
{
	private:
		Register regs[15];
		uint32_t pc;
		Register reg_a;
		Register reg_b;
		unsigned int flags[5];
		unsigned int itstate;
		Memory ram;
		Tracer tracer;
		Tracer_none tracer_none;
		unsigned long int instruction_count;

		bool with_gdb;
        std::string trace_index_filename;
        bool generate_trace_index;
        bool trace_index_done;
        FILE *trace_index_file;

		void report_error(const char *msg, const char *location);

		bool in_it_block(void);
		void update_flags(uint32_t res, unsigned int c, unsigned int v);
		void execute_conditional_branch(unsigned int cond, int32_t offset, bool is_ins32);
		void execute_alu_op(unsigned int alu_op, unsigned int rd, unsigned int rn, unsigned int s, uint32_t b);
		void execute_op16_pushm(uint16_t ins16);
		void execute_op16_popm(uint16_t ins16);
		void execute_op16_sub_imm_sp(uint16_t ins16);
		void execute_op16_add_imm_sp(uint16_t ins16);
		void execute_op16_st_reg_sp_rel(uint16_t ins16);
		void execute_op16_ld_reg_sp_rel(uint16_t ins16);
		void execute_op16_shift_imm_add_sub_mov_cmp(uint16_t ins16);
		void execute_op16_special_data_branch(uint16_t ins16);
		void execute_op16_ldmia(uint16_t ins16);
		void execute_op16_nop(void);
		void execute_op16_breakpoint(uint16_t ins16);
		void execute_op16_cond_branch(uint16_t ins16);
		void execute_op16_uncond_branch(uint16_t ins16);
		void execute_op16_ld_literal_pool(uint16_t ins16);
		void execute_op32_ldmia(uint16_t ins16, uint16_t ins16_b);
		void execute_op16_str_imm(uint16_t ins16);
		void execute_op16_ld_imm(uint16_t ins16);
		void execute_op16_ldrb_imm(uint16_t ins16);
		void execute_op16_ldrb_reg(uint16_t ins16);
		void execute_op16_ldr_reg(uint16_t ins16);
		void execute_op16_strb_imm(uint16_t ins16);
		void execute_op16_strb_reg(uint16_t ins16);
		void execute_op16_str_reg(uint16_t ins16);
		void execute_op16_rev(uint16_t ins16);
		void execute_op16_rev16(uint16_t ins16);
		void execute_op16_revsh(uint16_t ins16);
		void execute_op16_uxtb(uint16_t ins16);
		void execute_op16_uxth(uint16_t ins16);
		void execute_op16_sxtb(uint16_t ins16);
		void execute_op16_sxth(uint16_t ins16);
		void execute_op32_data_mod_imm(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_stmia(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_stmdb(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldmdb(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_data_shifted_reg(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_data_plain_imm(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_data_reg(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_branch_misc(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_str_imm(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_str_imm12(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldr_imm(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldr_imm12(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldrb_imm(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldrb_imm_alt(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldrb_reg(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_strb_imm(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_strb_imm_alt(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_strb_reg(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ld_literal_pool(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldrb_literal(uint16_t ins16, uint16_t ins16_b);
		void execute_op32_ldrd_imm(uint16_t ins16, uint16_t ins16_b);

	public:
		Cpu(Options &options);
		~Cpu();

		void reset(void);
		int load(const char *filename);
		void write_register(unsigned int reg_idx, uint32_t value);
		uint32_t read_register(unsigned int reg_idx);
		uint32_t read_apsr(void);
		void write8_ram(uint32_t addr, uint8_t value);
		void write16_ram(uint32_t addr, uint16_t value);
		void write32_ram(uint32_t addr, uint32_t value);
		uint8_t read8_ram(uint32_t addr);
		uint8_t read16_ram(uint32_t addr);
		uint8_t read32_ram(uint32_t addr);
		Step_status step(void);
		unsigned long int run(uint32_t from, uint32_t until, unsigned long int limit = -1);

		void dump_memory(uint32_t start, uint32_t len);
		void dump_regs(void);
		void copy_array_to_target(uint32_t *buffer, unsigned int len, uint32_t target_addr);
		void copy_array_from_target(uint32_t *buffer, unsigned int len, uint32_t target_addr);
		void reset_pwr_trace(void);
		std::vector<unsigned int> get_pwr_trace(void);

};

#endif
