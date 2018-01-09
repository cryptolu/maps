#! /usr/bin/env python
################################################################################
# 
#  University of Luxembourg
#  Laboratory of Algorithmics, Cryptology and Security (LACS)
# 
#  arm_v7m_leakage simulator
# 
#  Copyright (C) 2017 University of Luxembourg
# 
#  Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu> and
#  Daniel Dinu <daniel.dinu@uni.lu>
# 
#  This simulator is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  It is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
# 
################################################################################


import sys

try:
	fv = open("verilog_trace.log", "r")
except IOError:
	print("Can't open file 'verilog_trace.log'")
	sys.exit(-1)

try:
	fs = open("sim_trace.log", "r")
except IOError:
	print("Cant' open file 'sim_trace.log'")
	sys.exit(-1)

v_line_idx = 0
s_line_idx = 0

# Find start of verilog trace
while True:
	line_v = fv.readline().rstrip()
	if line_v == '':
		print("Error: can't find start of trace in verilog trace")
		sys.exit(-1)
	v_line_idx += 1
	if line_v[0] != '>':
		break
	
# Find start of sim trace
while True:
	line_s = fs.readline().rstrip()
	if line_s == '':
		print("Error: can't find start of trace in sim trace")
		sys.exit(-1)
	s_line_idx += 1
	if line_s == line_v:
		break

# Compare trace
#   verilog trace is the reference and should be shorter
n_diff = 0
while True:
	line_v = fv.readline().rstrip()
	if line_v == '':
		break
	v_line_idx += 1
	line_s = fs.readline().rstrip()
	if line_s == '':
		print("Error: sim trace is expected to be shorter than v trace")
		sys.exit(-1)
	s_line_idx += 1
	if line_v != line_s:
		n_diff += 1
		print("Found difference: v@{}/{} s@{}/{}".format(v_line_idx, line_v, s_line_idx, line_s))

if n_diff == 0:
	print("Traces are identical")
else:
	print("Found differences between traces")
