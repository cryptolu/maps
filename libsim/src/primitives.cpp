/******************************************************************************
 *
 * Primitives
 *
 ******************************************************************************/

#include <cstdint>

#include "primitives.h"


void lsl_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n)
{
	*c = (a >> (31 - n)) & 1;
	*y = (a << n);
}


void lsl(uint32_t *y, uint32_t a, unsigned int n)
{
	*y = (a << n);
}


void lsr_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n)
{
	*c = (a >> (n - 1)) & 1;
	*y = (a >> n);
}


void lsr(uint32_t *y, uint32_t a, unsigned int n)
{
	*y = (a >> n);
}


void asr_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n)
{
	if (n == 0)
	{
		*y = a;
		*c = 0;
	}
	else
	{
		*c = (a >> (n - 1)) & 1;
		unsigned int sign = (a >> 31) & 1;
		uint32_t mask;
		if (sign == 1)
		{
			mask = 0xffffffff;
		}
		else
		{
			mask = 0x00000000;
		}
		*y = (a >> n) | (mask << (32 - n));
	}
}


void asr(uint32_t *y, uint32_t a, unsigned int n)
{
	unsigned int c;
	asr_c(y, &c, a, n);
}


void ror_c(uint32_t *y, unsigned int *c, uint32_t a, unsigned int n)
{
	*y = (a >> n) | (a << (32 - n));
	*c = (*y >> 31) & 1;
}


void ror(uint32_t *y, uint32_t a, unsigned int n)
{
	*y = (a >> n) | (a << (32 - n));
}


void rrx_c(uint32_t *y, unsigned int *c_out, uint32_t a, unsigned int c_in)
{
	*y = (a >> 1) | (c_in << 31);
	*c_out = a & 1;
}


void rrx(uint32_t *y, uint32_t a, unsigned int c_in)
{
	*y = (a >> 1) | (c_in << 31);
}


void add_c(uint32_t *y, unsigned int *c_out, unsigned int *v_out, uint32_t a, uint32_t b, unsigned int c_in)
{
	uint64_t unsigned_sum = a + b + c_in;
	int64_t signed_sum = (int64_t)a + (int64_t)b + c_in;
	*y = unsigned_sum & 0xffffffff;
	if (((uint64_t)*y) == unsigned_sum)
	{
		*c_out = 0;
	}
	else
	{
		*c_out = 1;
	}
	if (((int64_t)*y) == signed_sum)
	{
		*v_out = 0;
	}
	else
	{
		*v_out = 1;
	}
}


void bw_and(uint32_t *y, uint32_t a, uint32_t b)
{
	*y = a & b;
}


void bw_orr(uint32_t *y, uint32_t a, uint32_t b)
{
	*y = a | b;
}


void bw_eor(uint32_t *y, uint32_t a, uint32_t b)
{
	*y = a ^ b;
}


void decode_imm_shift(SRType *srtype, unsigned int *n, unsigned int type, unsigned int imm5)
{
	switch (type)
	{
		case 0:
			*srtype = SRType_LSL;
			*n = imm5;
			break;
		case 1:
			*srtype = SRType_LSR;
			*n = imm5 == 0 ? 32 : imm5;
			break;
		case 2:
			*srtype = SRType_ASR;
			*n = imm5 == 0 ? 32 : imm5;
			break;
		case 3:
			if (imm5 == 0)
			{
				*srtype = SRType_RRX;
				*n = 1;
			}
			else
			{
				*srtype = SRType_ROR;
				*n = imm5;
			}
			break;
	}
}


void decode_reg_shift(SRType *srtype, unsigned int type)
{
	switch (type)
	{
		case 0:
			*srtype = SRType_LSL;
			break;
		case 1:
			*srtype = SRType_LSR;
			break;
		case 2:
			*srtype = SRType_ASR;
			break;
		case 3:
			*srtype = SRType_ROR;
			break;
	}
}


void shift_c(uint32_t *y, unsigned int *c_out,  uint32_t a, SRType srtype, unsigned int n, unsigned int c_in)
{
	switch (srtype)
	{
		case SRType_LSL:
			lsl_c(y, c_out, a, n);
			break;
		case SRType_LSR:
			lsr_c(y, c_out, a, n);
			break;
		case SRType_ASR:
			asr_c(y, c_out, a, n);
			break;
		case SRType_ROR:
			ror_c(y, c_out, a, n);
			break;
		case SRType_RRX:
			rrx_c(y, c_out, a, c_in);
			break;
	}
}


void shift(uint32_t *y, uint32_t a, SRType srtype, unsigned int n, unsigned int c_in)
{
	unsigned int c_out;
	shift_c(y, &c_out, a, srtype, n, c_in);
}


void thumb_expand_imm_c(uint32_t *y, uint32_t *c_out, unsigned int imm12, unsigned int c_in)
{
	uint32_t imm12_7_0 = imm12 & 0xff;
	if ((imm12 >> 10) == 0)
	{
		switch ((imm12 >> 8) & 3)
		{
			case 0:
				*y = imm12_7_0;
				break;
			case 1:
				*y = (imm12_7_0 << 16) | imm12_7_0;
				break;
			case 2:
				*y = (imm12_7_0 << 24) | (imm12_7_0 << 8);
				break;
			case 3:
				*y = (imm12_7_0 << 24) | (imm12_7_0 << 16) | (imm12_7_0 << 8) | imm12_7_0;
				break;
		}
		*c_out = c_in;
	}
	else
	{
		uint32_t unrotated_value = 0xffffffffU | (imm12_7_0 & 0x0000007fU);
		ror_c(y, c_out, unrotated_value, (imm12 >> 7) & 0x1f);
	}
}
