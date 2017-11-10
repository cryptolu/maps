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
 * interface to experiment simulation
 *
 ******************************************************************************/

#include <cstdio>
#include <cstdint>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <progress_bar.h>
#include "cpu.h"
#include "t_test.h"
#include "npy.h"
#include "options.h"
#include "sim_sec_algo.h"


#define TARGET_A_BUF_ADDR 0x20000084

void load(Cpu *cpu)
{
	if (cpu->load("./experiment.bin") < 0)
	{
		printf("-- ERROR: can not load ./experiment.bin\n");
		std::exit(EXIT_FAILURE);
	}
}

void mask(std::mt19937 &rnd_gen_uint32, uint32_t x, uint32_t *v, uint32_t *m)
{
	*m = rnd_gen_uint32();
	*v = x ^ *m;
}

unsigned long int experiment_wrapper(std::mt19937 &rnd_gen_uint32, Cpu *cpu, uint32_t x1, uint32_t x2, uint32_t *y1, uint32_t *y2)
{
    /* transfer inputs */
    uint32_t buf[2];
	buf[0] = x1;
	buf[1] = x2;
    cpu->copy_array_to_target(buf, 2, TARGET_A_BUF_ADDR);
	cpu->write_register(R0, TARGET_A_BUF_ADDR);

    /* run simulation. At this point:
        - r0 contains the address of a_buf
        - r1 contains the address of b_buf
    */
    cpu->reset_pwr_trace();
    unsigned long int count = cpu->run(0, 0xffffffff);

    /* read back results from buffer */
    cpu->copy_array_from_target(buf, 2, TARGET_A_BUF_ADDR);
	*y1 = buf[0];
	*y2 = buf[1];
    return count;
}

void check_sec_algo(Options &options)
{
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	options.mem_size = 0x2000fc00; /* try to match verilog simulation */
	Cpu cpu(options);
	uint32_t x1 = 0x12345678;
	uint32_t x2 = 0x90abcdef;
	uint32_t y1;
	uint32_t y2;

	load(&cpu);
	cpu.reset();

	unsigned long int count = experiment_wrapper(rnd_gen_uint32, &cpu, x1, x2, &y1, &y2);
	printf("-- %lu instructions executed\n", count);
	printf("-- y1 = 0x%08x, y2 = 0x%08x => ", y1, y2);
	if (y1 == 0x048d15df && y2 == 0x000000a8)
	{
		printf("ok\n");
	}
	else
	{
		printf("ERROR!\n");
	}
}

void t_test_sec_algo(Options &options)
{
	Cpu cpu(options);
	Ttest *ttest_ptr = nullptr;
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	std::vector<unsigned int> trace;
	std::ofstream trace_file_fixed;
	std::ofstream trace_file_random;

	load(&cpu);
	cpu.reset();
	const uint32_t x_fixed = 0x00000000;

	if (options.save_traces)
	{
		std::string filename_fixed = "trace_fixed_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_fixed.open(filename_fixed, std::ios::out | std::ios::binary);
		std::string filename_random = "trace_random_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_random.open(filename_random, std::ios::out | std::ios::binary);
	}

	Progress_bar progress_bar(options.n_measure, std::cout, "Simulating experiment ...\n");
	for (unsigned long int measure_idx = 0; measure_idx < options.n_measure; ++measure_idx)
	{
		uint32_t x;
		uint32_t x1;
		uint32_t x2;
		uint32_t y1;
		uint32_t y2;
		/* fixed */
		x = x_fixed;
		x2 = rnd_gen_uint32();
		x1 = x ^ x2;
		experiment_wrapper(rnd_gen_uint32, &cpu, x1, x2, &y1, &y2);
		trace = cpu.get_pwr_trace();
		if (measure_idx == 0)
		{
			ttest_ptr = new Ttest(trace.size());
		}
		ttest_ptr->update1(trace);
		if (options.save_traces)
		{
			trace_file_fixed.write((char *)trace.data(), sizeof(unsigned int)*trace.size());
		}
		/* random */
		x = rnd_gen_uint32();
		x2 = rnd_gen_uint32();
		x1 = x ^ x2;
		experiment_wrapper(rnd_gen_uint32, &cpu, x1, x2, &y1, &y2);
		trace = cpu.get_pwr_trace();
		ttest_ptr->update2(trace);
		if (options.save_traces)
		{
			trace_file_random.write((char *)trace.data(), sizeof(unsigned int)*trace.size());
		}
		++progress_bar;
	}

	std::vector<double> t = ttest_ptr->t_test();
	save_npy(options.t_test_filename, t);

	if (options.save_traces)
	{
		trace_file_fixed.close();
		trace_file_random.close();
	}

	if (ttest_ptr != nullptr)
	{
		delete ttest_ptr;
	}
}

