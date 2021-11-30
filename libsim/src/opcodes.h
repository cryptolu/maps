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
 * Opcodes
 *
 ******************************************************************************/

#ifndef __OPCODES_H__
#define __OPCODES_H__

/* utils */
#define TEST_INS16(category) ((ins16 & category##_MASK) == category##_VAL)
#define TEST_INS32(category) ((ins32 & category##_MASK) == category##_VAL)

/* Differentiate between 16-bit and 32-bit instructions */
#define OP16_MASK 0xf800
#define OP16_VAL1 0xe800
#define OP16_VAL2 0xf000
#define OP16_VAL3 0xf800

/* OP16 */
#define OP16_PUSHM_MASK 0xfe00
#define OP16_PUSHM_VAL  0xb400

#define OP16_POPM_MASK 0xfe00
#define OP16_POPM_VAL  0xbc00

#define OP16_LDMIA_MASK 0xf800
#define OP16_LDMIA_VAL  0xc800

#define OP16_LD_IMM_MASK 0xf800
#define OP16_LD_IMM_VAL  0x6800

#define OP16_LD_LITERAL_POOL_MASK 0xf800
#define OP16_LD_LITERAL_POOL_VAL  0x4800

#define OP16_LDRB_IMM_MASK 0xf800
#define OP16_LDRB_IMM_VAL  0x7800

#define OP16_LDR_REG_MASK 0xfe00
#define OP16_LDR_REG_VAL  0x5800

#define OP16_LDRB_REG_MASK 0xfe00
#define OP16_LDRB_REG_VAL  0x5c00

#define OP16_STRB_IMM_MASK 0xf800
#define OP16_STRB_IMM_VAL  0x7000

#define OP16_STRB_REG_MASK 0xfe00
#define OP16_STRB_REG_VAL  0x5400

#define OP16_STR_REG_MASK 0xfe00
#define OP16_STR_REG_VAL  0x5000

#define OP16_STR_IMM_MASK 0xf800
#define OP16_STR_IMM_VAL  0x6000

#define OP16_SUB_IMM_SP_MASK 0xff80
#define OP16_SUB_IMM_SP_VAL  0xb080

#define OP16_ADD_IMM_SP_MASK 0xff80
#define OP16_ADD_IMM_SP_VAL  0xb000

#define OP16_ST_REG_SP_REL_MASK 0xf800
#define OP16_ST_REG_SP_REL_VAL  0x9000

#define OP16_LD_REG_SP_REL_MASK 0xf800
#define OP16_LD_REG_SP_REL_VAL  0x9800

#define OP16_COND_BRANCH_MASK 0xf000
#define OP16_COND_BRANCH_VAL  0xd000

#define OP16_UNCOND_BRANCH_MASK 0xf800
#define OP16_UNCOND_BRANCH_VAL  0xe000

#define OP16_SHIFT_IMM_ADD_SUB_MOV_CMP_MASK 0xc000
#define OP16_SHIFT_IMM_ADD_SUB_MOV_CMP_VAL  0x0000

#define OP16_SPECIAL_DATA_BRANCH_MASK 0xfc00
#define OP16_SPECIAL_DATA_BRANCH_VAL  0x4400

#define OP16_REV_MASK 0xffc0
#define OP16_REV_VAL  0xba00

#define OP16_REV16_MASK 0xffc0
#define OP16_REV16_VAL  0xba40

#define OP16_REVSH_MASK 0xffc0
#define OP16_REVSH_VAL  0xbac0

#define OP16_UXTB_MASK 0xffc0
#define OP16_UXTB_VAL  0xb2c0

#define OP16_UXTH_MASK 0xffc0
#define OP16_UXTH_VAL  0xb280

#define OP16_SXTB_MASK 0xffc0
#define OP16_SXTB_VAL  0xb240

#define OP16_SXTH_MASK 0xffc0
#define OP16_SXTH_VAL  0xb200

#define OP16_BREAKPOINT_MASK 0xff00
#define OP16_BREAKPOINT_VAL  0xbe00

#define OP16_NOP_MASK 0xffff
#define OP16_NOP_VAL  0xbf00

/* OP32 */
#define OP32_LDMIA_MASK 0xffd00000U
#define OP32_LDMIA_VAL  0xe8900000U

#define OP32_STMIA_MASK 0xffd00000U
#define OP32_STMIA_VAL  0xe8800000U

#define OP32_STMDB_MASK 0xffd00000U
#define OP32_STMDB_VAL  0xe9000000U

#define OP32_LDMDB_MASK 0xffd00000U
#define OP32_LDMDB_VAL  0xe9100000U

#define OP32_DATA_SHIFTED_REG_MASK 0xfe000000U
#define OP32_DATA_SHIFTED_REG_VAL  0xea000000U

#define OP32_DATA_MOD_IMM_MASK 0xfa008000U
#define OP32_DATA_MOD_IMM_VAL  0xf0000000U

#define OP32_BRANCH_MISC_MASK 0xf8008000U
#define OP32_BRANCH_MISC_VAL  0xf0008000U

#define OP32_DATA_REG_MASK 0xff000000U
#define OP32_DATA_REG_VAL  0xfa000000U

#define OP32_STR_IMM_MASK 0xfff00800U
#define OP32_STR_IMM_VAL  0xf8400800U

#define OP32_LDR_IMM_MASK 0xfff00800U
#define OP32_LDR_IMM_VAL  0xf8500800U

#define OP32_LDRB_IMM_MASK 0xfff00000U
#define OP32_LDRB_IMM_VAL  0xf8900000U

#define OP32_LDRB_IMM_ALT_MASK 0xfff00800U
#define OP32_LDRB_IMM_ALT_VAL  0xf8100800U

#define OP32_LDRB_REG_MASK 0xfff00fc0U
#define OP32_LDRB_REG_VAL  0xf8100000U

#define OP32_STRB_IMM_MASK 0xfff00000U
#define OP32_STRB_IMM_VAL  0xf8800000U

#define OP32_LDRB_LITERAL_MASK 0xff7f0000U
#define OP32_LDRB_LITERAL_VAL  0xf81f0000U

#define OP32_STRB_IMM_ALT_MASK 0xfff00800U
#define OP32_STRB_IMM_ALT_VAL  0xf8000800U

#define OP32_STRB_REG_MASK 0xfff00fc0U
#define OP32_STRB_REG_VAL  0xf8000000U

#define OP32_DATA_PLAIN_IMM_MASK 0xfa008000U
#define OP32_DATA_PLAIN_IMM_VAL  0xf2000000U

#define OP32_LD_LITERAL_POOL_MASK 0xff7f0000U
#define OP32_LD_LITERAL_POOL_VAL  0xf85f0000U

#define OP32_LDRD_IMM_MASK 0xfe500000U
#define OP32_LDRD_IMM_VAL  0xe8500000U

#endif
