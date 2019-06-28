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

//#define CPU_DEBUG_TRACE

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>

#include "cpu.h"
#include "register.h"
#include "flag.h"
#include "options.h"
#include "tracer.h"
#include "primitives.h"
#include "opcodes.h"
#include "utils.h"
#include "debug.h"

#include "rsp_layer.h"


void Cpu::report_error(const char *msg, const char *location)
{
	fprintf(stderr, "-- ERROR: %s in %s at address 0x%08x\n", msg, location, this->pc);
	std::exit(EXIT_FAILURE);
}


Cpu::Cpu(Options &options)
{
	/* set up options */
	this->with_gdb = options.with_gdb;
	/* set up memory */
	this->ram.set_size(options.mem_size);
	this->ram.bind_tracer(&(this->tracer));
	/* set up registers */
	for (unsigned int i = 0; i < 15; i++)
	{
		this->regs[i].bind_tracer(&(this->tracer));
		this->regs[i].set_name("r" + std::to_string(i));
	}
	if (options.with_pipeline_leakage)
	{
		this->reg_a.bind_tracer(&(this->tracer));
		this->reg_b.bind_tracer(&(this->tracer));
	}
	else
	{
		this->reg_a.bind_tracer(&(this->tracer_none));
		this->reg_b.bind_tracer(&(this->tracer_none));
	}
	this->reg_a.set_name("rA");
	this->reg_b.set_name("rB");
	/* set up instruction count and trace capabilities */
	this->instruction_count = 0;
	this->trace_index_done = false;
	this->trace_index_filename = options.trace_index_filename;
	if (this->trace_index_filename.size() > 0)
	{
		this->generate_trace_index = true;
	}
	else
	{
		this->generate_trace_index = false;
	}
}


Cpu::~Cpu()
{
	/* intentionally empty */
}


/******************************************************************************
 * Simulator API
 ******************************************************************************/
void Cpu::reset(void)
{
	this->itstate = 0;
	this->instruction_count = 0;
	this->trace_index_done = false;
	/* stack pointer set to end of RAM */
	this->regs[SP].write(this->ram.get_size() - 4);
	/* program counter set to 0 */
	this->pc = 0;
}


int Cpu::load(const char *filename)
{
	return this->ram.load(filename);
}


void Cpu::write_register(unsigned int reg_idx, uint32_t value)
{
	if (reg_idx < 16)
	{
		this->regs[reg_idx].write(value);
	}
	else
	{
		this->report_error("reg_idx must be < 16", "Cpu::write_register()");
	}
}


uint32_t Cpu::read_register(unsigned int reg_idx)
{
	uint32_t res;
	if (reg_idx < 15)
	{
		res = this->regs[reg_idx].read();
	}
	else if (reg_idx == 15)
	{
		res = this->pc;
	}
	else
	{
		this->report_error("reg_idx must be < 16", "Cpu::read_register()");
	}
	return res;
}


uint32_t Cpu::read_apsr(void)
{
	uint32_t apsr = (this->flags[N] << 31) |
	                (this->flags[Z] << 30) |
	                (this->flags[C] << 29) |
	                (this->flags[V] << 28) |
	                (this->flags[Q] << 27);
	return apsr;
}


void Cpu::write8_ram(uint32_t addr, uint8_t value)
{
	this->ram.write8_notrace(addr, value);
}


void Cpu::write16_ram(uint32_t addr, uint16_t value)
{
	this->ram.write16_notrace(addr, value);
}


void Cpu::write32_ram(uint32_t addr, uint32_t value)
{
	this->ram.write32_notrace(addr, value);
}


uint8_t Cpu::read8_ram(uint32_t addr)
{
	return this->ram.read8_notrace(addr);
}


uint8_t Cpu::read16_ram(uint32_t addr)
{
	return this->ram.read16_notrace(addr);
}


uint8_t Cpu::read32_ram(uint32_t addr)
{
	return this->ram.read32_notrace(addr);
}


void Cpu::dump_memory(uint32_t start, uint32_t len)
{
	this->ram.dump(start, len);
}


void Cpu::dump_regs(void)
{
	fprintf(stderr, "R0 : 0x%08x, R1 : 0x%08x, ", this->regs[0].read(), this->regs[1].read());
	fprintf(stderr, "R2 : 0x%08x, R3 : 0x%08x\n", this->regs[2].read(), this->regs[3].read());
	fprintf(stderr, "R4 : 0x%08x, R5 : 0x%08x, ", this->regs[4].read(), this->regs[5].read());
	fprintf(stderr, "R6 : 0x%08x, R7 : 0x%08x\n", this->regs[6].read(), this->regs[7].read());
	fprintf(stderr, "R8 : 0x%08x, R9 : 0x%08x, ", this->regs[8].read(), this->regs[9].read());
	fprintf(stderr, "R10: 0x%08x, R11: 0x%08x\n", this->regs[10].read(), this->regs[11].read());
	fprintf(stderr, "R12: 0x%08x, SP : 0x%08x, ", this->regs[12].read(), this->regs[13].read());
	fprintf(stderr, "LR : 0x%08x, PC : 0x%08x\n", this->regs[14].read(), this->pc);
	fprintf(stderr, "n: %u, c = %u, z = %u, ", this->flags[N], this->flags[C], this->flags[Z]);
	fprintf(stderr, "v = %u, q = %u\n", this->flags[V], this->flags[Q]);
}


void Cpu::copy_array_to_target(uint32_t *buffer, unsigned int len, uint32_t target_addr)
{
	for (unsigned int i = 0; i < len; ++i)
	{
		this->ram.write32_notrace(target_addr + 4*i, buffer[i]);
	}
}


void Cpu::copy_array_from_target(uint32_t *buffer, unsigned int len, uint32_t target_addr)
{
	for (unsigned int i = 0; i < len; ++i)
	{
		buffer[i] = this->ram.read32_notrace(target_addr + 4*i);
	}
}


void Cpu::reset_pwr_trace(void)
{
	this->tracer.reset();
}


std::vector<unsigned int> Cpu::get_pwr_trace(void)
{
	return this->tracer.get_trace();
}


