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
 * interface to sec_speck_v03 simulation
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
#include "sim_sec_algo.h"


#define TARGET_BUFFER_ADDR 0x0400
#define TARGET_RK_MASKED_ADDR 0x0500

void load(Cpu *cpu)
{
	if (cpu->load("./sec_speck_v03.bin") < 0)
	{
		printf("-- ERROR: can not load ./sec_speck_v03.bin\n");
		std::exit(EXIT_FAILURE);
	}
}

void mask(std::mt19937 &rnd_gen_uint32, uint32_t x, uint32_t *v, uint32_t *m)
{
	*m = rnd_gen_uint32();
	*v = x ^ *m;
}


unsigned long int sec_speck_v03_wrapper(std::mt19937 &rnd_gen_uint32, Cpu *cpu, uint32_t *l, uint32_t *r, uint32_t *rk)
{
	/* mask inputs */
	uint32_t buffer[6];
	mask(rnd_gen_uint32, *r, buffer, buffer + 1);
	mask(rnd_gen_uint32, *l, buffer + 2, buffer + 3);
	buffer[4] = rnd_gen_uint32();
	buffer[5] = rnd_gen_uint32();

	cpu->copy_array_to_target(buffer, 6, TARGET_BUFFER_ADDR);
	cpu->write_register(R0, TARGET_BUFFER_ADDR);

	/* mask round keys */
	uint32_t rk_masked[27*2];
	for (unsigned int i = 0; i < 27; ++i)
	{
		mask(rnd_gen_uint32, rk[i], rk_masked + 2*i, rk_masked + 2*i + 1);
	}
	cpu->copy_array_to_target(rk_masked, 27*2, TARGET_RK_MASKED_ADDR);
	cpu->write_register(R1, TARGET_RK_MASKED_ADDR);

	/* run simulation. At this point:
		- r0 contains the address of buffer
		- r1 contains the address of rk_masked
	*/
	cpu->reset_pwr_trace();
	unsigned long int count = cpu->run(0, 0xffffffff);

	/* read back results from buffer */
	cpu->copy_array_from_target(buffer, 4, TARGET_BUFFER_ADDR);
	*r = buffer[0] ^ buffer[1];
	*l = buffer[2] ^ buffer[3];
	return count;
}


void check_sec_algo(Options &options)
{
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	Cpu cpu(options.with_gdb, options.trace_index_filename);

	load(&cpu);
	cpu.reset();

	uint32_t l = 0x3b726574;
	uint32_t r = 0x7475432d;
	uint32_t rk[] = {
		0x03020100, 0x131d0309, 0xbbd80d53, 0x0d334df3,
		0x7fa43565, 0x67e6ce55, 0xe98cb3d2, 0xaac76cbd,
		0x7f5951c8, 0x03fa82c2, 0x313533ad, 0xdff70882,
		0x9e487c93, 0xa934b928, 0xdd2edef5, 0x8be6388d,
		0x1f706b89, 0x2b87aaf8, 0x12d76c17, 0x6eaccd6c,
		0x6a1ab912, 0x10bc6bca, 0x6057dd32, 0xd3c9b381,
		0xb347813d, 0x8c113c35, 0xfe6b523a
	};

	unsigned int long count = sec_speck_v03_wrapper(rnd_gen_uint32, &cpu, &l, &r, rk);
	printf("-- %lu instructions executed\n", count);
	printf("-- l = 0x%08x, r = 0x%08x => ", l, r);
	if (l == 0x8c6fa548 && r == 0x454e028b)
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
	Cpu cpu(options.with_gdb, options.trace_index_filename);
	Ttest *ttest_ptr = nullptr;
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	std::vector<unsigned int> trace;
	std::ofstream trace_file_fixed;
	std::ofstream trace_file_random;

	load(&cpu);
	cpu.reset();
	uint32_t l_fixed = 0x3b726574;
	uint32_t r_fixed = 0x7475432d;
	uint32_t rk[] = {
		0x03020100, 0x131d0309, 0xbbd80d53, 0x0d334df3,
		0x7fa43565, 0x67e6ce55, 0xe98cb3d2, 0xaac76cbd,
		0x7f5951c8, 0x03fa82c2, 0x313533ad, 0xdff70882,
		0x9e487c93, 0xa934b928, 0xdd2edef5, 0x8be6388d,
		0x1f706b89, 0x2b87aaf8, 0x12d76c17, 0x6eaccd6c,
		0x6a1ab912, 0x10bc6bca, 0x6057dd32, 0xd3c9b381,
		0xb347813d, 0x8c113c35, 0xfe6b523a
	};

	if (options.save_traces)
	{
		std::string filename_fixed = "trace_fixed_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_fixed.open(filename_fixed, std::ios::out | std::ios::binary);
		std::string filename_random = "trace_random_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_random.open(filename_random, std::ios::out | std::ios::binary);
	}

	Progress_bar progress_bar(options.n_measure, std::cout, "Simulating sec_speck_v03 ...\n");
	for (unsigned long int measure_idx = 0; measure_idx < options.n_measure; ++measure_idx)
	{
		uint32_t l;
		uint32_t r;
		/* fixed */
		l = l_fixed;
		r = r_fixed;
		sec_speck_v03_wrapper(rnd_gen_uint32, &cpu, &l, &r, rk);
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
		l = rnd_gen_uint32();
		r = rnd_gen_uint32();
		sec_speck_v03_wrapper(rnd_gen_uint32, &cpu, &l, &r, rk);
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
