/******************************************************************************
 *
 * Opcodes
 *
 ******************************************************************************/

#ifndef __OPCODES_H__
#define __OPCODES_H__

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

#define OP16_SHIFT_IMM_ADD_SUB_MOV_CMP_MASK 0xc000
#define OP16_SHIFT_IMM_ADD_SUB_MOV_CMP_VAL  0x0000

#define OP16_SPECIAL_DATA_BRANCH_MASK 0xfc00
#define OP16_SPECIAL_DATA_BRANCH_VAL  0x4400

#define OP16_NOP_MASK 0xffff
#define OP16_NOP_VAL  0xbf00

/* OP32 */
#define OP32_LDMIA_MASK 0xffd0
#define OP32_LDMIA_VAL  0xe890

#define OP32_STMIA_MASK 0xffd0
#define OP32_STMIA_VAL  0xe880

#define OP32_STMDB_MASK 0xffc0
#define OP32_STMDB_VAL  0xe900

#define OP32_DATA_SHIFTED_REG_MASK 0xfe00
#define OP32_DATA_SHIFTED_REG_VAL  0xea00

#define OP32_DATA_MOD_IMM_MASK 0xfa00
#define OP32_DATA_MOD_IMM_VAL  0xf000
#define OP32_DATA_MOD_IMM_MASK_B 0x8000
#define OP32_DATA_MOD_IMM_VAL_B  0x0000

#define OP32_BRANCH_MISC_MASK 0xf800
#define OP32_BRANCH_MISC_VAL  0xf000
#define OP32_BRANCH_MISC_MASK_B 0x8000
#define OP32_BRANCH_MISC_VAL_B  0x8000

#define OP32_DATA_REG_MASK 0xff00
#define OP32_DATA_REG_VAL  0xfa00

#define OP32_STR_IMM_MASK 0xfff0
#define OP32_STR_IMM_VAL  0xf840
#define OP32_STR_IMM_MASK_B 0x0800
#define OP32_STR_IMM_VAL_B  0x0800

#define OP32_LDR_IMM_MASK 0xfff0
#define OP32_LDR_IMM_VAL  0xf850
#define OP32_LDR_IMM_MASK_B 0x0800
#define OP32_LDR_IMM_VAL_B  0x0800

#define OP32_DATA_PLAIN_IMM_MASK 0xfa00
#define OP32_DATA_PLAIN_IMM_VAL  0xf200
#define OP32_DATA_PLAIN_IMM_MASK_B 0x8000
#define OP32_DATA_PLAIN_IMM_VAL_B  0x0000

/* shifts */
#define OP_LSL 0
#define OP_LSR 1
#define OP_ASR 2
#define OP_ROR 3

#endif