Step_status Cpu::step(void)
{
	Step_status status = STEP_DONE;

	/* Instruction is fetch from the memory WITHOUT tracing those memory accesses since
	   fetching the instructions should not leak information (unless your code is really
	   really bad ...). This allows tracing the memory accesses when it is meaningfull
	   without impacting the simulation speed too much.
	 */
	uint16_t ins16 = this->ram.read16_notrace(this->pc);
	if (((ins16 & OP16_MASK) == OP16_VAL1) || ((ins16 & OP16_MASK) == OP16_VAL2) || ((ins16 & OP16_MASK) == OP16_VAL3))
	{ /* 32-bit instructions */
		uint32_t ins16_b = this->ram.read16_notrace(this->pc + 2);
		uint32_t ins32 = (ins16 << 16) | ins16_b;
		CPU_LOG_TRACE(">>>>>>>> p_addr = 0x%08x, ins32 = 0x%08x: ", this->pc, ins32);
		if (TEST_INS32(OP32_LDMIA))
		{
			this->execute_op32_ldmia(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STMIA))
		{
			this->execute_op32_stmia(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STMDB))
		{
			this->execute_op32_stmdb(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDMDB))
		{
			this->execute_op32_ldmdb(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_DATA_SHIFTED_REG))
		{
			this->execute_op32_data_shifted_reg(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_DATA_MOD_IMM))
		{
			this->execute_op32_data_mod_imm(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_DATA_PLAIN_IMM))
		{
			this->execute_op32_data_plain_imm(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_DATA_REG))
		{
			this->execute_op32_data_reg(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_BRANCH_MISC))
		{
			this->execute_op32_branch_misc(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STR_IMM))
		{
			this->execute_op32_str_imm(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STR_IMM12))
		{
			this->execute_op32_str_imm12(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDR_IMM))
		{
			this->execute_op32_ldr_imm(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDR_IMM12))
		{
			this->execute_op32_ldr_imm12(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDRB_IMM))
		{
			this->execute_op32_ldrb_imm(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDRB_IMM_ALT))
		{
			this->execute_op32_ldrb_imm_alt(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDRB_REG))
		{
			this->execute_op32_ldrb_reg(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STRB_IMM))
		{
			this->execute_op32_strb_imm(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STRB_IMM_ALT))
		{
			this->execute_op32_strb_imm_alt(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_STRB_REG))
		{
			this->execute_op32_strb_reg(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LD_LITERAL_POOL))
		{
			this->execute_op32_ld_literal_pool(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDRB_LITERAL))
		{
			this->execute_op32_ldrb_literal(ins16, ins16_b);
		}
		else if (TEST_INS32(OP32_LDRD_IMM))
		{
			this->execute_op32_ldrd_imm(ins16, ins16_b);
		}
		else
		{
			this->report_error("unsupported 32-bit instruction", "Cpu::step()");
		}
	}
	else
	{ /* 16-bit instructions */
		CPU_LOG_TRACE(">>>>>>>> p_addr = 0x%08x, ins16 = 0x%04x: ", this->pc, ins16);
		if (TEST_INS16(OP16_PUSHM))
		{
			this->execute_op16_pushm(ins16);
		}
		else if (TEST_INS16(OP16_POPM))
		{
			this->execute_op16_popm(ins16);
		}
		else if (TEST_INS16(OP16_SUB_IMM_SP))
		{
			this->execute_op16_sub_imm_sp(ins16);
		}
		else if (TEST_INS16(OP16_ADD_IMM_SP))
		{
			this->execute_op16_add_imm_sp(ins16);
		}
		else if (TEST_INS16(OP16_ST_REG_SP_REL))
		{
			this->execute_op16_st_reg_sp_rel(ins16);
		}
		else if (TEST_INS16(OP16_LD_REG_SP_REL))
		{
			this->execute_op16_ld_reg_sp_rel(ins16);
		}
		else if (TEST_INS16(OP16_LD_IMM))
		{
			this->execute_op16_ld_imm(ins16);
		}
		else if (TEST_INS16(OP16_SHIFT_IMM_ADD_SUB_MOV_CMP))
		{
			this->execute_op16_shift_imm_add_sub_mov_cmp(ins16);
		}
		else if (TEST_INS16(OP16_SPECIAL_DATA_BRANCH))
		{
			this->execute_op16_special_data_branch(ins16);
		}
		else if (TEST_INS16(OP16_LDMIA))
		{
			this->execute_op16_ldmia(ins16);
		}
		else if (TEST_INS16(OP16_STR_IMM))
		{
			this->execute_op16_str_imm(ins16);
		}
		else if (TEST_INS16(OP16_COND_BRANCH))
		{
			this->execute_op16_cond_branch(ins16);
		}
		else if (TEST_INS16(OP16_LD_LITERAL_POOL))
		{
			this->execute_op16_ld_literal_pool(ins16);
		}
		else if (TEST_INS16(OP16_LDRB_IMM))
		{
			this->execute_op16_ldrb_imm(ins16);
		}
		else if (TEST_INS16(OP16_LDRB_REG))
		{
			this->execute_op16_ldrb_reg(ins16);
		}
		else if (TEST_INS16(OP16_STRB_IMM))
		{
			this->execute_op16_strb_imm(ins16);
		}
		else if (TEST_INS16(OP16_STRB_REG))
		{
			this->execute_op16_strb_reg(ins16);
		}
		else if (TEST_INS16(OP16_REV))
		{
			this->execute_op16_rev(ins16);
		}
		else if (TEST_INS16(OP16_REV16))
		{
			this->execute_op16_rev16(ins16);
		}
		else if (TEST_INS16(OP16_REVSH))
		{
			this->execute_op16_revsh(ins16);
		}
		else if (TEST_INS16(OP16_UXTB))
		{
			this->execute_op16_uxtb(ins16);
		}
		else if (TEST_INS16(OP16_UXTH))
		{
			this->execute_op16_uxth(ins16);
		}
		else if (TEST_INS16(OP16_SXTB))
		{
			this->execute_op16_sxtb(ins16);
		}
		else if (TEST_INS16(OP16_SXTH))
		{
			this->execute_op16_sxth(ins16);
		}
		else if (TEST_INS16(OP16_NOP))
		{
			this->execute_op16_nop();
		}
		else if (TEST_INS16(OP16_BREAKPOINT))
		{
			this->execute_op16_breakpoint(ins16);
			status = STEP_BKPT;
		}
		else
		{
			this->report_error("unsupported 16-bit instruction", "Cpu::step()");
		}
	}
	this->instruction_count++;
	return status;
}


unsigned long int Cpu::run(uint32_t from, uint32_t until, unsigned long int limit)
{
	/* prepare file for trace index */
	FILE *trace_index_file;
	if (this->generate_trace_index == true && this->trace_index_done == false)
	{
		trace_index_file = fopen(this->trace_index_filename.c_str(), "w");
	}

	/* prepare to jump to code */
	this->regs[LR].write(until);
	this->pc = from;

	if (this->with_gdb == true)
	{
		Rsp_layer server;
		server.run(this);
	}
	else
	{
		uint32_t p_addr;
		while (1)
		{
			p_addr = this->pc;
			if (this->pc == until)
			{
				break;
			}
			if (this->step() == STEP_BKPT)
			{
				/* instruction was a breakpoint */
				fprintf(stderr, "---- Hit breakpoint at address 0x%08x\n", p_addr);
				this->dump_regs();
				/* a breakpoint instruction does not increment the PC, but here we are
				   using the "BKPT" instruction in a different way: to generate a
				   partial trace for debugging */
				this->pc = (p_addr + 2);
			}
			#ifdef CPU_DEBUG_TRACE
			this->dump_regs();
			this->dump_memory(0x0400, 16);
			this->dump_memory(0x0500, 16);
			this->dump_memory(8*1024 - 64, 64);
			#endif
			if (limit != 0 and this->instruction_count == limit)
			{
				break;
			}
			if (this->generate_trace_index == true && this->trace_index_done == false)
			{
				fprintf(trace_index_file, "(%lu, 0x%08x)\n", this->tracer.get_register_write_count(), p_addr);
			}
		}
	}

	/* close trace index file */
	if (this->generate_trace_index == true && this->trace_index_done == false)
	{
		fclose(trace_index_file);
		this->trace_index_done = true;
	}

	return this->instruction_count;
}


/******************************************************************************
 * Utils
 ******************************************************************************/
bool Cpu::in_it_block(void)
{
	return (this->itstate == 0);
}


void Cpu::update_flags(uint32_t res, unsigned int c, unsigned int v)
{
	this->flags[N] = compute_n(res);
	this->flags[Z] = compute_z(res);
	this->flags[C] = c;
	this->flags[V] = v;
}


void Cpu::execute_conditional_branch(unsigned int cond, int32_t offset, bool is_ins32)
{
	unsigned int tst;
	switch (cond)
	{
		case 0: /* EQ */
			tst = this->flags[Z];
			break;
		case 1: /* NE */
			tst = 1 - this->flags[Z];
			break;
		case 2: /* CS */
			tst = this->flags[C];
			break;
		case 3: /* CC */
			tst = 1 - this->flags[C];
			break;
		case 4: /* MI */
			tst = this->flags[N];
			break;
		case 5: /* PL */
			tst = 1 - this->flags[N];
			break;
		case 6: /* VS */
			tst = this->flags[V];
			break;
		case 7: /* VC */
			tst = 1 - this->flags[V];
			break;
		case 8: /* HI */
			tst = this->flags[C] & (1 - this->flags[Z]);
			break;
		case 9: /* LS */
			tst = this->flags[Z] & (1 - this->flags[C]);
			break;
		case 10: /* GE */
			tst = (this->flags[N] == this->flags[V]) ? 1 : 0;
			break;
		case 11: /* LT */
			tst = (this->flags[N] == this->flags[V]) ? 1 : 0;
			break;
		case 12: /* GT */
			tst = ((this->flags[Z] == 0) && (this->flags[V] == this->flags[N])) ? 1 : 0;
			break;
		case 13: /* LE */
			tst = ((this->flags[Z] == 1) && (this->flags[V] != this->flags[N])) ? 1 : 0;
			break;
		case 14: /* always */
			tst = 1;
			break;
		default:
			this->report_error(" unsupported cond", "OP16_COND_BRANCH");
			break;
	}
	if (tst == 1)
	{
		this->reg_a.write(this->pc + 4);
		this->pc = this->pc + offset + 4;
	}
	else
	{
		if (is_ins32)
		{
			this->pc = this->pc + 4;
		}
		else
		{
			this->pc = this->pc + 2;
		}
	}
}


