/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * arm_v7m_leakage simulator
 *
 * Copyright (C) 2017 University of Luxembourg
 *
 * Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu> and
 * Daniel Dinu <daniel.dinu@uni.lu>
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
