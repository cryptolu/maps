################################################################################
#
# University of Luxembourg
# Laboratory of Algorithmics, Cryptology and Security (LACS)
#
# arm_v7m_leakage simulator
#
# Copyright (C) 2017 University of Luxembourg
#
# Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu>
#
# This simulator is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# It is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#
################################################################################
#! /usr/bin/env python

import sys
import argparse
import numpy as np
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description = "t_test simulation analysis")
parser.add_argument("t_test_filename", help = "name of the .npy file containing the t_test results")
parser.add_argument("--trace_index", help = "name of the trace index file")

args = parser.parse_args()
t_test_filename = args.t_test_filename
trace_index_filename = args.trace_index

t = np.load(t_test_filename)

if trace_index_filename is not None:
	current_idx = 0
	trace_index = []
	with open(trace_index_filename, "r") as fh:
		for line in fh:
			if line[0:2] != '--':
				l = line[1:-2]
				ins_idx_str, p_addr = l.split(',')
				ins_idx = int(ins_idx_str) - 1
				#print(ins_idx, p_addr, current_idx, ins_idx + 1)
				for idx in range(current_idx, ins_idx + 1):
					trace_index.append(p_addr)
				current_idx = ins_idx + 1;

	#for i, paddr in enumerate(trace_index):
	#	print(i, paddr)

	leakage_index = np.where(np.abs(t) > 4.5)[0]
	if len(leakage_index) == 0:
		print("-- No leakage found")
	else:
		print("-- Leakage found at addresses:")
		for i in leakage_index:
			print("\t{} <{}>".format(trace_index[i], i))


plt.axhline(y = 4.5, color = 'r', linestyle = '--')
plt.axhline(y = -4.5, color = 'r', linestyle = '--')
plt.plot(t)
plt.show()