void Cpu::execute_alu_op(unsigned int alu_op, unsigned int rd, unsigned int rn, unsigned int s, uint32_t b)
{
	uint32_t y;
	unsigned int c_out = this->flags[C]; /* initialized to current value so that we can */
	unsigned int v_out = this->flags[V]; /* always update the flags */
	uint32_t a = this->regs[rn].read();
	switch (alu_op)
	{
		case 0: /* AND A6.7.9/T2, TST */
			bw_and(&y, a, b);
			if (rd != 15)
			{
				this->regs[rd].write(y);
			}
			break;
		case 1: /* BIC A6.7.16/T2 */
			bw_and(&y, a, ~b);
			this->regs[rd].write(y);
			break;
		case 2: /* ORR A6.7.91/T2 */
			if (rn == 15)
			{ /* move reg and immediate shift */
				y = b;
			}
			else
			{ /* ORR A6.7.91/T2 */
				bw_orr(&y, a, b);
			}
			this->regs[rd].write(y);
			break;
		case 3:
			if (rn == 15)
			{ /* MVN A6.7.85/T2 */
				bw_orr(&y, 0, ~b);
			}
			else
			{ /* ORN A&.7.89/T1 */
				bw_orr(&y, a, ~b);
			}
			this->regs[rd].write(y);
			break;
		case 4: /*  EOR A6.7.35/T2, TEQ A6.7.141/T1*/
			bw_eor(&y, a, b);
			if (rd != 15)
			{
				this->regs[rd].write(y);
			}
			break;
		case 8: /* CMN A6.7.26/T2, ADD A6.7.4/T3 */
			add_c(&y, &c_out, &v_out, a, b, 0);
			if (rd != 15)
			{
				this->regs[rd].write(y);
			}
			break;
		case 10: /* ADC A6.7.2/T2 */
			add_c(&y, &c_out, &v_out, a, b, this->flags[C]);
			this->regs[rd].write(y);
			break;
		case 11: /* SBC A6.7.109/T2 */
			add_c(&y, &c_out, &v_out, a, ~b, this->flags[C]);
			this->regs[rd].write(y);
			break;
		case 13: /* CMP A6.7.28/T3, SUB A6.7.133/T2 */
			add_c(&y, &c_out, &v_out, a, ~b, 1);
			if (rd != 15)
			{
				this->regs[rd].write(y);
			}
			break;
		case 14: /* RSB A6.7.107/T1 */
			add_c(&y, &c_out, &v_out, b, a, 1);
			this->regs[rd].write(y);
			break;
		default:
			this->report_error("incorrect operand", "OP32_DATA_SHIFTED_REG");
			break;
	}
	if (s == 1)
	{
		this->update_flags(y, c_out, v_out);
	}
}


/******************************************************************************
 * 16-bit instructions
 ******************************************************************************/
void Cpu::execute_op16_pushm(uint16_t ins16)
{
	/* push multiple registers A6.7.98 */
	CPU_LOG_TRACE("OP16_PUSHM\n");
	this->pc += 2;
	unsigned int m = GET_BIT(ins16, 8);
	uint8_t register_list = GET_FIELD(ins16, 0, 8);
	uint32_t current_sp = this->regs[SP].read();
	this->reg_a.write(current_sp);
	uint32_t d_addr_base = current_sp - 4*bit_count(register_list) - 4*m;
	uint32_t d_addr = d_addr_base;
	this->regs[SP].write(d_addr_base);
	for (unsigned int i = 0; i < 8; i++)
	{
		if (GET_BIT(register_list, i) == 1)
		{
			uint32_t current_reg_value = this->regs[i].read();
			this->reg_a.write(current_reg_value);
			this->ram.write32(d_addr, current_reg_value);
			d_addr += 4;
		}
	}
	if (m == 1)
	{
		uint32_t current_lr = this->regs[LR].read();
		this->reg_a.write(current_lr);
		this->ram.write32(d_addr, current_lr);
		d_addr += 4;
	}
}


void Cpu::execute_op16_popm(uint16_t ins16)
{
	/* pop multiple registers A6.7.97/T1 */
	CPU_LOG_TRACE("OP16_POPM\n");
	unsigned int p = GET_BIT(ins16, 8);
	unsigned int register_list = GET_FIELD(ins16, 0, 8);
	uint32_t d_addr = this->regs[SP].read();
	this->reg_a.write(d_addr);
	uint32_t f_addr = d_addr + 4*bit_count(register_list) + 4*p;
	this->regs[SP].write(f_addr);
	for (unsigned int i = 0; i < 8; ++i)
	{
		if (GET_BIT(register_list, i) == 1)
		{
			this->regs[i].write(this->ram.read32(d_addr));
			d_addr += 4;
		}
	}
	if (p == 1)
	{
		this->pc = this->ram.read32(d_addr);
		d_addr += 4;
	}
	else
	{
		this->pc += 2;
	}
}


void Cpu::execute_op16_sub_imm_sp(uint16_t ins16)
{
	/* SUB SP IMM A6.7.134 */
	CPU_LOG_TRACE("OP16_SUB_IMM_SP\n");
	this->pc += 2;
	uint32_t imm7 = GET_FIELD(ins16, 0, 7) << 2;
	uint32_t current_sp = this->regs[SP].read();
	this->reg_a.write(current_sp);
	this->regs[SP].write(current_sp - imm7);
}


void Cpu::execute_op16_add_imm_sp(uint16_t ins16)
{
	/* ADD SP IMM A6.7.5/T2 */
	CPU_LOG_TRACE("OP16_ADD_IMM_SP\n");
	this->pc += 2;
	uint32_t imm7 = GET_FIELD(ins16, 0, 7) << 2;
	uint32_t current_sp = this->regs[SP].read();
	this->reg_a.write(current_sp);
	this->regs[SP].write(current_sp + imm7);
}


void Cpu::execute_op16_st_reg_sp_rel(uint16_t ins16)
{
	/* store reg SP relative A6.7.119/T2 */
	CPU_LOG_TRACE("OP16_ST_REG_SP_REL\n");
	this->pc += 2;
	uint32_t imm8 = GET_FIELD(ins16, 0, 8) << 2;
	unsigned int rt = GET_FIELD(ins16, 8, 3);
	uint32_t current_sp = this->regs[SP].read();
	this->reg_a.write(current_sp);
	uint32_t d_addr = current_sp + imm8;
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->ram.write32(d_addr, current_rt);
}


void Cpu::execute_op16_ld_reg_sp_rel(uint16_t ins16)
{
	/* load reg SP relative A6.7.42/T2 */
	CPU_LOG_TRACE("OP16_LD_REG_SP_REL\n");
	this->pc += 2;
	uint32_t imm8 = GET_FIELD(ins16, 0, 8) << 2;
	unsigned int rt = GET_FIELD(ins16, 8, 3);
	uint32_t current_sp = this->regs[SP].read();
	this->reg_a.write(current_sp);
	uint32_t d_addr = current_sp + imm8;
	this->regs[rt].write(this->ram.read32(d_addr));
}


