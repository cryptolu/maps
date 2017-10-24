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

//#define DEBUG_TRACE

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

#include "cpu.h"
#include "opcodes.h"
#include "utils.h"
#include "debug.h"

#include "rsp_layer.h"


void Cpu::report_error(const char *msg, const char *location)
{
    fprintf(stderr, "-- ERROR: %s in %s at address 0x%08x\n", msg, location, this->regs[PC].read());
    std::exit(EXIT_FAILURE);
}


Cpu::Cpu(bool with_gdb, std::string trace_index_filename)
{
	this->with_gdb = with_gdb;
	for (unsigned int i = 0; i < 16; i++)
	{
		this->regs[i].bind_tracer(&(this->tracer));
	}
	this->instruction_count = 0;
	this->trace_index_done = false;
	this->trace_index_filename = trace_index_filename;
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


void Cpu::reset(void)
{
	this->instruction_count = 0;
	this->trace_index_done = false;
	/* stack pointer set to end of RAM */
	this->regs[SP].write(this->ram.get_size() - 4);
	/* program counter set to 0 */
	this->regs[PC].write(0);
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
	if (reg_idx < 16)
	{
		res = this->regs[reg_idx].read();
	}
	else
	{
		this->report_error("reg_idx must be < 16", "Cpu::read_register()");
	}
	return res;
}


uint32_t Cpu::read_apsr(void)
{
	uint32_t apsr = (this->alu.get_n() << 31) |
	                (this->alu.get_z() << 30) |
	                (this->alu.get_c() << 29) |
	                (this->alu.get_v() << 28) |
	                (this->alu.get_q() << 27);
	return apsr;
}


void Cpu::write8_ram(unsigned int addr, uint8_t value)
{
	this->ram.write8(addr, value);
}


void Cpu::write16_ram(unsigned int addr, uint16_t value)
{
	this->ram.write16(addr, value);
}


void Cpu::write32_ram(unsigned int addr, uint32_t value)
{
	this->ram.write32(addr, value);
}


uint8_t Cpu::read8_ram(unsigned int addr)
{
	return this->ram.read8(addr);
}


uint8_t Cpu::read16_ram(unsigned int addr)
{
	return this->ram.read16(addr);
}


uint8_t Cpu::read32_ram(unsigned int addr)
{
	return this->ram.read32(addr);
}


void Cpu::dump_memory(unsigned int start, unsigned int len)
{
	this->ram.dump(start, len);
}


void Cpu::dump_regs(void)
{
	fprintf(stderr, "R0 : 0x%08x, R1 : 0x%08x, R2 : 0x%08x, R3 : 0x%08x\n", this->regs[0].read(), this->regs[1].read(), this->regs[2].read(), this->regs[3].read());
	fprintf(stderr, "R4 : 0x%08x, R5 : 0x%08x, R6 : 0x%08x, R7 : 0x%08x\n", this->regs[4].read(), this->regs[5].read(), this->regs[6].read(), this->regs[7].read());
	fprintf(stderr, "R8 : 0x%08x, R9 : 0x%08x, R10: 0x%08x, R11: 0x%08x\n", this->regs[8].read(), this->regs[9].read(), this->regs[10].read(), this->regs[11].read());
	fprintf(stderr, "R12: 0x%08x, SP : 0x%08x, LR : 0x%08x, PC : 0x%08x\n", this->regs[12].read(), this->regs[13].read(), this->regs[14].read(), this->regs[15].read());
	fprintf(stderr, "n: %u, c = %u, z = %u, v = %u, q = %u\n", this->alu.get_n(), this->alu.get_c(), this->alu.get_z(), this->alu.get_v(), this->alu.get_q());
}


uint32_t Cpu::generate_modified_immediate(uint16_t ins16, uint16_t ins16_b)
{
	unsigned int i = GET_BIT(ins16, 10);
	unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
	unsigned int a = GET_BIT(ins16_b, 7);
	unsigned int op = a | (imm3 << 1) | (i << 4);
	uint32_t abcdefgh = GET_FIELD(ins16_b, 0, 8);
	uint32_t y;
	switch (op)
	{
		case 0:
		case 1:
			y = abcdefgh;
			break;
		case 2:
		case 3:
			y = abcdefgh | (abcdefgh << 16);
			break;
		case 4:
		case 5:
			y = (abcdefgh << 8) | (abcdefgh << 24);
			break;
		case 6:
		case 7:
			y =  abcdefgh | (abcdefgh << 8) | (abcdefgh << 16) | (abcdefgh << 24);
			break;
		default:
			uint32_t one_bcdefgh = abcdefgh | (1 << 7);
			y = (one_bcdefgh << (32 - op));
			this->alu.set_c(y >> 31);
			break;
	}
	return y;
}


uint32_t Cpu::decode_imm_shift(unsigned int op, unsigned int imm, unsigned int s, uint32_t a)
{
	uint32_t y;
	unsigned int n;

	switch (op)
	{
		case OP_LSL:
			n = imm;
			y = this->alu.lsl(a, n, s);
			break;
		case OP_LSR:
			if (imm == 0)
			{
				n = 32;
			}
			else
			{
				n = imm;
			}
			y = this->alu.lsr(a, n, s);
			break;
		case OP_ASR:
			if (imm == 0)
			{
				n = 32;
			}
			else
			{
				n = imm;
			}
			y = this->alu.asr(a, n, s);
			break;
		case OP_ROR:
			if (imm == 0)
			{ /* rrx */
				this->report_error("RRX unsupported", "decode_imm_shift()");
			}
			else
			{ /* ror */
				y = this->alu.ror(a, imm, s);
			}
			break;
		default:
			this->report_error("unsupported shift opcode", "decode_imm_shift()");
			break;
	}
	return y;
}


unsigned int Cpu::conditional_branch(unsigned int cond, int32_t offset, bool is_ins32, unsigned int p_addr)
{
	unsigned int tst;
	unsigned int next_p_addr;
	switch (cond)
	{
		case 0: /* EQ */
			tst = this->alu.get_z();
			break;
		case 1: /* NE */
			tst = 1 - this->alu.get_z();
			break;
		case 2: /* CS */
			tst = this->alu.get_c();
			break;
		case 3: /* CC */
			tst = 1 - this->alu.get_c();
			break;
		case 4: /* MI */
			tst = this->alu.get_n();
			break;
		case 5: /* PL */
			tst = 1 - this->alu.get_n();
			break;
		case 6: /* VS */
			tst = this->alu.get_v();
			break;
		case 7: /* VC */
			tst = 1 -this->alu.get_v();
			break;
		case 8: /* HI */
			tst = this->alu.get_c() & (1 - this->alu.get_z());
			break;
		case 9: /* LS */
			tst = this->alu.get_z() & (1 - this->alu.get_c());
			break;
		case 10: /* GE */
			if (this->alu.get_n() == this->alu.get_v())
			{
				tst = 1;
			}
			else
			{
				tst = 0;
			}
			break;
		case 11: /* LT */
			if (this->alu.get_n() == this->alu.get_v())
			{
				tst = 0;
			}
			else
			{
				tst = 1;
			}
			break;
		case 12: /* GT */
			if (this->alu.get_z() == 0 && (this->alu.get_v() == this->alu.get_n()))
			{
				tst = 1;
			}
			else
			{
				tst = 0;
			}
			break;
		case 13: /* LE */
			if (this->alu.get_z() == 1 && (this->alu.get_v() != this->alu.get_n()))
			{
				tst = 1;
			}
			else
			{
				tst = 0;
			}
		case 14: /* always */
			tst = 1;
			break;
		default:
			this->report_error(" unsupported cond", "OP16_COND_BRANCH");
			break;
	}
	if (tst == 1)
	{
		next_p_addr = p_addr + offset + 4;
	}
	else
	{
		if (is_ins32)
		{
			next_p_addr = p_addr + 4;
		}
		else
		{
			next_p_addr = p_addr + 2;
		}
	}
	return next_p_addr;
}


/* return -1 if executed a breakpoint (BKPT) instruction */
int Cpu::step(void)
{
	int status = 0; /* returning 0 by default to inform gdb server that we didn't hit a breakpoint */

	/* fetch 1st part */
	unsigned int p_addr = this->regs[PC].read();
	uint16_t ins16 = this->ram.read16(p_addr);

	if (((ins16 & OP16_MASK) == OP16_VAL1) || ((ins16 & OP16_MASK) == OP16_VAL2) || ((ins16 & OP16_MASK) == OP16_VAL3))
	{ /* 32-bit instructions */
		uint32_t ins16_b = this->ram.read16(p_addr + 2);
		LOG_TRACE(">>>>>>>> p_addr = 0x%08x, ins16 = 0x%04x, ins16b = 0x%04x: ", p_addr, ins16, ins16_b);
		if ((ins16 & OP32_LDMIA_MASK) == OP32_LDMIA_VAL)
		{
			LOG_TRACE("OP32_LDMIA\n");
			/* LDMIA A6.7.40/T2 */
			unsigned int w = GET_BIT(ins16, 5);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int register_list = ins16_b;
			unsigned int d_addr = this->regs[rn].read();
			unsigned int p = GET_BIT(ins16_b, 15);
			for (unsigned int i = 1; i < 16; ++i)
			{
				if (GET_BIT(register_list, i) == 1)
				{
					this->regs[i].write(this->ram.read32(d_addr));
					d_addr += 4;
				}
			}
			if (w == 1)
			{
				this->regs[rn].write(d_addr);
			}
			if (p == 1)
			{
				p_addr = this->regs[PC].read();
			}
			else
			{
				p_addr += 4;
			}
		}
		else if ((ins16 & OP32_STMIA_MASK) == OP32_STMIA_VAL)
		{
			LOG_TRACE("OP32_STMIA\n");
			/* STMIA A6.7.117/T2 */
			unsigned int w = GET_BIT(ins16, 5);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int register_list = ins16_b;
			unsigned int d_addr = this->regs[rn].read();
			for (unsigned int i = 0; i < 15; i++)
			{
				if (GET_BIT(register_list, i) == 1)
				{
					this->ram.write32(d_addr, this->regs[i].read());
					d_addr += 4;
				}
			}
			if (w == 1)
			{
				this->regs[rn].write(d_addr);
			}
			p_addr += 4;
		}
		else if ((ins16 & OP32_STMDB_MASK) == OP32_STMDB_VAL)
		{
			LOG_TRACE("OP32_STMDB\n");
			/* STMDB A6.7.118/T1 */
			unsigned int w = GET_BIT(ins16, 5);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int register_list = ins16_b;
			unsigned int register_count = bit_count(register_list);
			unsigned int d_addr_base = this->regs[rn].read() - 4*register_count;
			unsigned int d_addr = d_addr_base;
			for (unsigned int i = 0; i < 15; ++i)
			{
				if (GET_BIT(register_list, i) == 1)
				{
					this->ram.write32(d_addr, this->regs[i].read());
					d_addr += 4;
				}
			}
			if (w == 1)
			{
				this->regs[rn].write(d_addr_base);
			}
			p_addr += 4;
		}
		else if ((ins16 & OP32_DATA_SHIFTED_REG_MASK) == OP32_DATA_SHIFTED_REG_VAL)
		{
			LOG_TRACE("OP32_DATA_SHIFTED_REG\n");
			/* data processing (shifted register) A5.3.11 */
			unsigned int op = GET_FIELD(ins16, 5, 4);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int s = GET_BIT(ins16, 4);
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int rm = GET_FIELD(ins16_b, 0, 4);
			unsigned int imm = (GET_FIELD(ins16_b, 12, 3) << 2) | GET_FIELD(ins16_b, 6, 2);
			unsigned int shift_op = GET_FIELD(ins16_b, 4, 2);
			uint32_t b = this->decode_imm_shift(shift_op, imm, 0, this->regs[rm].read());
			uint32_t y;
			switch (op)
			{
				case 0:
					if (rd == 15)
					{ /* TST */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.bw_and(this->regs[rn].read(), b, 1);
					}
					else
					{ /* AND A6.7.9/T2 */
						y = this->alu.bw_and(this->regs[rn].read(), b, s);
						this->regs[rd].write(y);
					}
					break;
				case 1: /* BIC A&.7.16/T2 */
					y = this->alu.bw_andn(this->regs[rn].read(), b, s);
					this->regs[rd].write(y);
					break;
				case 2:
					if (rn == 15)
					{ /* move reg and immediate shift */
						y = this->alu.bw_or(0, b, s);;
						this->regs[rd].write(y);
					}
					else
					{ /* ORR A6.7.91/T2 */
						y = this->alu.bw_or(this->regs[rn].read(), b, s);
						this->regs[rd].write(y);
					}
					break;
				case 3:
					if (rn == 15)
					{ /* MVN A6.7.85/T2 */
						y = this->alu.bw_orn(0, b, s);
						this->regs[rd].write(y);
					}
					else
					{ /* ORN A&.7.89/T1 */
						y = this->alu.bw_orn(this->regs[rn].read(), b, s);
						this->regs[rd].write(y);
					}
					break;
				case 4:
					if (rd == 15)
					{ /* TEQ A6.7.141/T1 */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.bw_xor(this->regs[rn].read(), b, s);
					}
					else
					{ /* EOR A6.7.35/T2 */
						y = this->alu.bw_xor(this->regs[rn].read(), b, s);
						this->regs[rd].write(y);
					}
					break;
				case 8:
					if (rd == 15)
					{ /* CMN A6.7.26/T2 */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.add(this->regs[rn].read(), b, s);
					}
					else
					{ /* ADD A6.7.4/T3 */
						y = this->alu.add(this->regs[rn].read(), b, s);
						this->regs[rd].write(y);
					}
					break;
				case 10: /* ADC A6.7.2/T2 */
					y = this->alu.addc(this->regs[rn].read(), b, s);
					this->regs[rd].write(y);
					break;
				case 11: /* SBC A6.7.109/T2 */
					y = this->alu.subc(this->regs[rn].read(), b, s);
					this->regs[rd].write(y);
					break;
				case 13:
					if (rd == 15)
					{ /* CMP A6.7.28/T3 */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.sub(this->regs[rn].read(), b, s);
					}
					else
					{ /* SUB A6.7.133/T2 */
						y = this->alu.sub(this->regs[rn].read(), b, s);
						this->regs[rd].write(y);
					}
					break;
				case 14: /* RSB A6.7.107/T1 */
					y = this->alu.sub(b, this->regs[rn].read(), s);
					this->regs[rd].write(y);
					break;
				default:
					this->report_error("incorrect operand", "OP32_DATA_SHIFTED_REG");
					break;
			}
			p_addr += 4;
		}
		else if (((ins16 & OP32_DATA_MOD_IMM_MASK) == OP32_DATA_MOD_IMM_VAL) && ((ins16_b & OP32_DATA_MOD_IMM_MASK_B) == OP32_DATA_MOD_IMM_VAL_B))
		{
			LOG_TRACE("OP32_DATA_MOD_IMM\n");
			/* data processing (modified immediate) A5.3.1 */
			unsigned int op = GET_FIELD(ins16, 5, 4);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int rd = GET_FIELD(ins16_b, 8, 4);
			unsigned int s = GET_BIT(ins16, 4);
			uint32_t a = this->regs[rn].read();
			uint32_t b = this->generate_modified_immediate(ins16, ins16_b);
			uint32_t y;
			switch (op)
			{
				case 0:
					if (rn == 15)
					{ /* TST */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.bw_and(a, b, 1);
					}
					else
					{ /* AND */
						y = this->alu.bw_and(a, b, s);
						this->regs[rd].write(y);
					}
					break;
				case 1: /* BIC */
					y = this->alu.bw_andn(a, b, s);
					this->regs[rd].write(y);
					break;
				case 2:
					if (rn == 15)
					{ /* MOV */
						y = alu.bw_or(0, b, s); /* mov */
						this->regs[rd].write(y);
					}
					else
					{ /* ORR */
						y = this->alu.bw_or(a, b, s);
						this->regs[rd].write(y);
					}
					break;
				case 3:
					if (rn == 15)
					{ /* MVN */
						y = this->alu.bw_orn(0, b, s);
						this->regs[rd].write(y);
					}
					else
					{ /* ORN */
						y = this->alu.bw_orn(a, b, s);
						this->regs[rd].write(y);
					}
					break;
				case 4:
					if (rn == 15)
					{ /* TEQ */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.bw_xor(a, b, 1);
					}
					else
					{ /* EOR */
						y = this->alu.bw_xor(a, b, s);
						this->regs[rd].write(y);
					}
					break;
				case 8: /* add/compare negative */
					if (rn == 15)
					{ /* CMN */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.add(a, b, 1);
					}
					else
					{ /* ADD */
						y = this->alu.add(a, b, s);
						this->regs[rd].write(y);
					}
					break;
				case 13: /* substract/compare */
					if (rn == 15)
					{ /* CMP */
						/* we are not interested in the result, only in the flags. s is supposed to be 1 */
						y = this->alu.sub(a, b, 1);
					}
					else
					{ /* SUB */
						y = this->alu.sub(a, b, s);
						this->regs[rd].write(y);
					}
					break;
				case 10: /* ADC */
					y = this->alu.addc(a, b, s);
					this->regs[rd].write(y);
					break;
				case 11: /* SBC */
					y = this->alu.subc(a, b, s);
					this->regs[rd].write(y);
					break;
				case 14: /* RSB */
					y = this->alu.sub(b, a, s);
					this->regs[rd].write(y);
					break;
				default:
					this->report_error("unsupported opcode", "OP32_DATA_MOD_IMM");
					break;
			}
			p_addr += 4;
		}
		else if (((ins16 & OP32_DATA_PLAIN_IMM_MASK) == OP32_DATA_PLAIN_IMM_VAL) && ((ins16_b & OP32_DATA_PLAIN_IMM_MASK_B) == OP32_DATA_PLAIN_IMM_VAL_B))
		{
			LOG_TRACE("OP32_DATA_PLAIN_IMM\n");
			/* data processing (plain binary immediate) A5.3.3 */
			unsigned int op = GET_FIELD(ins16, 4, 5);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			if (op == 0)
			{
				if (rn == 15)
				{ /* ADR */
					this->report_error("unsupported ADR(add)", "OP32_DATA_PLAIN_IMM");
				}
				else
				{ /* ADD (12-bit) */
					this->report_error("unsupported ADD", "OP32_DATA_PLAIN_IMM");
				}
			}
			else if (op == 4)
			{ /* MOV (16-bit) */
				this->report_error("unsupported MOV", "OP32_DATA_PLAIN_IMM");
			}
			else if (op == 10)
			{
				if (rn == 15)
				{ /* ADR */
					this->report_error("unsupported ADR(sub)", "OP32_DATA_PLAIN_IMM");
				}
				else
				{ /* SUB (12-bit) */
					this->report_error("unsupported SUB", "OP32_DATA_PLAIN_IMM");
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
				uint32_t val = (src & 0xffff) | (imm16 << 16);
				this->regs[rd].write(val);
			}
			else if (op == 20)
			{ /* SBFX */
				this->report_error("unsupported SBFX", "OP32_DATA_PLAIN_IMM");
			}
			else if (op == 22)
			{
				if (rn == 15)
				{ /* BFC */
					this->report_error("unsupported BFX", "OP32_DATA_PLAIN_IMM");
				}
				else
				{ /* BFI A6.7.114/T1 */
					unsigned int rd = GET_FIELD(ins16_b, 8, 4);
					unsigned int imm2 = GET_FIELD(ins16_b, 6, 2);
					unsigned int imm3 = GET_FIELD(ins16_b, 12, 3);
					unsigned int msbit = GET_FIELD(ins16_b, 0, 5);
					unsigned int lsbit = imm2 | (imm3 << 2);
					unsigned int width = msbit - lsbit + 1;
					uint32_t mask = 0xffffffffU >> (32 - width);
					uint32_t field = this->regs[rn].read() & mask;
					uint32_t val = this->regs[rd].read();
					val = val & ~(mask << lsbit);
					val = val | (field << lsbit);
					this->regs[rd].write(val);
				}
			}
			else if (op == 28)
			{ /* UBFX */
				this->report_error("unsupported UBFX", "OP32_DATA_PLAIN_IMM");
			}
			else
			{
				this->report_error("unsupported operand", "OP32_DATA_PLAIN_IMM");
			}
			p_addr += 4;
		}
		else if ((ins16 & OP32_DATA_REG_MASK) == OP32_DATA_REG_VAL)
		{
			LOG_TRACE("OP32_DATA_REG\n");
			/* data processing (register) A5.3.12 */
			unsigned int op1 = GET_FIELD(ins16, 4, 4);
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int op2 = GET_FIELD(ins16_b, 4, 4);
			if ((op1 == 0 || op1 == 1) && (op2 == 0))
			{ /* LSL A6.7.68/T2*/
				unsigned int rm = GET_FIELD(ins16_b, 0, 4);
				unsigned int rd = GET_FIELD(ins16_b, 8, 4);
				unsigned int s = GET_BIT(ins16, 4);
				uint32_t y = this->alu.lsl(this->regs[rn].read(), this->regs[rm].read(), s);
				this->regs[rd].write(y);
			}
			else if ((op1 == 0) && (GET_BIT(op2, 3) == 1))
			{ /* SXTH */
				this->report_error("instruction SXTH not supported", "OP32_DATA_REG");
			}
			else if ((op1 == 1) && (GET_BIT(op2, 3) == 1))
			{ /* UXTH */
				this->report_error("instruction UXTH not supported", "OP32_DATA_REG");
			}
			else if ((op1 == 2) || (op1 == 3))
			{ /* LSR A6.7.60/T2 */
				unsigned int rm = GET_FIELD(ins16_b, 0, 4);
				unsigned int rd = GET_FIELD(ins16_b, 8, 4);
				unsigned int s = GET_BIT(ins16, 4);
				uint32_t y = this->alu.lsr(this->regs[rn].read(), this->regs[rm].read(), s);
				this->regs[rd].write(y);
			}
			else if ((op1 == 4 || op1 == 5) && (op2 == 0))
			{ /* ASR A6.7.11/T2 */
				unsigned int rm = GET_FIELD(ins16_b, 0, 4);
				unsigned int rd = GET_FIELD(ins16_b, 8, 4);
				unsigned int s = GET_BIT(ins16, 4);
				uint32_t y = this->alu.asr(this->regs[rn].read(), this->regs[rm].read(), s);
				this->regs[rd].write(y);
			}
			else if ((op1 == 4) && (GET_BIT(op2, 3) == 1))
			{ /* SXTB */
				this->report_error("instruction SXTB not supported", "OP32_DATA_REG");
			}
			else if ((op1 == 5) && (GET_BIT(op2, 3) == 1))
			{ /* UXTB */
				this->report_error("instruction UXTB not supported", "OP32_DATA_REG");
			}
			else if ((op1 == 6 || op1 == 7) && (op2 == 0))
			{ /* ROR A&.7.104/T2 */
				unsigned int rm = GET_FIELD(ins16_b, 0, 4);
				unsigned int rd = GET_FIELD(ins16_b, 8, 4);
				unsigned int s = GET_BIT(ins16, 4);
				uint32_t y = this->alu.ror(this->regs[rn].read(), this->regs[rm].read(), s);
				this->regs[rd].write(y);
			}
			else if ((op1 == 8 || op1 == 9 || op1 == 10 || op1 == 11) && (op2 == 8 || op2 == 9 || op2 == 10 || op2 == 11))
			{ /* MISC A5-29 */
				this->report_error("instruction MISC not supported", "OP32_DATA_REG");
			}
			else
			{
				this->report_error("unsupported instruction", "OP32_DATA_REG");
			}
			p_addr += 4;
		}
		else if (((ins16 & OP32_BRANCH_MISC_MASK) == OP32_BRANCH_MISC_VAL) && ((ins16_b & OP32_BRANCH_MISC_MASK_B) == OP32_BRANCH_MISC_VAL_B))
		{
			LOG_TRACE("OP32_BRANCH_MISC\n");
			/* branch and miscellanous control A5.3.4 */
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
					offset |= 0xffe00000;
				}
				p_addr = this->conditional_branch(cond, offset, true, p_addr);
			}
			else
			{
				report_error("unsupported instruction", "OP32_BRANCH_MISC_MASK");
			}
		}
		else if (((ins16 & OP32_STR_IMM_MASK) == OP32_STR_IMM_VAL) && ((ins16_b & OP32_STR_IMM_MASK_B) == OP32_STR_IMM_VAL_B))
		{
			LOG_TRACE("OP32_STR_IMM\n");
			/* STR(immediate) A6.7.119/T4 */
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int rt = GET_FIELD(ins16_b, 12, 4);
			unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
			unsigned int p = GET_BIT(ins16_b, 10);
			unsigned int u = GET_BIT(ins16_b, 9);
			unsigned int w = GET_BIT(ins16_b, 8);
			unsigned int d_addr = this->regs[rn].read();
			unsigned int offset_addr;
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
			this->ram.write32(d_addr, this->regs[rt].read());
			if (w == 1)
			{
				this->regs[rn].write(offset_addr);
			}
			p_addr += 4;
		}
		else if (((ins16 & OP32_LDR_IMM_MASK) == OP32_LDR_IMM_VAL) && ((ins16_b & OP32_LDR_IMM_MASK_B) == OP32_LDR_IMM_VAL_B))
		{
			LOG_TRACE("OP32_LDR_IMM\n");
			/* LDR(immediate) A6.7.42/T4 */
			unsigned int rn = GET_FIELD(ins16, 0, 4);
			unsigned int rt = GET_FIELD(ins16_b, 12, 4);
			unsigned int imm8 = GET_FIELD(ins16_b, 0, 8);
			unsigned int p = GET_BIT(ins16_b, 10);
			unsigned int u = GET_BIT(ins16_b, 9);
			unsigned int w = GET_BIT(ins16_b, 8);
			unsigned int d_addr = this->regs[rn].read();
			unsigned int offset_addr;
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
			if (w == 1)
			{
				this->regs[rn].write(offset_addr);
			}
			this->regs[rt].write(data);
			p_addr += 4;
		}
		else
		{
			this->report_error("unsupported 32-bit instruction", "Cpu::step()");
		}
	}
	else
	{ /* 16-bit instructions */
		LOG_TRACE(">>>>>>>> p_addr = 0x%08x, ins16 = 0x%04x,                : ", p_addr, ins16);
		if ((ins16 & OP16_PUSHM_MASK) == OP16_PUSHM_VAL)
		{
			LOG_TRACE("OP16_PUSHM\n");
			/* push multiple registers A6.7.98 */
			unsigned int m = GET_BIT(ins16, 8);
			uint8_t register_list = GET_FIELD(ins16, 0, 8);
			unsigned int d_addr_base = this->regs[SP].read() - 4*bit_count(register_list);
			if (m == 1)
			{
				d_addr_base -= 4;
			}
			unsigned int d_addr = d_addr_base;
			for (unsigned int i = 0; i < 8; i++)
			{
				if (GET_BIT(register_list, i) == 1)
				{
					this->ram.write32(d_addr, this->regs[i].read());
					d_addr += 4;
				}
			}
			if (m == 1)
			{
				this->ram.write32(d_addr, this->regs[LR].read());
				d_addr += 4;
			}
			this->regs[SP].write(d_addr_base);
			p_addr += 2;
		}
		else if ((ins16 & OP16_POPM_MASK) == OP16_POPM_VAL)
		{
			LOG_TRACE("OP16_POPM\n");
			/* pop multiple registers A6.7.97/T1 */
			unsigned int p = GET_BIT(ins16, 8);
			unsigned int register_list = GET_FIELD(ins16, 0, 8);
			unsigned int d_addr = this->regs[SP].read();
			for (unsigned int i = 0; i < 8; ++i)
			{
				if (GET_BIT(register_list, i) == 1)
				{
					this->regs[i].write(this->ram.read32(d_addr));
					d_addr += 4;
				}
			}
			this->regs[SP].write(d_addr);
			if (p == 1)
			{
				p_addr = this->ram.read32(d_addr);
				d_addr += 4;
			}
			else
			{
				p_addr += 2;
			}
			this->regs[SP].write(d_addr);
		}
		else if ((ins16 & OP16_SUB_IMM_SP_MASK) == OP16_SUB_IMM_SP_VAL)
		{
			LOG_TRACE("OP16_SUB_IMM_SP\n");
			/* SUB SP IMM A6.7.134 */
			uint32_t imm7 = GET_FIELD(ins16, 0, 7) << 2;
			this->regs[SP].write(this->regs[SP].read() - imm7);
			p_addr += 2;
		}
		else if ((ins16 & OP16_ADD_IMM_SP_MASK) == OP16_ADD_IMM_SP_VAL)
		{
			LOG_TRACE("OP16_ADD_IMM_SP\n");
			/* ADD SP IMM A6.7.5/T2 */
			uint32_t imm7 = GET_FIELD(ins16, 0, 7) << 2;
			this->regs[SP].write(this->regs[SP].read() + imm7);
			p_addr += 2;
		}
		else if ((ins16 & OP16_ST_REG_SP_REL_MASK) == OP16_ST_REG_SP_REL_VAL)
		{
			LOG_TRACE("OP16_ST_REG_SP_REL\n");
			/* store reg SP relative A6.7.119/T2 */
			uint32_t imm8 = GET_FIELD(ins16, 0, 8) << 2;
			unsigned int rt = GET_FIELD(ins16, 8, 3);
			unsigned int d_addr = this->regs[SP].read() + imm8;
			this->ram.write32(d_addr, this->regs[rt].read());
			p_addr += 2;
		}
		else if ((ins16 & OP16_LD_REG_SP_REL_MASK) == OP16_LD_REG_SP_REL_VAL)
		{
			LOG_TRACE("OP16_LD_REG_SP_REL\n");
			/* load reg SP relative A6.7.42/T2 */
			uint32_t imm8 = GET_FIELD(ins16, 0, 8) << 2;
			unsigned int rt = GET_FIELD(ins16, 8, 3);
			unsigned int d_addr = this->regs[SP].read() + imm8;
			this->regs[rt].write(this->ram.read32(d_addr));
			p_addr += 2;
		}
		else if ((ins16 & OP16_SHIFT_IMM_ADD_SUB_MOV_CMP_MASK) == OP16_SHIFT_IMM_ADD_SUB_MOV_CMP_VAL)
		{
			LOG_TRACE("OP16_SHIFT_IMM_ADD_SUB_MOV_CMP\n");
			unsigned int op = GET_FIELD(ins16, 9, 5);
			unsigned int rn;
			unsigned int rm;
			unsigned int rd;
			uint32_t imm;
			uint32_t y;
			switch (op)
			{
				case 0:
				case 1:
				case 2:
				case 3: /* LSL A6.7.67/T1 */
					rm = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					imm = GET_FIELD(ins16, 6, 5);
					/* TODO: do not update flag if inside IT block */
					y = this->decode_imm_shift(OP_LSL, imm, 1, this->regs[rm].read());
					this->regs[rd].write(y);
					break;
				case 4:
				case 5:
				case 6:
				case 7: /* LSR A6.7.69/T1 */
					rm = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					imm = GET_FIELD(ins16, 6, 5);
					/* TODO: do not update flag if inside IT block */
					y = this->decode_imm_shift(OP_LSR, imm, 1, this->regs[rm].read());
					this->regs[rd].write(y);
					break;
				case 8:
				case 9:
				case 10:
				case 11: /* ASR */
					rm = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					imm = GET_FIELD(ins16, 6, 5);
					/* TODO: do not update flag if inside IT block */
					y = this->decode_imm_shift(OP_ASR, imm, 1, this->regs[rm].read());
					this->regs[rd].write(y);
					break;
				case 12: /* ADD REG A6.7.4/T1 */
					rm = GET_FIELD(ins16, 6, 3);
					rn = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.add(this->regs[rn].read(), this->regs[rm].read(), 1);
					this->regs[rd].write(y);
					break;
				case 13: /* SUB REG A6.7.133/T1 */
					rm = GET_FIELD(ins16, 6, 3);
					rn = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.sub(this->regs[rn].read(), this->regs[rm].read(), 1);
					this->regs[rd].write(y);
					break;
				case 14: /* ADD IMM3 A6.7.3/T1 */
					rn = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					imm = GET_FIELD(ins16, 6, 3);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.add(this->regs[rn].read(), imm, 1);
					this->regs[rd].write(y);
					break;
				case 15: /* SUB IMM3 A6.7.132/T1 */
					rn = GET_FIELD(ins16, 3, 3);
					rd = GET_FIELD(ins16, 0, 3);
					imm = GET_FIELD(ins16, 6, 3);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.sub(this->regs[rn].read(), imm, 1);
					this->regs[rd].write(y);
					break;
				case 16:
				case 17:
				case 18:
				case 19:/* MOV IMM A6.7.75/T1 */
					rd = GET_FIELD(ins16, 8, 3);
					imm = GET_FIELD(ins16, 0, 8);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.bw_or(0, imm, 1);
					this->regs[rd].write(y);
					break;
				case 20:
				case 21:
				case 22:
				case 23: /* CMP IMM A6.7.27/T1 */
					rn = GET_FIELD(ins16, 8, 3);
					imm = GET_FIELD(ins16, 0, 8);
					y = this->alu.sub(this->regs[rn].read(), imm, 1);
					break;
				case 24:
				case 25:
				case 26:
				case 27: /* ADD IMM8 A6.7.3/T2 */
					rd = GET_FIELD(ins16, 8, 3);
					imm = GET_FIELD(ins16, 0, 8);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.add(this->regs[rd].read(), imm, 1);
					this->regs[rd].write(y);
					break;
				case 28:
				case 29:
				case 30:
				case 31: /* SUB IMM8 A6.7.132/T2 */
					rd = GET_FIELD(ins16, 8, 3);
					imm = GET_FIELD(ins16, 0, 8);
					/* TODO: do not update flag if inside IT block */
					y = this->alu.sub(this->regs[rd].read(), imm, 1);
					this->regs[rd].write(y);
					break;
					break;
				default:
					this->report_error("undefined opcode", "OP16_SHIFT_IMM_ADD_SUB_MOV_CMP");
					break;
			}
			p_addr += 2;
		}
		else if ((ins16 & OP16_SPECIAL_DATA_BRANCH_MASK) == OP16_SPECIAL_DATA_BRANCH_VAL)
		{
			LOG_TRACE("OP16_SPECIAL_DATA_BRANCH\n"); /* A5.2.3 */
			unsigned int op = GET_FIELD(ins16, 6, 4);
			unsigned int rd;
			unsigned int rm;
			uint32_t y;
			switch (op)
			{
				case 0:
				case 1:
				case 2:
				case 3: /* ADD REG A6.7.4/T2 */
					rd = GET_FIELD(ins16, 0, 2) | (GET_BIT(ins16, 7) << 3);
					rm = GET_FIELD(ins16, 3, 4);
					y = this->alu.add(this->regs[rd].read(), this->regs[rm].read(), 0);
					this->regs[rd].write(y);
					p_addr += 2;
					break;
				case 5:
				case 6:
				case 7: /* CMP REG A6.7.28/T2 */
					rd = GET_FIELD(ins16, 0, 2) | (GET_BIT(ins16, 7) << 3);
					rm = GET_FIELD(ins16, 3, 4);
					y = this->alu.add(this->regs[rd].read(), this->regs[rm].read(), 1);
					p_addr += 2;
					break;
				case 8:
				case 9:
				case 10:
				case 11: /* MOV REG A6.7.76/T1 */
					rd = GET_FIELD(ins16, 0, 2) | (GET_BIT(ins16, 7) << 3);
					rm = GET_FIELD(ins16, 3, 4);
					y = this->alu.add(0, this->regs[rm].read(), 0);
					this->regs[rd].write(y);
					p_addr += 2;
					break;
				case 12:
				case 13: /* BRANCH AND EXCHANGE A6.7.20/T1 */
					rm = GET_FIELD(ins16, 3, 4);
					p_addr = this->regs[rm].read();
					break;
				case 14:
				case 15: /* BRANCH WITH LINK AND EXCHANGE */
					rm = GET_FIELD(ins16, 3, 4);
					this->regs[LR].write(p_addr + 2);
					p_addr = this->regs[rm].read();
					break;
				default: /* includes case '4' */
					this->report_error("undefined opcode", "OP16_SPECIAL_DATA_BRANCH");
					break;
			}
		}
		else if ((ins16 & OP16_COND_BRANCH_MASK) == OP16_COND_BRANCH_VAL)
		{
			LOG_TRACE("OP16_COND_BRANCH\n");
			/* conditional branch A6.7.12/T1 */
			unsigned int cond = GET_FIELD(ins16, 8, 4);
			int8_t imm8 = (int8_t)GET_FIELD(ins16, 0, 8) & 0xff;
			int32_t offset = imm8*2;
			p_addr = this->conditional_branch(cond, offset, false, p_addr);
		}
		else if ((ins16 & OP16_LDMIA_MASK) == OP16_LDMIA_VAL)
		{
			LOG_TRACE("OP16_LDMIA\n");
			/* load multiple register A6.7.40/T1 */
			unsigned int rn = GET_FIELD(ins16, 8, 3);
			unsigned int register_list = GET_FIELD(ins16, 0, 8);
			unsigned int d_addr = this->regs[rn].read();
			for (unsigned int i = 0; i < 8; i++)
			{
				if (GET_BIT(register_list, i) == 1)
				{
					this->regs[i].write(this->ram.read32(d_addr));
					d_addr += 4;
				}
			}
			if (GET_BIT(register_list, rn) == 0)
			{
				this->regs[rn].write(d_addr);
			}
			p_addr += 2;
		}
		else if ((ins16 & OP16_NOP_MASK) == OP16_NOP_VAL)
		{
			LOG_TRACE("OP16_NOP\n");
			/* NOP A6.7.87/T1 */
			p_addr += 2;
		}
		else if ((ins16 & OP16_BREAKPOINT_MASK) == OP16_BREAKPOINT_VAL)
		{
			LOG_TRACE("OP16_BREAKPOINT\n");
			/* BKPT A6.7.17 */
			status = -1; /* returning -1 to inform gdb server that we hit a breakpoint */
		}
		else
		{
			this->report_error("unsupported 16-bit instruction", "Cpu::step()");
		}
	}
	this->regs[PC].write(p_addr);
	this->instruction_count++;
	return status;
}


