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