void Cpu::execute_op16_shift_imm_add_sub_mov_cmp(uint16_t ins16)
{
	CPU_LOG_TRACE("OP16_SHIFT_IMM_ADD_SUB_MOV_CMP\n");
	this->pc += 2;
	uint32_t y;
	unsigned int c_out;
	unsigned int op = GET_FIELD(ins16, 9, 5);
	switch (op)
	{
		case 0 ... 11: /* LSL A6.7.67/T1, LSR A6.7.69/T1, ASR A6.7.10/T1 */
			{
				unsigned int rm = GET_FIELD(ins16, 3, 3);
				unsigned int rd = GET_FIELD(ins16, 0, 3);
				unsigned int imm5 = GET_FIELD(ins16, 6, 5);
				unsigned int type = GET_FIELD(ins16, 11, 2);
				SRType sr_type;
				unsigned int n;
				decode_imm_shift(&sr_type, &n, type, imm5);
				uint32_t current_rm = this->regs[rm].read();
				this->reg_b.write(current_rm);
				shift_c(&y, &c_out, current_rm, sr_type, n, this->flags[C]);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, this->flags[V]);
				}
			}
			break;
		case 12: /* ADD REG A6.7.4/T1 */
			{
				unsigned int rm = GET_FIELD(ins16, 6, 3);
				unsigned int rn = GET_FIELD(ins16, 3, 3);
				unsigned int rd = GET_FIELD(ins16, 0, 3);
				unsigned int v_out;
				uint32_t current_rn = this->regs[rn].read();
				uint32_t current_rm = this->regs[rm].read();
				this->reg_a.write(current_rn);
				this->reg_b.write(current_rm);
				add_c(&y, &c_out, &v_out, current_rn, current_rm, 0);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
			}
			break;
		case 13: /* SUB REG A6.7.133/T1 */
			{
				unsigned int rm = GET_FIELD(ins16, 6, 3);
				unsigned int rn = GET_FIELD(ins16, 3, 3);
				unsigned int rd = GET_FIELD(ins16, 0, 3);
				unsigned int v_out;
				uint32_t current_rn = this->regs[rn].read();
				uint32_t current_rm = this->regs[rm].read();
				this->reg_a.write(current_rn);
				this->reg_b.write(current_rm);
				add_c(&y, &c_out, &v_out, current_rn, ~current_rm, 1);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}

			}
			break;
		case 14: /* ADD IMM3 A6.7.3/T1 */
			{
				unsigned int rn = GET_FIELD(ins16, 3, 3);
				unsigned int rd = GET_FIELD(ins16, 0, 3);
				unsigned int imm3 = GET_FIELD(ins16, 6, 3);
				unsigned int v_out;
				uint32_t current_rn = this->regs[rn].read();
				this->reg_a.write(current_rn);
				add_c(&y, &c_out, &v_out, current_rn, imm3, 0);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
			}
			break;
		case 15: /* SUB IMM3 A6.7.132/T1 */
			{
				unsigned int rn = GET_FIELD(ins16, 3, 3);
				unsigned int rd = GET_FIELD(ins16, 0, 3);
				unsigned int imm3 = GET_FIELD(ins16, 6, 3);
				unsigned int v_out;
				uint32_t current_rn = this->regs[rn].read();
				this->reg_a.write(current_rn);
				add_c(&y, &c_out, &v_out, current_rn, ~imm3, 1);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
			}
			break;
		case 16 ... 19:/* MOV IMM A6.7.75/T1 */
			{
				unsigned int rd = GET_FIELD(ins16, 8, 3);
				uint32_t imm8 = GET_FIELD(ins16, 0, 8);
				unsigned int v_out;
				add_c(&y, &c_out, &v_out, 0, imm8, 0);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
			}
			break;
		case 20 ... 23: /* CMP IMM A6.7.27/T1 */
			{
				unsigned int rn = GET_FIELD(ins16, 8, 3);
				uint32_t imm8 = GET_FIELD(ins16, 0, 8);
				unsigned int v_out;
				uint32_t current_rn = this->regs[rn].read();
				this->reg_a.write(current_rn);
				add_c(&y, &c_out, &v_out, current_rn, ~imm8, 1);
				this->update_flags(y, c_out, v_out);
			}
			break;
		case 24 ... 27: /* ADD IMM8 A6.7.3/T2 */
			{
				unsigned int rd = GET_FIELD(ins16, 8, 3);
				uint32_t imm8 = GET_FIELD(ins16, 0, 8);
				unsigned int v_out;
				uint32_t current_rdn = this->regs[rd].read();
				this->reg_a.write(current_rdn);
				add_c(&y, &c_out, &v_out, current_rdn, imm8, 0);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
			}
			break;
		case 28 ... 31: /* SUB IMM8 A6.7.132/T2 */
			{
				unsigned int rd = GET_FIELD(ins16, 8, 3);
				uint32_t imm8 = GET_FIELD(ins16, 0, 8);
				uint32_t y;
				unsigned int c_out;
				unsigned int v_out;
				uint32_t current_rd = this->regs[rd].read();
				this->reg_a.write(current_rd);
				add_c(&y, &c_out, &v_out, current_rd, ~imm8, 1);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
			}
			break;
		default:
			this->report_error("undefined opcode", "OP16_SHIFT_IMM_ADD_SUB_MOV_CMP");
			break;
	}
}


void Cpu::execute_op16_special_data_branch(uint16_t ins16)
{
	CPU_LOG_TRACE("OP16_SPECIAL_DATA_BRANCH\n"); /* A5.2.3 */
	unsigned int op = GET_FIELD(ins16, 6, 4);
	unsigned int rd;
	unsigned int rm;
	uint32_t y;
	unsigned int c_out;
	unsigned int v_out;
	switch (op)
	{
		case 0 ... 3: /* ADD REG A6.7.4/T2 */
			{
				rd = GET_FIELD(ins16, 0, 2) | (GET_BIT(ins16, 7) << 3);
				rm = GET_FIELD(ins16, 3, 4);
				uint32_t current_rd = this->regs[rd].read();
				uint32_t current_rm = this->regs[rm].read();
				this->reg_a.write(current_rd);
				this->reg_b.write(current_rm);
				add_c(&y, &c_out, &v_out, current_rd, current_rm, 0);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
				this->pc += 2;
			}
			break;
		case 5 ... 7: /* CMP REG A6.7.28/T2 */
			{
				rd = GET_FIELD(ins16, 0, 2) | (GET_BIT(ins16, 7) << 3);
				rm = GET_FIELD(ins16, 3, 4);
				uint32_t current_rd = this->regs[rd].read();
				uint32_t current_rm = this->regs[rm].read();
				this->reg_a.write(current_rd);
				this->reg_b.write(current_rm);
				add_c(&y, &c_out, &v_out, current_rd, current_rm, 0);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
				this->pc += 2;
			}
			break;
		case 8 ... 11: /* MOV REG A6.7.76/T1 */
			{
				rd = GET_FIELD(ins16, 0, 2) | (GET_BIT(ins16, 7) << 3);
				rm = GET_FIELD(ins16, 3, 4);
				uint32_t current_rm = this->regs[rm].read();
				this->reg_b.write(current_rm);
				add_c(&y, &c_out, &v_out, 0, current_rm, 0);
				this->regs[rd].write(y);
				if (!this->in_it_block())
				{
					this->update_flags(y, c_out, v_out);
				}
				this->pc += 2;
			}
			break;
		case 12 ... 13: /* BRANCH AND EXCHANGE A6.7.20/T1 */
			{
				rm = GET_FIELD(ins16, 3, 4);
				uint32_t current_rm = this->regs[rm].read();
				this->reg_b.write(current_rm);
				this->pc = current_rm;
				break;
			}
		case 14 ... 15: /* BRANCH WITH LINK AND EXCHANGE */
			{
				rm = GET_FIELD(ins16, 3, 4);
				this->regs[LR].write(this->pc + 2);
				uint32_t current_rm = this->regs[rm].read();
				this->reg_b.write(current_rm);
				this->pc = current_rm;
			}
			break;
		default: /* includes case '4' */
			this->report_error("undefined opcode", "OP16_SPECIAL_DATA_BRANCH");
			break;
	}
}


void Cpu::execute_op16_ldmia(uint16_t ins16)
{
	/* load multiple register A6.7.40/T1 */
	CPU_LOG_TRACE("OP16_LDMIA\n");
	this->pc += 2;
	unsigned int rn = GET_FIELD(ins16, 8, 3);
	unsigned int register_list = GET_FIELD(ins16, 0, 8);
	uint32_t d_addr = this->regs[rn].read();
	this->reg_a.write(d_addr);
	uint32_t f_addr = d_addr + 4*bit_count(register_list);
	if (GET_BIT(register_list, rn) == 0)
	{
		this->regs[rn].write(f_addr);
	}
	for (unsigned int i = 0; i < 8; i++)
	{
		if (GET_BIT(register_list, i) == 1)
		{
			this->regs[i].write(this->ram.read32(d_addr));
			d_addr += 4;
		}
	}
}


void Cpu::execute_op16_str_imm(uint16_t ins16)
{
	/* store word immediate A6.7.119/T1 */
	CPU_LOG_TRACE("OP16_STR_IMM\n");
	this->pc += 2;
	unsigned int rn = GET_FIELD(ins16, 3, 3);
	unsigned int rt = GET_FIELD(ins16, 0, 3);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t imm32 = GET_FIELD(ins16, 6, 5) << 2;
	uint32_t d_addr = current_rn + imm32;
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->ram.write32(d_addr, current_rt);
}

void Cpu::execute_op16_nop(void)
{
	/* NOP A6.7.87/T1 */
	CPU_LOG_TRACE("OP16_NOP\n");
	this->pc += 2;
}


void Cpu::execute_op16_breakpoint(uint16_t ins16)
{
	/* BKPT A6.7.17 */
	CPU_LOG_TRACE("OP16_BREAKPOINT\n");
}


void Cpu::execute_op16_cond_branch(uint16_t ins16)
{
	/* conditional branch A6.7.12/T1 */
	CPU_LOG_TRACE("OP16_COND_BRANCH\n");
	unsigned int cond = GET_FIELD(ins16, 8, 4);
	int8_t imm8 = (int8_t)GET_FIELD(ins16, 0, 8) & 0xff;
	int32_t offset = imm8*2;
	this->execute_conditional_branch(cond, offset, false);
}


void Cpu::execute_op16_ld_literal_pool(uint16_t ins16)
{
	/* LDR (literal pool) A6.7.43/T1 */
	CPU_LOG_TRACE("OP16_LD_LITERAL_POOL\n");
	this->pc += 2;
	unsigned int rt = GET_FIELD(ins16, 8, 3);
	unsigned int imm8 = GET_FIELD(ins16, 0, 8);
	uint32_t imm32 = (imm8 << 2);
	uint32_t base = (this->pc & 0xfffffffcU) + 2;
	uint32_t addr = base + imm32;
	uint32_t data = this->ram.read32(addr);
	this->regs[rt].write(data);
}

void Cpu::execute_op16_ld_imm(uint16_t ins16)
{
	/* LDR (imm) A6.7.42/T1 */
	CPU_LOG_TRACE("OP16_LD_IMM\n");
	this->pc += 2;
	unsigned int rt = GET_FIELD(ins16, 0, 3);
	unsigned int rn = GET_FIELD(ins16, 3, 3);
	unsigned int imm5 = GET_FIELD(ins16, 6, 5);
	uint32_t imm32 = imm5 << 2;
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr = current_rn + imm32;
	uint32_t data = this->ram.read32(d_addr);
	this->regs[rt].write(data);
}

