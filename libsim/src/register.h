/******************************************************************************
 *
 * Registers
 *
 ******************************************************************************/

#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <cstdint>
#include "tracer.h"

class Register
{
	private:
		uint32_t value;
		Tracer *tracer_ptr;
		

	public:
		Register();
		~Register();
		void write(uint32_t val);
		uint32_t read(void);
		void bind_tracer(Tracer *ptr);
};

#endif
