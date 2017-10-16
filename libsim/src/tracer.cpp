/******************************************************************************
 *
 * Tracer
 *
 ******************************************************************************/

#include <cstdint>
#include "tracer.h"

Tracer::Tracer()
{
	/* intentionally empty */
}

Tracer::~Tracer()
{
	/* intentionally empty */
}

void Tracer::reset(void)
{
	this->trace.clear();
	this->register_write_count = 0;
}

void Tracer::update(unsigned int value)
{
	/* TODO: define power model */
	this->trace.push_back(value);
	this->register_write_count++;
}

std::vector<unsigned int> Tracer::get_trace(void) const
{
	return this->trace;
}

unsigned long int Tracer::get_register_write_count(void) const
{
	return this->register_write_count;
}
