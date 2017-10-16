#! /usr/bin/env python
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

from __future__ import print_function, division

import sys
import numpy as np
import matplotlib.pyplot as plt
import random
import t_test
import inspect

def hw(x):
	u = x
	h = 0
	while u > 0:
		h += (u & 1)
		u >>= 1
	return h

def hd(x, y):
	return hw(x ^ y)

def ror(x, n):
	return ((x >> n) | (x << (32 - n))) & 0xffffffff


def rnd_uint32():
	return random.getrandbits(32)


R0 = 0
R1 = 1
R2 = 2
R3 = 3
R4 = 4
R5 = 5
R6 = 6
R7 = 7
R8 = 8
R9 = 9
R10 = 10
R11 = 11
R12 = 12
R13 = 13
R14 = 14
R15 = 15
SP = 13
LR = 14
PC = 15

class Cpu(object):
	def __init__(self):
		self.regs = [0]*16

	def mov_imm(self, rd, imm):
		self.regs[rd] = imm

	def	mov(self, rd, rm, n = 0):
		
		""" rm is written to regb and then regb is written to rd """
		self.regs[rd] = ror(self.reg[rm], n)

	def and_(self, rd, rn, rm, n = 0):
		v = self.regs[rn] & ror(self.regs[rm], n)
		self.regs[rd] = v

	def orn(self, rd, rn, rm, n = 0):
		v = (self.regs[rn] | ~(ror(self.regs[rm], n))) & 0xffffffff
		self.regs[rd] = v

	def eor(self, rd, rn, rm, n = 0):
		v = self.regs[rn] ^ ror(self.regs[rm], n)
		self.regs[rd] = v

	def lsl(self, rd, rn, rm):
		v = (self.regs[rn] << self.regs[rm]) & 0xffffffff
		self.regs[rd] = v

	def lsl_imm(self, rd, rn, imm):
		v = (self.regs[rn] << imm) & 0xffffffff
		self.regs[rd] = v

	def read_reg(self, rn):
		return self.regs[rn]

	def dump(self):
		print("R0 : 0x{:08x}, R1 : 0x{:08x}, R2 : 0x{:08x}, R3 : 0x{:08x}".format(self.regs[R0], self.regs[R1], self.regs[R2], self.regs[R3]))
		print("R4 : 0x{:08x}, R5 : 0x{:08x}, R6 : 0x{:08x}, R7 : 0x{:08x}".format(self.regs[R4], self.regs[R5], self.regs[R6], self.regs[R7]))
		print("R8 : 0x{:08x}, R9 : 0x{:08x}, R10: 0x{:08x}, R11: 0x{:08x}".format(self.regs[R8], self.regs[R9], self.regs[R10], self.regs[R11]))
		print("R12: 0x{:08x}, SP : 0x{:08x}, LR : 0x{:08x}, PC : 0x{:08x}".format(self.regs[R12], self.regs[SP], self.regs[LR], self.regs[PC]))

	def SEC_SHIFT(self, r_mval, r_mask, x_mval, x_mask, val):
		self.lsl(r_mval, x_mval, val)
		self.lsl(r_mask, x_mask, val)

	def SEC_AND(self, r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp):
		self.and_(r_mval, x_mval, y_mval)
		self.orn(temp, x_mval, y_mask)
		self.eor(r_mval, r_mval, temp)
		self.and_(r_mask, x_mask, y_mval)
		self.orn(temp, x_mask, y_mask)
		self.eor(r_mask, r_mask, temp)

	def SEC_XOR(self, r_mval, x_mval, x_mask, y_mask, y_mval):
		self.eor(r_mval, x_mval, y_mval)
		self.eor(r_mval, r_mval, y_mask)

	def SEC_AND_IN_PLACE(self, r_mask, x_mval, x_mask, y_mask, y_mval, temp):
		self.and_(temp, x_mval, y_mval)
		self.orn(x_mval, x_mval, y_mask)
		self.eor(x_mval, x_mval, temp)
		self.and_(temp, x_mask, y_mval)
		self.orn(r_mask, x_mask, y_mask)
		self.eor(r_mask, r_mask, temp)

def sec_add_v05(cpu, a, b, masked = True):
	if masked == True:
		am = rnd_uint32()
		bm = rnd_uint32()
	else:
		am = 0
		bm = 0
	av = a ^ am
	bv = b ^ bm
	# load data
	cpu.mov_imm(R2, av)
	cpu.mov_imm(R3, am)
	cpu.mov_imm(R4, bv)
	cpu.mov_imm(R5, bm)

	# p(mval, mask) = (r8, r3)
	cpu.SEC_XOR(R8, R4, R5, R2, R3)

	# g(mval, mask) = (r9, r6)
	cpu.SEC_AND(R9, R6, R2, R3, R4, R5, R12)

	# g(mval, mask) = (r9, r3)
	cpu.eor(R9, R9, R3)
	cpu.eor(R9, R9, R6)

	r2_save = cpu.read_reg(R2)

	cpu.mov_imm(R12, 1)
	while True:

		# h(mval, mask) = (r10, r7)
		cpu.SEC_SHIFT(R10, R7, R9, R3, R12)

		# u(mval, mask) = (r11, r6)
		cpu.SEC_AND(R11, R6, R8, R5, R10, R7, R2)

		# g(mval, mask) = (r9, r3)
		cpu.SEC_XOR(R9, R9, R3, R11, R6)

		# h(mval, mask) = (r10, r7)
		cpu.SEC_SHIFT(R10, R7, R8, R5, R12)

        # h(mval, mask) = (r10, r3)
		cpu.eor(R10, R10, R3)
		cpu.eor(R10, R10, R7)

		# p(mval, mask) = (r8, r6)
		cpu.SEC_AND_IN_PLACE(R6, R8, R5, R10, R3, R7)

		# p(mval, mask) = (r8, r5)
		cpu.eor(R8, R8, R5)
		cpu.eor(R8, R8, R6)

		cpu.lsl_imm(R12, R12, 1)

		if (cpu.read_reg(R12) == 16):
			break

	# h(mval, mask) = (r10, r7)
	cpu.SEC_SHIFT(R10, R7, R9, R3, R12)

	# u(mval, mask) = (r11, r6)
	cpu.SEC_AND(R11, R6, R8, R5, R10, R7, R12)

	# g(mval, mask) = (r9, r3)
	cpu.SEC_XOR(R9, R9, R3, R11, R6)

	cpu.mov_imm(R2, r2_save)

	# z(mval, mask) = (r11, r5)
	cpu.SEC_XOR(R11, R4, R5, R2, R3)

	# z(mval, mask) = (r11, r5)
	#  cpu.eor(R11, R11, R9, LSL #1)
	r9_lsl1 = (cpu.read_reg(R9) << 1) & 0xffffffff
	cpu.regs[R11] = cpu.read_reg(R11) ^ r9_lsl1
	#  cpu.eor(R11, R11, R3, LSL #1)
	r3_lsl1 = (cpu.read_reg(R3) << 1) & 0xffffffff
	cpu.regs[R11] = cpu.read_reg(R11) ^ r3_lsl1

	# Change mask
	# z(mval, mask) = (r2, r3)
	cpu.eor(R2, R11, R3)
	cpu.eor(R2, R2, R5)

	y = cpu.read_reg(R2) ^ cpu.read_reg(R3)
	return y


cpu = Cpu()
a = 0xb1a6f5e5
b = 0x127cbff6
y = sec_add_v05(cpu, a, b, False)
print("-- y = 0x{:08x} => ".format(y), end = '')
if (y == a ^ b):
	print("ok")
else:
	print("ERROR!")

