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
 * Main
 *
 ******************************************************************************/


#include <cstdio>
#include <cstdint>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "t_test.h"
#include "npy.h"
#include "sim_sec_algo.h"

int main(int argc, char *argv[])
{
	Options options = {"", "t_test.npy", false, 0};
	bool do_test = false;
	int c;

	while ((c = getopt(argc, argv, "sto:n:i:")) != -1)
	{
		switch (c)
		{
			case 'i':
				options.trace_index_filename = optarg;
				break;
			case 's':
				options.save_traces = true;
				break;
			case 't':
				do_test = true;
				break;
			case 'o':
				options.t_test_filename = optarg;
				break;
			case 'n':
				options.n_measure = strtoul(optarg, NULL, 0);
				break;
			default:
                fprintf(stderr, "%s [-i <trace_index_file>] [-s] [-o <filename>] [-t | -n <n_measure]>\n", argv[0]);
                fprintf(stderr, "\t-i: generate power trace index\n");
				fprintf(stderr, "\t-s: save traces\n");
				fprintf(stderr, "\t-t: test for correctness with test vectors\n");
				fprintf(stderr, "\t-o: name of .npy file. Default to 't_test.npy'\n");
				fprintf(stderr, "\t-n: number of measurements\n");
				std::exit(EXIT_FAILURE);
		}
	}
	if (options.n_measure == 0 && do_test == false)
	{
		fprintf(stderr, "ERROR: -n <unsigned int> required\n");
		std::exit(EXIT_FAILURE);
	}
	
	if (do_test)
	{
		check_sec_algo(options);
	}
	else
	{
		t_test_sec_algo(options);
	}

	return 0;
}
