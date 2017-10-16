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
 * .npy files
 *
 ******************************************************************************/

#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>

#define MAJOR 0x01
#define MINOR 0x00

void save_npy(std::string filename, std::vector<double> vec)
{
	std::ofstream file;
	std::string header = "{'descr': '<f8', 'fortran_order': False, 'shape': (" + std::to_string(vec.size()) + ",), }";

	file.open(filename, std::ios::out | std::ios::binary);
	unsigned int len = 10 + header.length();
	unsigned int padding = 16 - (len % 16) - 1;
	if (padding > 0)
	{
		header.append(padding, ' ');
	}
	header.append("\n");
	uint16_t header_len = header.length();

	/* magic */
	file << static_cast<char>(0x93) << "NUMPY";
	/* versions */
	file << static_cast<char>(MAJOR) << static_cast<char>(MINOR);
	/* header */
	file.write((char *)&header_len, sizeof(uint16_t));
	file.write((char *)header.data(), header_len);
	/* data */
	file.write((char *)vec.data(), sizeof(double)*vec.size());

	file.close();
}
