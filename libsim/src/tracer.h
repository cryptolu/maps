/******************************************************************************
 *
 * Tracer
 *
 ******************************************************************************/

#ifndef __TRACER_H__
#define __TRACER_H__

#include <cstdint>
#include <vector>

class Tracer
{
	private:
		std::vector<unsigned int> trace;
		unsigned long int register_write_count;

	public:
		Tracer();
		~Tracer();

		void reset(void);
		void update(unsigned int value);
		std::vector<unsigned int> get_trace(void) const;
		unsigned long int get_register_write_count(void) const;
};

#endif
