#! /usr/bin/env python

import sys
import random

def neg(x):
	return (~x) & 0xffffffff

class Masked(object):

	def __init__(self, v = 0, m = 0):
		if (m == -1):
			self.m = random.getrandbits(32)
			self.v = v ^ self.m
		else:
			self.v = v
			self.m = m

	def unmask(self):
		return self.v ^ self.m

	def __xor__(self, other):
		res_v = self.v ^ other.v
		res_m = self.m ^ other.m
		return Masked(res_v, res_m)

	def __invert__(self):
		return Masked(neg(self.v), self.m)

	def __and__(self, other):
		r = random.getrandbits(32)
		t = self.v & other.v
		v = r ^ t
		t = self.v & other.m
		v = v ^ t
		t = self.m & other.v
		v = v ^ t
		t = self.m & other.m
		v = v ^ t
		return Masked(v, r)

	def __or__(self, other):
		m = 0x82a17954 # random.getrandbits(32)
		xv_bar = neg(self.v)
		yv_bar = neg(other.v)
		t = xv_bar & yv_bar
		v_bar = m ^ t
		t = xv_bar & other.m
		v_bar = v_bar ^ t
		t = self.m & yv_bar
		v_bar = v_bar ^ t
		t = self.m & other.m
		v_bar = v_bar ^ t
		v = neg(v_bar)
		return Masked(v, m)

	def __repr__(self):
		return "0x{:08x} <0x{:08x}, 0x{:08x}>".format(self.v ^ self.m, self.v, self.m)


a = Masked(0x2307889e, 0x67cbf536)
b = Masked(0x54cb227c, 0x67cbbb2d)
c = a | b
print(c)


sys.exit(0)


for i in range(16):
	x = Masked(random.getrandbits(32), -1)
	y = Masked(random.getrandbits(32), -1)
	z = x | y
	print("x = {}, y = {}, z = {} => ".format(x, y, z), end = '')
	if z.unmask() == (x.unmask() | y.unmask()):
		print("ok")
	else:
		print("ERROR!")
