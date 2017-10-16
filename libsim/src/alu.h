/******************************************************************************
 *
 * ALU
 *
 ******************************************************************************/

#ifndef __ALU_H__
#define __ALU_H__

#include <cstdint>

class Alu
{
	private:
		unsigned int n;
		unsigned int z;
		unsigned int c;
		unsigned int v;
		unsigned int q;

		void compute_z(uint32_t y);
		void compute_n(uint32_t y);

	public:
		Alu();
		~Alu();
		unsigned int get_n(void);
		unsigned int get_z(void);
		unsigned int get_c(void);
		unsigned int get_v(void);
		unsigned int get_q(void);
		void set_c(unsigned int value);
		uint32_t bw_xor(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_and(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_or(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_orn(uint32_t a, uint32_t b, unsigned int s);
		uint32_t bw_andn(uint32_t a, uint32_t b, unsigned int s);
		uint32_t add(uint32_t a, uint32_t b, unsigned int s);
		uint32_t addc(uint32_t a, uint32_t b, unsigned int s);
		uint32_t sub(uint32_t a, uint32_t b, unsigned int s);
		uint32_t subc(uint32_t a, uint32_t b, unsigned int s);
		uint32_t lsl(uint32_t a, unsigned int n, unsigned int s);
		uint32_t lsr(uint32_t a, unsigned int n, unsigned int s);
		uint32_t asr(uint32_t a, unsigned int n, unsigned int s);
		uint32_t ror(uint32_t a, unsigned int n, unsigned int s);
};

#endif