void Cpu::execute_op16_ldrb_imm(uint16_t ins16)
{
	/* LDRB (imm) A6.7.45/T1 */
	CPU_LOG_TRACE("OP16_LDRB_IMM\n");
	this->pc += 2;
	unsigned int rt = GET_FIELD(ins16, 0, 3);
	unsigned int rn = GET_FIELD(ins16, 3, 3);
	unsigned int imm5 = GET_FIELD(ins16, 6, 5);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr = current_rn + imm5;
	uint32_t data = this->ram.read8(d_addr);
	this->regs[rt].write(data);
}

void Cpu::execute_op16_ldrb_reg(uint16_t ins16)
{
	/* LDRB (reg) A6.7.47/T1 */
	CPU_LOG_TRACE("OP16_LDRB_REG\n");
	this->pc += 2;
	unsigned int rt = GET_FIELD(ins16, 0, 3);
	unsigned int rn = GET_FIELD(ins16, 3, 3);
	unsigned int rm = GET_FIELD(ins16, 6, 3);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	uint32_t addr = current_rn + current_rm;
	uint8_t data = this->ram.read8(addr);
	this->regs[rt].write(data);
}

void Cpu::execute_op16_strb_imm(uint16_t ins16)
{
	/* LDRB (imm) A6.7.121/T1 */
	CPU_LOG_TRACE("OP16_STRB_IMM\n");
	this->pc += 2;
	unsigned int rt = GET_FIELD(ins16, 0, 3);
	unsigned int rn = GET_FIELD(ins16, 3, 3);
	unsigned int imm5 = GET_FIELD(ins16, 6, 5);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	uint32_t d_addr = current_rn + imm5;
	this->ram.write8(d_addr, current_rt);
}

void Cpu::execute_op16_strb_reg(uint16_t ins16)
{
	/* LDRB (imm) A6.7.122/T1 */
	CPU_LOG_TRACE("OP16_STRB_REG\n");
	this->pc += 2;
	unsigned int rn = GET_FIELD(ins16, 3, 3);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	unsigned int rm = GET_FIELD(ins16, 6, 3);
	uint32_t current_rm = this->regs[rm].read();
	uint32_t addr = current_rn + current_rm;
	unsigned int rt = GET_FIELD(ins16, 0, 3);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_a.write(current_rt);
	this->ram.write8(addr, current_rt);
}

