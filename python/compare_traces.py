#! /usr/bin/env python

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
