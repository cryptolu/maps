/******************************************************************************
 *
 * Welsch t-test
 *
 ******************************************************************************/

#ifndef __T_TEST_H__
#define __T_TEST_H__

#include <vector>

class Ttest
{
	private:
		unsigned int n_sample;
		unsigned int n1;
		unsigned int n2;
		std::vector<double> v1;
		std::vector<double> v2;
		std::vector<double> m1;
		std::vector<double> m2;

	public:
		Ttest(unsigned int n_sample);
		~Ttest();
		void reset(void);
		void update1(std::vector<unsigned int> vec);
		void update2(std::vector<unsigned int> vec);
		std::vector<double> t_test(void);
};

#endif
