/******************************************************************************
 *
 * interface to sec_simon simulation
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


#define TARGET_BUFFER_ADDR 0x0200
#define TARGET_RK_MASKED_ADDR 0x0300

void load(Cpu *cpu)
{
	if (cpu->load("./sec_simon.bin") < 0)
	{
		printf("-- ERROR: can not load ./sec_simon.bin\n");
		std::exit(EXIT_FAILURE);
	}
}

void mask(std::mt19937 &rnd_gen_uint32, uint32_t x, uint32_t *v, uint32_t *m)
{
	*m = rnd_gen_uint32();
	*v = x ^ *m;
}


unsigned long int sec_simon_wrapper(std::mt19937 &rnd_gen_uint32, Cpu *cpu, uint32_t *l, uint32_t *r, uint32_t *rk)
{
	/* mask inputs */
	uint32_t buffer[4];
	mask(rnd_gen_uint32, *r, buffer, buffer + 1);
	mask(rnd_gen_uint32, *l, buffer + 2, buffer + 3);

	cpu->copy_array_to_target(buffer, 4, TARGET_BUFFER_ADDR);
	cpu->write_register(R0, TARGET_BUFFER_ADDR);

	/* mask round keys */
	uint32_t rk_masked[88];
	for (unsigned int i = 0; i < 44; ++i)
	{
		mask(rnd_gen_uint32, rk[i], rk_masked + 2*i, rk_masked + 2*i + 1);
	}
	cpu->copy_array_to_target(rk_masked, 88, TARGET_RK_MASKED_ADDR);
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
	Cpu cpu(options.trace_index_filename);

	load(&cpu);
	cpu.reset();

	uint32_t l = 0x656b696c;
	uint32_t r = 0x20646e75;
	uint32_t rk[] = {
		0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918,
		0x70a011c3, 0xb770ec49, 0x57e3e835, 0xd397bc42,
		0x94dcf81f, 0xbf4b5f18, 0x8e5dabb9, 0xdbf4a863,
		0xcd0c28fc, 0x5cb69911, 0x79f112a5, 0x77205863,
		0x99880c12, 0x1ce97c58, 0xc8ed2145, 0xb800dbb8,
		0xe86a2756, 0x7c06d4dd, 0xab52df0a, 0x247f66a8,
		0x53587ca6, 0xd25c13f1, 0x4583b64b, 0x7d9c960d,
		0xefbfc2f3, 0x89ed8513, 0x308dfc4e, 0xbf1a2a36,
		0xe1499d70, 0x4ce4d2ff, 0x32b7ebef, 0xc47505c1,
		0xd0e929e8, 0x8fe484b9, 0x42054bee, 0xaf77bae2,
		0x18199c02, 0x719e3f1c, 0x0c1cf793, 0x15df4696
	};

	unsigned int long count = sec_simon_wrapper(rnd_gen_uint32, &cpu, &l, &r, rk);
	printf("-- %lu instructions executed\n", count);
	printf("-- l = 0x%08x, r = 0x%08x => ", l, r);
	if (l == 0x44c8fc20 && r == 0xb9dfa07a)
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
	Cpu cpu(options.trace_index_filename);
	Ttest *ttest_ptr = nullptr;
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	std::vector<unsigned int> trace;
	std::ofstream trace_file_fixed;
	std::ofstream trace_file_random;

	load(&cpu);
	cpu.reset();
	const uint32_t l_fixed = 0x656b696c;
	const uint32_t r_fixed = 0x20646e75;
	uint32_t rk[] = {
		0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918,
		0x70a011c3, 0xb770ec49, 0x57e3e835, 0xd397bc42,
		0x94dcf81f, 0xbf4b5f18, 0x8e5dabb9, 0xdbf4a863,
		0xcd0c28fc, 0x5cb69911, 0x79f112a5, 0x77205863,
		0x99880c12, 0x1ce97c58, 0xc8ed2145, 0xb800dbb8,
		0xe86a2756, 0x7c06d4dd, 0xab52df0a, 0x247f66a8,
		0x53587ca6, 0xd25c13f1, 0x4583b64b, 0x7d9c960d,
		0xefbfc2f3, 0x89ed8513, 0x308dfc4e, 0xbf1a2a36,
		0xe1499d70, 0x4ce4d2ff, 0x32b7ebef, 0xc47505c1,
		0xd0e929e8, 0x8fe484b9, 0x42054bee, 0xaf77bae2,
		0x18199c02, 0x719e3f1c, 0x0c1cf793, 0x15df4696
	};

	if (options.save_traces)
	{
		std::string filename_fixed = "trace_fixed_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_fixed.open(filename_fixed, std::ios::out | std::ios::binary);
		std::string filename_random = "trace_random_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_random.open(filename_random, std::ios::out | std::ios::binary);
	}

	Progress_bar progress_bar(options.n_measure, std::cout, "Simulating sec_simon ...\n");
	for (unsigned long int measure_idx = 0; measure_idx < options.n_measure; ++measure_idx)
	{
		uint32_t l;
		uint32_t r;
		/* fixed */
		l = l_fixed;
		r = r_fixed;
		sec_simon_wrapper(rnd_gen_uint32, &cpu, &l, &r, rk);
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
		sec_simon_wrapper(rnd_gen_uint32, &cpu, &l, &r, rk);
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
