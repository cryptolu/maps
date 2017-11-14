#! /usr/bin/env python

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
