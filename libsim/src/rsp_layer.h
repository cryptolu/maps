/******************************************************************************
 *
 * GDB RSP layer
 *
 ******************************************************************************/

#ifndef __RSP_LAYER_H__
#define __RSP_LAYER_H__

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>

#include "cpu.h"
#include "presentation_layer.h"

class Rsp_layer
{
	private:
		Presentation_layer presentation;

		void byte_to_str(char *str, uint8_t b);
		void push_uint8(std::string &str, uint8_t x);
		void push_uint32(std::string &str, uint32_t x);
		void report_error(const char *msg, const char *location) __attribute__ ((noreturn));

	public:
		Rsp_layer();
		~Rsp_layer();
		void run(Cpu *cpu);
};


#endif
