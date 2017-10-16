#! /usr/bin/python



def rol(x, n):
	return ((x << n) | (x >> (32 - n))) & 0xffffffff

def ror(x, n):
	return ((x >> n) | (x << (32 - n))) & 0xffffffff


class Simon(object):
	Z_XOR_3 = (
		1 ^3, 1 ^3, 0 ^3, 1 ^3, 1 ^3, 0 ^3, 1 ^3, 1 ^3,
		1 ^3, 0 ^3, 1 ^3, 0 ^3, 1 ^3, 1 ^3, 0 ^3, 0 ^3,
		0 ^3, 1 ^3, 1 ^3, 0 ^3, 0 ^3, 1 ^3, 0 ^3, 1 ^3,
		1 ^3, 1 ^3, 1 ^3, 0 ^3, 0 ^3, 0 ^3, 0 ^3, 0 ^3,
		0 ^3, 1 ^3, 0 ^3, 0 ^3, 1 ^3, 0 ^3, 0 ^3, 0 ^3,
		1 ^3, 0 ^3, 1 ^3, 0 ^3
	)
	def __init__(self, n_round):
		self.n_round = n_round

	def key_schedule(self):
		""" k is a vector of 4 32-bit integers """
		self.rk = list(self.k)
		for i in range(4, self.n_round):
			tmp = ror(self.rk[i - 1], 3) ^ self.rk[i - 3]
			tmp ^= ror(tmp, 1)
			rk_ = (~(self.rk[i - 4]) & 0xffffffff) ^ tmp ^ self.Z_XOR_3[i - 4]
			self.rk.append(rk_)

	def set_key(self, key):
		self.k = key
		self.key_schedule()

	def one_round(self, l, r, rk):
		t = (rol(l, 1) & rol(l, 8)) ^ rol(l, 2) ^ r ^ rk
		r = l
		l = t
		return l, r

	def encrypt(self, l, r):
		l_, r_ = l, r
		for round_idx in range(self.n_round):
			l_, r_ = self.one_round(l_, r_, self.rk[round_idx])
		return l_, r_


if __name__ == '__main__':
	simon = Simon(44)
	simon.set_key((0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918))
	li, ri = 0x656b696c, 0x20646e75
	lo, ro = simon.encrypt(li, ri)
	print("input = 0x{:08x}{:08x}, output = 0x{:08x}{:08x}".format(li, ri, lo, ro))
