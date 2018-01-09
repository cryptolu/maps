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


regs_keys = ("r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r9", "r10", "r11", "r12", "r13", "r13", "rA", "rB")

regs = {}
for k in regs_keys:
	regs[k] = "xxxxxxxx"

with open("tarmac.log", "r") as fh:
	nop_count = 0
	state = 'SEARCH_START'
	for line in fh:
		fields = line.split();
		if len(fields) >= 4:
			if fields[2] == 'R':
				reg_name = fields[3]
				if reg_name == 'r13_mn':
					reg_name = "r13"
				if reg_name == 'psr':
					continue
				reg_value = fields[4]
				regs[reg_name] = reg_value
				if state == 'START_FOUND':
					print("{} = {}".format(reg_name, reg_value))
				continue
		if len(fields) >= 8:
			if fields[7] == 'NOP':
				nop_count += 1
				if nop_count == 15:
					if state == 'SEARCH_START':
						state = 'START_FOUND'
						for k in regs_keys:
							print("> {} = {}".format(k, regs[k]))
					elif state == 'START_FOUND':
						state = 'DONE'
				continue
		nop_count = 0
