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
} Options;

void check_sec_algo(Options &options);
void t_test_sec_algo(Options &options);

#endif
