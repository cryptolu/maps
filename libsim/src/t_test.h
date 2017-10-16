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
