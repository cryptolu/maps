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
 * sec_algo interface (generic)
 *
 ******************************************************************************/

#ifndef __SIM_SEC_ALGO_H__
#define __SIM_SEC_ALGO_H__

#include <string>

typedef struct
{
	std::string trace_index_filename;
	std::string t_test_filename;
	bool save_traces;
	unsigned long int n_measure;
	bool with_gdb;
} Options;

void check_sec_algo(Options &options);
void t_test_sec_algo(Options &options);

#endif
