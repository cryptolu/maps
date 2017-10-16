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
 * Progress bar (copied from by Boot 1.5)
 *
 ******************************************************************************/

#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__

#include <string>
#include <fstream>
#include <iostream>

class Progress_bar
{
	private:
		std::ostream &m_os;
		const std::string m_s1;
		const std::string m_s2;
		const std::string m_s3;
		unsigned long int _count;
		unsigned long int _expected_count;
		unsigned long int _next_tic_count;
		unsigned int _tic;

		void display_tic(void);

	public:
		explicit Progress_bar(
			unsigned long expected_count,
			std::ostream & os = std::cout,
			const std::string &s1 = "\n", //leading strings
			const std::string &s2 = "",
			const std::string &s3 = ""
		);

	void restart(unsigned long expected_count);
	unsigned long operator+=(unsigned long increment);
	unsigned long operator++();
	unsigned long count() const;
	unsigned long expected_count() const;


};

#endif