unsigned long int Cpu::run(unsigned int from, unsigned int until, unsigned long int limit)
{
	/* prepare file for trace index */
	FILE *trace_index_file;
	if (this->generate_trace_index == true && this->trace_index_done == false)
	{
		trace_index_file = fopen(this->trace_index_filename.c_str(), "w");
	}

	/* prepare to jump to code */
	this->regs[LR].write(until);
	this->regs[PC].write(from);

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
			p_addr = this->regs[PC].read();
			if (p_addr == until)
			{
				break;
			}
			if (this->step() == -1)
			{
				/* instruction was a breakpoint */
				fprintf(stderr, "---- Hit breakpoint at address 0x%08x\n", p_addr);
				this->dump_regs();
				/* a breakpoint instruction does not increment the PC, but here we are
				   using the "BKPT" instruction in a different way for debugging */
				this->regs[PC].write(p_addr + 2);
			}
			#ifdef DEBUG_TRACE
			this->dump_regs();
			this->dump_memory(0x0400, 16);
			this->dump_memory(0x0500, 16);
			this->dump_memory(8*1024 - 64, 64);
			printf("0x%08x\n", this->regs[PC].read());
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


void Cpu::copy_array_to_target(uint32_t *buffer, unsigned int len, unsigned int target_addr)
{
	for (unsigned int i = 0; i < len; ++i)
	{
		this->ram.write32(target_addr + 4*i, buffer[i]);
	}
}


void Cpu::copy_array_from_target(uint32_t *buffer, unsigned int len, unsigned int target_addr)
{
	for (unsigned int i = 0; i < len; ++i)
	{
		buffer[i] = this->ram.read32(target_addr + 4*i);
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
