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
 * options
 *
 ******************************************************************************/

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>

typedef struct
{
	uint32_t mem_size;                    /* memory size in bytes */
	std::string trace_index_filename;     /* name of the trace index file */
	std::string t_test_filename;          /* name of t_test result file */
	bool save_traces;                     /* select to save measure waveforms (debug only!) */
	unsigned long int n_measure;          /* number of measurements for t-test */
	bool with_gdb;                        /* true when connected to GDB server */
	bool with_pipeline_leakage;           /* include leakage from pipeline registers A and B */           
} Options;

const Options default_options =
{
	8*1024,
	"",
	"t_test.npy",
	false,
	0,
	false,
	false /* TODO: set it to true after functionality has been verified */
};

#endif
