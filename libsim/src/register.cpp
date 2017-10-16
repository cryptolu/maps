/******************************************************************************
 *
 * Registers
 *
 ******************************************************************************/

#include "register.h"
#include "utils.h"

Register::Register()
{
	this->value = 0;
	this->tracer_ptr = nullptr;
}

Register::~Register()
{
	/* intentionally empty */
}

void Register::write(uint32_t val)
{
	unsigned int pwr = bit_count(this->value ^ val);
	this->value = val;
	this->tracer_ptr->update(pwr);
}


uint32_t Register::read(void)
{
	return this->value;
}

void Register::bind_tracer(Tracer *ptr)
{
	this->tracer_ptr = ptr;
}
