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
