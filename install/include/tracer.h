/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * arm_v7m_leakage simulator
 *
 * Copyright (C) 2017 University of Luxembourg
 *
 * Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu>
 *
 * This simulator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

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
