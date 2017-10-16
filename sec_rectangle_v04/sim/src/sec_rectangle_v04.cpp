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
 * interface to sec_rectangle_v04 simulation
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
	if (cpu->load("./sec_rectangle_v04.bin") < 0)
	{
		printf("-- ERROR: can not load ./sec_rectangle_v04.bin\n");
		std::exit(EXIT_FAILURE);
	}
}

template <typename T>
void mask(std::mt19937 &rnd_gen_uint32, T x, T *v, T *m)
{
	*m = rnd_gen_uint32();
	*v = x ^ *m;
}


unsigned long int sec_rectangle_v04_wrapper(std::mt19937 &rnd_gen_uint32, Cpu *cpu, uint16_t rows[4], const uint32_t rk[26*2])
{
	/* mask inputs */
	uint16_t buffer[8];
	mask(rnd_gen_uint32, rows[0], buffer + 0, buffer + 4);
	mask(rnd_gen_uint32, rows[1], buffer + 1, buffer + 5);
	mask(rnd_gen_uint32, rows[2], buffer + 2, buffer + 6);
	mask(rnd_gen_uint32, rows[3], buffer + 3, buffer + 7);

	cpu->copy_array_to_target((uint32_t *)buffer, 4, TARGET_BUFFER_ADDR);
	cpu->write_register(R0, TARGET_BUFFER_ADDR);

	/* mask round keys */
	uint32_t rk_masked[26*2*2];
	for (unsigned int i = 0; i < 26; ++i)
	{
		mask(rnd_gen_uint32, rk[2*i + 0], rk_masked + 4*i + 0, rk_masked + 4*i + 2);
		mask(rnd_gen_uint32, rk[2*i + 1], rk_masked + 4*i + 1, rk_masked + 4*i + 3);
	}

	cpu->copy_array_to_target(rk_masked, 26*2*2, TARGET_RK_MASKED_ADDR);
	cpu->write_register(R1, TARGET_RK_MASKED_ADDR);

	/* run simulation. At this point:
		- r0 contains the address of buffer
		- r1 contains the address of rk_masked
	*/
	cpu->reset_pwr_trace();
	unsigned long int count = cpu->run(0, 0xffffffff);

	/* read back results from buffer */
	cpu->copy_array_from_target((uint32_t *)buffer, 8, TARGET_BUFFER_ADDR);
	rows[0] = buffer[0] ^ buffer[4];
	rows[1] = buffer[1] ^ buffer[5];
	rows[2] = buffer[2] ^ buffer[6];
	rows[3] = buffer[3] ^ buffer[7];
	return count;
}


void check_sec_algo(Options &options)
{
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	Cpu cpu(options.trace_index_filename);

	load(&cpu);
	cpu.reset();

	uint16_t rows[] = {0xffff, 0xffff, 0xffff, 0xffff};
	const uint32_t rk[] =
	{
		0xffffffff, 0xffffffff, 0x00ff01ff, 0x00ffff00,
		0x01000301, 0xfefffeff, 0x02fff802, 0x02010100,
		0xfc000f04, 0xfb0202fe, 0x0afee5fa, 0xfa04f2fd,
		0xe7fdee13, 0xedfa04fa, 0xf0fae51c, 0xe113f906,
		0x17061de7, 0x1d1cf7ea, 0x02ea00e4, 0xe2e711e3,
		0x0ce30919, 0xf3e405f5, 0x05f50214, 0xf719f01c,
		0x081cedf0, 0x0514e412, 0xf612f518, 0x04f011e6,
		0x0fe6e206, 0x1418fd05, 0xed05e3ef, 0x0906e2fa,
		0x17fa16ec, 0xe9efee17, 0x0017e7fd, 0x07ec0c03,
		0xf403ebfb, 0xecfd08eb, 0xfbeb06ec, 0xeffbf310,
		0xf11014f3, 0x18ec1efe, 0xe5fe0312, 0x02f313f5,
		0xeef5f3ed, 0x1312f505, 0x1f05f602, 0xf7edf2fd,
		0xecfde7e0, 0xe5021e0a, 0xea0a0305, 0xf8e0e20b
	};
	const uint16_t expected_rows[] = {0x3ee8, 0xeeef, 0x154a, 0x467a};

	unsigned int long count = sec_rectangle_v04_wrapper(rnd_gen_uint32, &cpu, rows, rk);
	printf("-- %lu instructions executed\n", count);
	printf("-- rows = {0x%04x, 0x%04x, 0x%04x, 0x%04x} => ", rows[0], rows[1], rows[2], rows[3]);

	for (unsigned int i = 0; i < 4; ++i)
	{
		if (rows[i] != expected_rows[i])
		{
			printf("ERROR!\n");
			return;
		}
	}
	printf("ok\n");
}


void t_test_sec_algo(Options &options)
{
	Cpu cpu(options.trace_index_filename);
	Ttest *ttest_ptr = nullptr;
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	std::vector<unsigned int> trace;
	std::ofstream trace_file_fixed;
	std::ofstream trace_file_random;

	load(&cpu);
	cpu.reset();
	uint16_t rows_fixed[] = {0xffff, 0xffff, 0xffff, 0xffff};
	const uint32_t rk[] =
	{
		0xffffffff, 0xffffffff, 0x00ff01ff, 0x00ffff00,
		0x01000301, 0xfefffeff, 0x02fff802, 0x02010100,
		0xfc000f04, 0xfb0202fe, 0x0afee5fa, 0xfa04f2fd,
		0xe7fdee13, 0xedfa04fa, 0xf0fae51c, 0xe113f906,
		0x17061de7, 0x1d1cf7ea, 0x02ea00e4, 0xe2e711e3,
		0x0ce30919, 0xf3e405f5, 0x05f50214, 0xf719f01c,
		0x081cedf0, 0x0514e412, 0xf612f518, 0x04f011e6,
		0x0fe6e206, 0x1418fd05, 0xed05e3ef, 0x0906e2fa,
		0x17fa16ec, 0xe9efee17, 0x0017e7fd, 0x07ec0c03,
		0xf403ebfb, 0xecfd08eb, 0xfbeb06ec, 0xeffbf310,
		0xf11014f3, 0x18ec1efe, 0xe5fe0312, 0x02f313f5,
		0xeef5f3ed, 0x1312f505, 0x1f05f602, 0xf7edf2fd,
		0xecfde7e0, 0xe5021e0a, 0xea0a0305, 0xf8e0e20b
	};

	if (options.save_traces)
	{
		std::string filename_fixed = "trace_fixed_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_fixed.open(filename_fixed, std::ios::out | std::ios::binary);
		std::string filename_random = "trace_random_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_random.open(filename_random, std::ios::out | std::ios::binary);
	}

	Progress_bar progress_bar(options.n_measure, std::cout, "Simulating sec_rectangle_v04 ...\n");
	for (unsigned long int measure_idx = 0; measure_idx < options.n_measure; ++measure_idx)
	{
		uint16_t rows[4];
		/* fixed */
		rows[0] = rows_fixed[0];
		rows[1] = rows_fixed[1];
		rows[2] = rows_fixed[2];
		rows[3] = rows_fixed[3];
		sec_rectangle_v04_wrapper(rnd_gen_uint32, &cpu, rows, rk);
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
		rows[0] = rnd_gen_uint32() & 0xffff;
		rows[1] = rnd_gen_uint32() & 0xffff;
		rows[2] = rnd_gen_uint32() & 0xffff;
		rows[3] = rnd_gen_uint32() & 0xffff;
		sec_rectangle_v04_wrapper(rnd_gen_uint32, &cpu, rows, rk);
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
