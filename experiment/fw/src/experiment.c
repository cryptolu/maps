#include <stdint.h>

#define CR "\n\t"

void experiment(uint32_t *block)
{
	asm volatile (
		"ldr pc,=_start" CR
		".space 0x1ec" CR
		"_start:" CR
		"ldm %[block], {r2-r3}" CR
		"mov r4, 0x55" CR
		"mov r5, 0xaa" CR
		"orr r4, r4, r2, ror 2" CR
		"and r5, r5, r3, ror 3" CR
		"stm %[block], {r4-r5}" CR
		:
		: [block] "r" (block)
		: "r2", "r3", "r4", "r5"
	);
}
