/******************************************************************************
 *
 * interface to sec_add_v05 simulation
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


#define TARGET_A_BUF_ADDR 0x0200
#define TARGET_B_BUF_ADDR 0x0300

void load(Cpu *cpu)
{
	if (cpu->load("./sec_add_v05.bin") < 0)
	{
		printf("-- ERROR: can not load ./sec_add_v05.bin\n");
		std::exit(EXIT_FAILURE);
	}
}

void mask(std::mt19937 &rnd_gen_uint32, uint32_t x, uint32_t *v, uint32_t *m)
{
	*m = rnd_gen_uint32();
	*v = x ^ *m;
}

unsigned long int sec_add_v05_wrapper(std::mt19937 &rnd_gen_uint32, Cpu *cpu, uint32_t a, uint32_t b, uint32_t *y)
{
    /* mask inputs */
    uint32_t a_buf[2];
    mask(rnd_gen_uint32, a, a_buf, a_buf + 1);
    cpu->copy_array_to_target(a_buf, 4, TARGET_A_BUF_ADDR);
	cpu->write_register(R0, TARGET_A_BUF_ADDR);

	uint32_t b_buf[2];
    mask(rnd_gen_uint32, b, b_buf, b_buf + 1);
    cpu->copy_array_to_target(b_buf, 4, TARGET_B_BUF_ADDR);
	cpu->write_register(R1, TARGET_B_BUF_ADDR);

    /* run simulation. At this point:
        - r0 contains the address of a_buf
        - r1 contains the address of b_buf
    */
    cpu->reset_pwr_trace();
    unsigned long int count = cpu->run(0, 0xffffffff);

    /* read back results from buffer */
    cpu->copy_array_from_target(a_buf, 2, TARGET_A_BUF_ADDR);
	*y = a_buf[0] ^ a_buf[1];
    return count;
}

void check_sec_algo(Options &options)
{
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	Cpu cpu(options.trace_index_filename);
	uint32_t a = 0xb1a6f5e5;
	uint32_t b = 0x127cbff6;
	uint32_t y;

	load(&cpu);
	cpu.reset();

	unsigned long int count = sec_add_v05_wrapper(rnd_gen_uint32, &cpu, a, b, &y);
	printf("-- %lu instructions executed\n", count);
	printf("-- y = 0x%08x => ", y);
	if (y == (a + b))
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
	const uint32_t a_fixed = 0xb1a6f5e5;
	const uint32_t b_fixed = 0x127cbff6;

	if (options.save_traces)
	{
		std::string filename_fixed = "trace_fixed_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_fixed.open(filename_fixed, std::ios::out | std::ios::binary);
		std::string filename_random = "trace_random_n_measure_" + std::to_string(options.n_measure) + ".bin";
		trace_file_random.open(filename_random, std::ios::out | std::ios::binary);
	}

	Progress_bar progress_bar(options.n_measure, std::cout, "Simulating sec_add_v05 ...\n");
	for (unsigned long int measure_idx = 0; measure_idx < options.n_measure; ++measure_idx)
	{
		uint32_t a;
		uint32_t b;
		uint32_t y;
		/* fixed */
		a = a_fixed;
		b = b_fixed;
		sec_add_v05_wrapper(rnd_gen_uint32, &cpu, a, b, &y);
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
		a = rnd_gen_uint32();
		b = rnd_gen_uint32();
		sec_add_v05_wrapper(rnd_gen_uint32, &cpu, a, b, &y);
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

