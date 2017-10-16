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