void Cpu::execute_op16_rev(uint16_t ins16)
{
	/* REV A6.7.100/T1 */
	CPU_LOG_TRACE("OP16_REV\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	uint32_t rev = (current_rm & 0xff) << 24;
	rev |= (((current_rm >>  8) & 0xff) << 16);
	rev |= (((current_rm >> 16) & 0xff) << 8);
	rev |= ((current_rm >> 24) & 0xff);
	this->regs[rd].write(rev);
}

void Cpu::execute_op16_rev16(uint16_t ins16)
{
	/* REV16 A6.7.101/T1 */
	CPU_LOG_TRACE("OP16_REV16\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	uint32_t rev = ((current_rm >> 16) & 0xff) << 24;
	rev |= ((current_rm >> 24) & 0xff) << 16;
	rev |= (current_rm & 0xff) << 8;
	rev |= ((current_rm >> 8) & 0xff);
	this->regs[rd].write(rev);
}

void Cpu::execute_op16_revsh(uint16_t ins16)
{
	/* REVSH A6.7.102/T1 */
	CPU_LOG_TRACE("OP16_REVSH\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	uint32_t low_byte = (current_rm & 0xff);
	uint32_t hig_byte = (current_rm >> 8) & 0xff;
	uint32_t res;
	if ((low_byte & 0x80) == 0x80)
	{
		/* low byte is negative, so sign extend */
		res = 0xffff0000 | (low_byte << 8) | hig_byte;
		
	}
	else
	{
		/* low byte is positive */
		res = (low_byte << 8) | hig_byte;
	}
	this->regs[rd].write(res);
}

void Cpu::execute_op16_uxtb(uint16_t ins16)
{
	/* UXTB A6.7.149/T1 */
	CPU_LOG_TRACE("OP16_UXTB\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	this->regs[rd].write(current_rm & 0xff);
}

void Cpu::execute_op16_uxth(uint16_t ins16)
{
	/* UXTB A6.7.150/T1 */
	CPU_LOG_TRACE("OP16_UXTH\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	this->regs[rd].write(current_rm & 0xffff);
}

void Cpu::execute_op16_sxtb(uint16_t ins16)
{
	/* SXTB A6.7.137/T1 */
	CPU_LOG_TRACE("OP16_SXTB\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	if (GET_BIT(current_rm, 7) == 1)
	{
		current_rm |= 0xffffff00;
	}
	else
	{
		current_rm &= 0x000000ff;
	}
	this->regs[rd].write(current_rm);
}

void Cpu::execute_op16_sxth(uint16_t ins16)
{
	/* SXTH A6.7.138/T1 */
	CPU_LOG_TRACE("OP16_SXTH\n");
	this->pc += 2;
	unsigned rm = GET_FIELD(ins16, 3, 3);
	unsigned rd = GET_FIELD(ins16, 0, 3);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	if (GET_BIT(current_rm, 7) == 1)
	{
		current_rm |= 0xffff0000;
	}
	else
	{
		current_rm &= 0x0000ffff;
	}
	this->regs[rd].write(current_rm);
}


/******************************************************************************
 * 32-bit instructions
 ******************************************************************************/
void Cpu::execute_op32_ldmia(uint16_t ins16, uint16_t ins16_b)
{
	/* LDMIA A6.7.40/T2 */
	CPU_LOG_TRACE("OP32_LDMIA\n");
	unsigned int w = GET_BIT(ins16, 5);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int register_list = ins16_b;
	uint32_t d_addr = this->regs[rn].read();
	this->reg_a.write(d_addr);
	unsigned int p = GET_BIT(ins16_b, 15);
	uint32_t f_addr = d_addr + 4*bit_count(register_list);
	if (w == 1)
	{
		this->regs[rn].write(f_addr);
	}
	for (unsigned int i = 1; i < 15; ++i)
	{
		if (GET_BIT(register_list, i) == 1)
		{
			this->regs[i].write(this->ram.read32(d_addr));
			d_addr += 4;
		}
	}
	if (p == 1)
	{
		this->pc = this->ram.read32(d_addr);
		d_addr += 4;
	}
	if (p == 0)
	{
		this->pc += 4;
	}
}


void Cpu::execute_op32_stmia(uint16_t ins16, uint16_t ins16_b)
{
	/* STMIA A6.7.117/T2 */
	CPU_LOG_TRACE("OP32_STMIA\n");
	this->pc += 4;
	unsigned int w = GET_BIT(ins16, 5);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int register_list = ins16_b;
	uint32_t d_addr = this->regs[rn].read();
	this->reg_a.write(d_addr);
	for (unsigned int i = 0; i < 15; i++)
	{
		if (GET_BIT(register_list, i) == 1)
		{
			this->ram.write32(d_addr, this->regs[i].read());
			this->reg_a.write(this->regs[i].read());
			d_addr += 4;
		}
	}
	if (w == 1)
	{
		this->regs[rn].write(d_addr);
	}
}


void Cpu::execute_op32_stmdb(uint16_t ins16, uint16_t ins16_b)
{
	/* STMDB A6.7.118/T1 */
	CPU_LOG_TRACE("OP32_STMDB\n");
	this->pc += 4;
	unsigned int w = GET_BIT(ins16, 5);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int register_list = ins16_b;
	unsigned int register_count = bit_count(register_list);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr_base = current_rn - 4*register_count;
	uint32_t d_addr = d_addr_base;
	for (unsigned int i = 0; i < 15; ++i)
	{
		if (GET_BIT(register_list, i) == 1)
		{
			uint32_t val = this->regs[i].read();
			this->reg_a.write(val);
			this->ram.write32(d_addr, val);
			d_addr += 4;
		}
	}
	if (w == 1)
	{
		this->regs[rn].write(d_addr_base);
	}
}


void Cpu::execute_op32_ldmdb(uint16_t ins16, uint16_t ins16_b)
{
	/* LDMDB A6.7.41/T1 */
	CPU_LOG_TRACE("OP32_LDMDB\n");
	this->pc += 4;
	unsigned int w = GET_BIT(ins16, 5);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int register_list = ins16_b;
	unsigned int register_count = bit_count(register_list);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr_base = current_rn - 4*register_count;
	uint32_t d_addr = d_addr_base;
	for (unsigned int i = 0; i < 15; ++i)
	{
		if (GET_BIT(register_list, i) == 1)
		{
		    this->regs[i].write(this->ram.read32(d_addr));
			d_addr += 4;
		}
	}
	if (w == 1)
	{
		this->regs[rn].write(d_addr_base);
	}
}


void Cpu::execute_op32_data_shifted_reg(uint16_t ins16, uint16_t ins16_b)
{
	/* data processing (shifted register) A5.3.11 */
	CPU_LOG_TRACE("OP32_DATA_SHIFTED_REG\n");
	this->pc += 4;
	unsigned int alu_op = GET_FIELD(ins16, 5, 4);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int s = GET_BIT(ins16, 4);
	unsigned int rd = GET_FIELD(ins16_b, 8, 4);
	unsigned int rm = GET_FIELD(ins16_b, 0, 4);
	unsigned int imm = (GET_FIELD(ins16_b, 12, 3) << 2) | GET_FIELD(ins16_b, 6, 2);
	unsigned int type = GET_FIELD(ins16_b, 4, 2);
	SRType srtype;
	unsigned int n;
	decode_imm_shift(&srtype, &n, type, imm);
	uint32_t b;
	unsigned int c_out;
	shift_c(&b, &c_out, this->regs[rm].read(), srtype, n, this->flags[C]);
	if (s == 1)
	{
		this->flags[C] = c_out;
	}
	this->reg_a.write(this->regs[rn].read());
	this->reg_b.write(this->regs[rm].read());
	/* C flag may be overriden by the next procedure but it is intended */
	this->execute_alu_op(alu_op, rd, rn, s, b);
}


void Cpu::execute_op32_data_mod_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* data processing (modified immediate) A5.3.1 */
	CPU_LOG_TRACE("OP32_DATA_MOD_IMM\n");
	this->pc += 4;
	unsigned int alu_op = GET_FIELD(ins16, 5, 4);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rd = GET_FIELD(ins16_b, 8, 4);
	unsigned int s = GET_BIT(ins16, 4);
	unsigned int imm12 = (GET_BIT(ins16, 10) << 11) | (GET_FIELD(ins16_b, 12, 3) << 8) | (GET_FIELD(ins16_b, 0, 8));
	uint32_t imm32;
	unsigned int c_out;
	thumb_expand_imm_c(&imm32, &c_out, imm12, this->flags[C]);
	if (s == 1)
	{
		this->flags[C] = c_out;
	}
	this->reg_a.write(this->regs[rn].read());
	/* C flag may be overriden by the next procedure but it is intended */
	this->execute_alu_op(alu_op, rd, rn, s, imm32);
}


void Cpu::execute_op32_data_plain_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* data processing (plain binary immediate) A5.3.3 */
	CPU_LOG_TRACE("OP32_DATA_PLAIN_IMM\n");
	this->pc += 4;
	unsigned int op = GET_FIELD(ins16, 4, 5);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	if (op == 0)
	{
		if (rn == 15)
		{ /* ADR A6.7.7/T3 */
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
			unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
			unsigned int i = GET_BIT(ins16, 10);
			uint32_t imm32 = (i << 11) | (imm3 << 8) | imm8;
			uint32_t res = (this->pc & 0xfffffffc) + imm32;
			this->reg_a.write(this->pc);
			this->regs[rd].write(res);
		}
		else
		{ /* ADD (12-bit) A6.7.3/T4 */
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
			unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
			unsigned int i = GET_BIT(ins16, 10);
			uint32_t imm32 = (i << 11) | (imm3 << 8) | imm8;
			uint32_t current_rn = this->regs[rn].read();
			this->reg_a.write(current_rn);
			uint32_t y;
			unsigned int c_out;
			unsigned int v_out;
			add_c(&y, &c_out, &v_out, current_rn, imm32, 0);
			this->regs[rd].write(y);
		}
	}
	else if (op == 4)
	{ /* MOV (16-bit) A6.7.75/T4 */
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
		unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
		unsigned int imm4 = GET_FIELD(ins16, 0, 4);
		unsigned int i = GET_BIT(ins16, 10);
		uint32_t imm32 =  (imm4 << 12)| (i << 11) | (imm3 << 8) | imm8;
		uint32_t current_rd = this->regs[rd].read();
		this->reg_b.write(current_rd);
		this->regs[rd].write(imm32);
	}
	else if (op == 10)
	{
		if (rn == 15)
		{ /* ADR A6.7.7/T2 */
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
			unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
			unsigned int i = GET_BIT(ins16, 10);
			uint32_t imm32 = (i << 11) | (imm3 << 8) | imm8;
			uint32_t res = (this->pc & 0xfffffffc) - imm32;
			this->reg_a.write(this->pc);
			this->regs[rd].write(res);
		}
		else
		{ /* SUB (12-bit) A6.7.132/T4 */
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
			unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
			unsigned int i = GET_BIT(ins16, 10);
			uint32_t imm32 = (i << 11) | (imm3 << 8) | imm8;
			uint32_t current_rn = this->regs[rn].read();
			this->reg_a.write(current_rn);
			uint32_t y;
			unsigned int c_out;
			unsigned int v_out;
			add_c(&y, &c_out, &v_out, current_rn, ~imm32, 1);
			this->regs[rd].write(y);
		}
	}
	else if (op == 12)
	{ /* MOVT */
		unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
		unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
		unsigned int imm4 = GET_FIELD(ins16, 0, 4);
		unsigned int i = GET_BIT(ins16, 10);
		unsigned int imm16 = imm8 | (imm3 << 8) | (i << 11) | (imm4 << 12);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		uint32_t src = this->regs[rd].read();
		this->reg_b.write(src);
		uint32_t val = (src & 0xffff) | (imm16 << 16);
		this->regs[rd].write(val);
	}
	else if (op == 20)
	{ /* SBFX A6.7.110/T1 */
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int imm2 = GET_FIELD(ins16_b, 6, 2);
		unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
		unsigned int lsbit = (imm3 << 2) | imm2;
		unsigned width = GET_FIELD(ins16_b, 0, 5) + 1;
		uint32_t mask = 0xffffffffU >> (32 - width);
		uint32_t current_rd = this->regs[rd].read();
		this->reg_a.write(current_rd);
		uint32_t current_rn = this->regs[rn].read();
		this->reg_b.write(current_rn);
		uint32_t field = (current_rn >> lsbit)  & mask;
		if ((field >> (width - 1)) == 1)
		{
			field |= (0xffffffffU << width);
		}
		this->regs[rd].write(field);

	}
	else if (op == 22)
	{
		if (rn == 15)
		{ /* BFC A6.7.13/T1 */
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int imm2 = GET_FIELD(ins16_b, 6, 2);
			unsigned int imm3 = GET_FIELD(ins16_b, 12, 4);
			unsigned int lsbit = (imm3 << 2) | imm2;
			unsigned int msbit = GET_FIELD(ins16_b, 0, 5);
			uint32_t mask = (0xffffffffU << (msbit + 1)) | (0xffffffffU >> (32 - lsbit));
			uint32_t current_rd = this->regs[rd].read();
			this->reg_a.write(current_rd);
			this->reg_b.write(this->pc);
			uint32_t next_rd = current_rd & mask;
			this->regs[rd].write(next_rd);
		}
		else
		{ /* BFI A6.7.14/T1 */
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int imm2 = GET_FIELD(ins16_b, 6, 2);
			unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
			unsigned int msbit = GET_FIELD(ins16_b, 0, 5);
			unsigned int lsbit = imm2 | (imm3 << 2);
			unsigned int width = msbit - lsbit + 1;
			uint32_t mask = 0xffffffffU >> (32 - width);
			uint32_t current_rn = this->regs[rn].read();
			uint32_t field = current_rn & mask;
			uint32_t val = this->regs[rd].read();
			this->reg_a.write(val);
			this->reg_b.write(current_rn);
			val = val & ~(mask << lsbit);
			val = val | (field << lsbit);
			this->regs[rd].write(val);
		}
	}
	else if (op == 28)
	{ /* UBFX A6.7.144/T1 */
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int imm2 = GET_FIELD(ins16_b, 6, 2);
		unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
		unsigned int lsbit = (imm3 << 2) | imm2;
		unsigned width = GET_FIELD(ins16_b, 0, 5) + 1;
		uint32_t mask = 0xffffffffU >> (32 - width);
		uint32_t current_rd = this->regs[rd].read();
		this->reg_a.write(current_rd);
		uint32_t current_rn = this->regs[rn].read();
		this->reg_b.write(current_rn);
		uint32_t field = (current_rn >> lsbit)  & mask;
		this->regs[rd].write(field);
	}
	else
	{
		this->report_error("unsupported operand", "OP32_DATA_PLAIN_IMM");
	}
}


void Cpu::execute_op32_data_reg(uint16_t ins16, uint16_t ins16_b)
{
	/* data processing (register) A5.3.12 */
	CPU_LOG_TRACE("OP32_DATA_REG\n");
	this->pc += 4;
	unsigned int op1 = GET_FIELD(ins16, 4, 4);
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int op2 = GET_FIELD(ins16_b, 4, 4);
	if ((op1 == 0 || op1 == 1) && (op2 == 0))
	{ /* LSL A6.7.68/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int s = GET_BIT(ins16, 4);
		uint32_t y;
		unsigned c_out;
		uint32_t current_rn = this->regs[rn].read();
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(current_rm);
		this->reg_b.write(current_rn);
		shift_c(&y, &c_out, current_rn, SRType_LSL, current_rm, this->flags[C]);
		this->regs[rd].write(y);
		if (s == 1)
		{
			this->update_flags(y, c_out, this->flags[Z]);
		}
	}
	else if ((op1 == 0) && (GET_BIT(op2, 3) == 1))
	{ /* SXTH A6.7.138/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int rot = GET_FIELD(ins16_b, 4, 2) << 3;
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(this->pc);
		this->reg_b.write(current_rm);
		uint32_t y;
		ror(&y, current_rm, rot);
		if (GET_BIT(y, 15) == 1)
		{
			y |= 0xffff0000;
		}
		else
		{
			y &= 0x0000ffff;
		}
		this->regs[rd].write(y);
	}
	else if ((op1 == 1) && (GET_BIT(op2, 3) == 1))
	{ /* UXTH A6.7.150/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int rot = GET_FIELD(ins16_b, 4, 2) << 3;
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(this->pc);
		this->reg_b.write(current_rm);
		uint32_t y;
		ror(&y, current_rm, rot);
		this->regs[rd].write(y & 0xffff);
	}
	else if ((op1 == 2) || (op1 == 3))
	{ /* LSR A6.7.60/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int s = GET_BIT(ins16, 4);
		uint32_t y;
		unsigned c_out;
		uint32_t current_rn = this->regs[rn].read();
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(current_rm);
		this->reg_b.write(current_rn);
		shift_c(&y, &c_out, current_rn, SRType_LSR, current_rm, this->flags[C]);
		this->regs[rd].write(y);
		if (s == 1)
		{
			this->update_flags(y, c_out, this->flags[Z]);
		}
	}
	else if ((op1 == 4 || op1 == 5) && (op2 == 0))
	{ /* ASR A6.7.11/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int s = GET_BIT(ins16, 4);
		uint32_t y;
		unsigned c_out;
		uint32_t current_rn = this->regs[rn].read();
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(current_rm);
		this->reg_b.write(current_rn);
		shift_c(&y, &c_out, current_rn, SRType_ASR, current_rm, this->flags[C]);
		this->regs[rd].write(y);
		if (s == 1)
		{
			this->update_flags(y, c_out, this->flags[Z]);
		}
	}
	else if ((op1 == 4) && (GET_BIT(op2, 3) == 1))
	{ /* SXTB A6.7.137/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int rot = GET_FIELD(ins16_b, 4, 2) << 3;
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(this->pc);
		this->reg_b.write(current_rm);
		uint32_t y;
		ror(&y, current_rm, rot);
		if (GET_BIT(y, 7) == 1)
		{
			y |= 0xffffff00;
		}
		else
		{
			y &= 0x000000ff;
		}
		this->regs[rd].write(y);
	}
	else if ((op1 == 5) && (GET_BIT(op2, 3) == 1))
	{ /* UXTB A6.7.149/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int rot = GET_FIELD(ins16_b, 4, 2) << 3;
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(this->pc);
		this->reg_b.write(current_rm);
		uint32_t y;
		ror(&y, current_rm, rot);
		this->regs[rd].write(y & 0xff);
	}
	else if ((op1 == 6 || op1 == 7) && (op2 == 0))
	{ /* ROR A6.7.104/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		unsigned int s = GET_BIT(ins16, 4);
		uint32_t y;
		unsigned c_out;
		uint32_t current_rn = this->regs[rn].read();
		uint32_t current_rm = this->regs[rm].read();
		this->reg_a.write(current_rm);
		this->reg_b.write(current_rn);
		shift_c(&y, &c_out, current_rn, SRType_ROR, current_rm, this->flags[C]);
		this->regs[rd].write(y);
		if (s == 1)
		{
			this->update_flags(y, c_out, this->flags[Z]);
		}
	}
	else if ((op1 == 9) && (op2 == 10))
	{
		/* RBIT A6.7.99/T1 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		uint32_t current_rm = this->regs[rm].read();
		this->reg_b.write(current_rm);
		uint32_t rev = 0;
		for (unsigned int i = 0; i < 32; ++i)
		{
			rev <<= 1;
			rev |= (current_rm & 1);
			current_rm >>= 1;
		}
		this->regs[rd].write(rev);
	}
	else if ((op1 == 9) && (op2 == 8))
	{
		/* REV A6.7.100/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		uint32_t current_rm = this->regs[rm].read();
		this->reg_b.write(current_rm);
		uint32_t rev = (current_rm & 0xff) << 24;
		rev |= (((current_rm >>  8) & 0xff) << 16);
		rev |= (((current_rm >> 16) & 0xff) << 8);
		rev |= ((current_rm >> 24) & 0xff);
		this->regs[rd].write(rev);
	}
	else if ((op1 == 9) && (op2 == 9))
	{
		/* REV16 A6.7.101/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		uint32_t current_rm = this->regs[rm].read();
		this->reg_b.write(current_rm);
		uint32_t rev = ((current_rm >> 16) & 0xff) << 24;
		rev |= ((current_rm >> 24) & 0xff) << 16;
		rev |= (current_rm & 0xff) << 8;
		rev |= ((current_rm >> 8) & 0xff);
		this->regs[rd].write(rev);
	}
	else if ((op1 == 11) && (op2 == 8))
	{
		/* CLZ A6.7.24/T1 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		uint32_t current_rm = this->regs[rm].read();
		this->reg_b.write(current_rm);
		uint32_t nlz = 0;
		for (unsigned int i = 0; i < 32; ++i)
		{
			if (GET_BIT(current_rm, 31 - i) == 1)
			{
				break;
			}
			else
			{
				nlz++;
			}
		}
		this->regs[rd].write(nlz);
	}
	else if ((op1 == 9) && (op2 == 11))
	{
		/* REVSH A6.7.102/T2 */
		unsigned int rm = GET_FIELD(ins16_b, 0, 4);
		unsigned int rd = GET_FIELD(ins16_b, 8, 4);
		uint32_t current_rm = this->regs[rm].read();
		this->reg_b.write(current_rm);
		uint32_t low_byte = (current_rm & 0xff);
		uint32_t hig_byte = (current_rm >> 8) & 0xff;
		uint32_t res;
		if ((low_byte & 0x80) == 0x80)
		{
			/* low byte is negative, so sign extend */
			res = 0xffff0000 | (low_byte << 8) | hig_byte;
			
		}
		else
		{
			/* low byte is positive */
			res = (low_byte << 8) | hig_byte;
		}
		this->regs[rd].write(res);
	}
	else
	{
		this->report_error("unsupported instruction", "OP32_DATA_REG");
	}
}


void Cpu::execute_op32_branch_misc(uint16_t ins16, uint16_t ins16_b)
{
	/* branch and miscellanous control A5.3.4 */
	CPU_LOG_TRACE("OP32_BRANCH_MISC\n");
	unsigned int op1 = GET_FIELD(ins16, 4, 7);
	unsigned int op2 = GET_FIELD(ins16_b, 12, 3);
	if (((op2 & 0x05) == 0x00) && ((op1 & 0x38) != 0x38))
	{ /* B A6.7.12/T3 */
		/* only valid outside of an IT block!! */
		unsigned int cond = GET_FIELD(ins16, 6, 4);
		unsigned int imm11 = GET_FIELD(ins16_b, 0, 11);
		unsigned int imm6 = GET_FIELD(ins16, 0, 6);
		unsigned int s = GET_BIT(ins16, 10);
		unsigned int i2 = 1 - (GET_BIT(ins16_b, 11) ^ s);
		unsigned int i1 = 1 - (GET_BIT(ins16_b, 13) ^ s);
		unsigned int offset_u = (imm11 << 1) | (imm6 << 12) | (i2 << 18) | (i1 << 19) | (s << 20);
		int32_t offset = offset_u;
		if (s == 1)
		{
			offset |= 0xffe00000U;
		}
        this->execute_conditional_branch(cond, offset, true);
	}
	else
	{
		report_error("unsupported instruction", "OP32_BRANCH_MISC_MASK");
	}
}


void Cpu::execute_op32_str_imm12(uint16_t ins16, uint16_t ins16_b)
{
	/* STR(immediate) A6.7.119/T3 */
	CPU_LOG_TRACE("OP32_STR_IMM12\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm12 = GET_FIELD(ins16_b, 0, 12);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr = current_rn + imm12;
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->ram.write32(d_addr, current_rt);
}


void Cpu::execute_op32_str_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* STR(immediate) A6.7.119/T4 */
	CPU_LOG_TRACE("OP32_STR_IMM\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
	unsigned int p = GET_BIT(ins16_b, 10);
	unsigned int u = GET_BIT(ins16_b, 9);
	unsigned int w = GET_BIT(ins16_b, 8);
	uint32_t d_addr = this->regs[rn].read();
	this->reg_a.write(d_addr);
	uint32_t offset_addr;
	if (u == 1)
	{
		offset_addr = d_addr + imm8;
	}
	else
	{
		offset_addr = d_addr - imm8;
	}
	if (p == 1)
	{
		d_addr = offset_addr;
	}
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->ram.write32(d_addr, current_rt);
	if (w == 1)
	{
		this->regs[rn].write(offset_addr);
	}
}


void Cpu::execute_op32_ldr_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* LDR(immediate) A6.7.42/T4 */
	CPU_LOG_TRACE("OP32_LDR_IMM\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
	unsigned int p = GET_BIT(ins16_b, 10);
	unsigned int u = GET_BIT(ins16_b, 9);
	unsigned int w = GET_BIT(ins16_b, 8);
	uint32_t d_addr = this->regs[rn].read();
	this->reg_a.write(d_addr);
	uint32_t offset_addr;
	if (u == 1)
	{
		offset_addr = d_addr + imm8;
	}
	else
	{
		offset_addr = d_addr - imm8;
	}
	if (p == 1)
	{
		d_addr = offset_addr;
	}
	uint32_t data = this->ram.read32(d_addr);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	if (w == 1)
	{
		this->regs[rn].write(offset_addr);
	}
	this->regs[rt].write(data);
}


void Cpu::execute_op32_ldr_imm12(uint16_t ins16, uint16_t ins16_b)
{
	/* LDR(immediate) A6.7.42/T3 */
	CPU_LOG_TRACE("OP32_LDR_IMM12\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm12 = GET_FIELD(ins16_b, 0, 12);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr = current_rn + imm12;
	uint32_t data = this->ram.read32(d_addr);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->regs[rt].write(data);
}


void Cpu::execute_op32_ldrb_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* LDRB (imm) A6.7.45/T2 */
	CPU_LOG_TRACE("OP32_LDRB_IMM\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm12 = GET_FIELD(ins16_b, 0, 12);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr = current_rn + imm12;
	uint32_t data = this->ram.read8(d_addr);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->regs[rt].write(data);
}

void Cpu::execute_op32_ldrb_imm_alt(uint16_t ins16, uint16_t ins16_b)
{
	/* LDRB (imm) A6.7.45/T3 */
	CPU_LOG_TRACE("OP32_LDRB_IMM_ALT\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm32 = GET_FIELD(ins16_b, 0, 8);
	unsigned int p = GET_BIT(ins16_b, 10);
	unsigned int u = GET_BIT(ins16_b, 9);
	unsigned int w = GET_BIT(ins16_b, 8);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	uint32_t offset_addr = (u == 1) ? current_rn + imm32 : current_rn - imm32;
	uint32_t addr = (p == 1) ? offset_addr : current_rn;
	uint32_t data = this->ram.read8(addr);
	this->regs[rt].write(data);
	if (w == 1)
	{
		this->regs[rn].write(offset_addr);
	}
}

void Cpu::execute_op32_ldrb_reg(uint16_t ins16, uint16_t ins16_b)
{
	/* LDRB (reg) A6.7.47/T2 */
	CPU_LOG_TRACE("OP32_LDRB_REG\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int rm = GET_FIELD(ins16_b, 0, 4);
	unsigned int imm2 = GET_FIELD(ins16_b, 4, 2);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	uint32_t offset;
	shift(&offset, current_rm, SRType_LSL, imm2, this->flags[C]);
	uint32_t addr = current_rn + offset;
	uint8_t data = this->ram.read8(addr);
	this->regs[rt].write(data);
}

void Cpu::execute_op32_strb_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* STRB (imm) A6.7.121/T2 */
	CPU_LOG_TRACE("OP32_STRB_IMM\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm12 = GET_FIELD(ins16_b, 0, 12);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t d_addr = current_rn + imm12;
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	this->ram.write8(d_addr, current_rt);
}

void Cpu::execute_op32_strb_imm_alt(uint16_t ins16, uint16_t ins16_b)
{
	/* STRB (imm) A6.7.121/T3 */
	CPU_LOG_TRACE("OP32_STRB_IMM_ALT\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm32 = GET_FIELD(ins16_b, 0, 12);
	unsigned int p = GET_BIT(ins16_b, 10);
	unsigned int u = GET_BIT(ins16_b, 9);
	unsigned int w = GET_BIT(ins16_b, 8);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_b.write(current_rt);
	uint32_t offset_addr = (u == 1) ? current_rn + imm32 : current_rn - imm32;
	uint32_t addr = (p == 1) ? offset_addr : current_rn;
	this->ram.write8(addr, current_rt);
	if (w == 1)
	{
		this->regs[rn].write(offset_addr);
	}
}

void Cpu::execute_op32_strb_reg(uint16_t ins16, uint16_t ins16_b)
{
	/* STRB (reg) A6.7.122/T2 */
	CPU_LOG_TRACE("OP32_STRB_REG\n");
	this->pc += 4;
	unsigned int rn = GET_FIELD(ins16, 0, 4);
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int rm = GET_FIELD(ins16_b, 0, 4);
	unsigned int shift_n = GET_FIELD(ins16_b, 4, 2);

	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t current_rm = this->regs[rm].read();
	this->reg_b.write(current_rm);
	uint32_t current_rt = this->regs[rt].read();
	this->reg_a.write(current_rt);
	uint32_t offset;
	shift(&offset, current_rm, SRType_LSL, shift_n, this->flags[C]);
	uint32_t addr = current_rn + offset;
	this->ram.write8(addr, current_rt);
}

void Cpu::execute_op32_ld_literal_pool(uint16_t ins16, uint16_t ins16_b)
{
	/* LDR (literal pool) A6.7.43/T2 */
	CPU_LOG_TRACE("OP32_LD_LITERAL_POOL\n");
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm32 = GET_FIELD(ins16_b, 0, 12);
	unsigned int u = GET_BIT(ins16, 7);
	uint32_t base = (this->pc & 0xfffffffcU) + 4;
	uint32_t addr = (u == 1) ? base + imm32 : base - imm32;
	uint32_t data = this->ram.read32(addr);
	this->reg_a.write(this->pc + 4);
	if (rt == 15)
	{
		this->pc = data;
	}
	else
	{
		this->regs[rt].write(data);
		this->pc += 4;
	}
}

void Cpu::execute_op32_ldrb_literal(uint16_t ins16, uint16_t ins16_b)
{
	/* LDRB (literal pool) A6.7.46/T1 */
	CPU_LOG_TRACE("OP32_LDRB_LITERAL\n");
	this->pc += 4;
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int imm12 = GET_FIELD(ins16_b, 0, 12);
	unsigned int u = GET_BIT(ins16, 7);
	this->reg_a.write(this->pc);
	uint32_t base = this->pc & 0xfffffffcU;
	uint32_t addr = (u == 1) ? base + imm12 : base - imm12;
	uint32_t data = this->ram.read8(addr);
	this->regs[rt].write(data);
}

void Cpu::execute_op32_ldrd_imm(uint16_t ins16, uint16_t ins16_b)
{
	/* LDRD (imm) A6.7.49/T1 */
	CPU_LOG_TRACE("OP32_LDRD_IMM\n");
	this->pc += 4;
	unsigned int rt = GET_FIELD(ins16_b, 12, 4);
	unsigned int rt2 = GET_FIELD(ins16_b, 8, 4);
	unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
	unsigned int p = GET_BIT(ins16, 8);
	unsigned int u = GET_BIT(ins16, 7);
	unsigned int w = GET_BIT(ins16, 5);
	uint32_t rn = GET_FIELD(ins16, 0, 4);
	uint32_t current_rn = this->regs[rn].read();
	this->reg_a.write(current_rn);
	uint32_t imm32 = imm8 << 2;
	uint32_t offset_addr = (u == 1) ? current_rn + imm32 : current_rn - imm32;
	uint32_t addr = (p == 1) ? offset_addr : current_rn;
	this->regs[rt].write(this->ram.read32(addr));
	this->regs[rt2].write(this->ram.read32(addr + 4));
	if (w == 1)
	{
		this->regs[rn].write(offset_addr);
	}
}
