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

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "t_test.h"


Ttest::Ttest(unsigned int n_sample)
{
	this->n_sample = n_sample;
	this->n1 = 0;
	this->m1.resize(n_sample, 0.0);
	this->v1.resize(n_sample, 0.0);
	this->n2 = 0;
	this->m2.resize(n_sample, 0.0);
	this->v2.resize(n_sample, 0.0);
}

Ttest::~Ttest()
{
}

void Ttest::reset(void)
{
	this->m1.clear();
	this->m1.resize(this->n_sample, 0.0);
	this->v1.clear();
	this->v1.resize(this->n_sample, 0.0);
	this->n1 = 0;
	this->m2.clear();
	this->m2.resize(this->n_sample, 0.0);
	this->v2.clear();
	this->v2.resize(this->n_sample, 0.0);
	this->n2 = 0;
}

void Ttest::update1(std::vector<unsigned int> vec)
{
	double delta1;
	double delta2;

	for (unsigned int i = 0; i < this->n_sample; ++i)
	{
		delta1 = static_cast<double>(vec[i]) - this->m1[i];
		this->m1[i] += delta1/(this->n1 + 1);
		delta2 = static_cast<double>(vec[i]) - this->m1[i];
		this->v1[i] += delta1*delta2;
	}
	this->n1++;
}

void Ttest::update2(std::vector<unsigned int> vec)
{
	double delta1;
	double delta2;

	for (unsigned int i = 0; i < this->n_sample; ++i)
	{
		delta1 = static_cast<double>(vec[i]) - this->m2[i];
		this->m2[i] += delta1/(this->n2 + 1);
		delta2 = static_cast<double>(vec[i]) - this->m2[i];
		this->v2[i] += delta1*delta2;
	}
	this->n2++;
}

std::vector<double> Ttest::t_test(void)
{
	std::vector<double> t;
	double var1;
	double var2;
	double term1;
	double term2;
	double term;

	for (unsigned int i = 0; i < this->n_sample; ++i)
	{
		var1 = this->v1[i]/(this->n1 - 1);
		var2 = this->v2[i]/(this->n2 - 1);
		term1 = var1/this->n1;
		term2 = var2/this->n2;
		term = sqrt(term1 + term2);
		if (term == 0.0)
		{
			t.push_back(0.0);
		}
		else
		{
			t.push_back((this->m1[i] - this->m2[i])/term);
		}
	}
	return t;
}
