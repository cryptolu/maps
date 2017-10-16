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

N_ROUND = 25

def rol16(x, n):
	return ((x << n) | (x >> (16 - n))) & 0xffff

def p_layer(data):
	data[1] = rol16(data[1], 1)
	data[2] = rol16(data[2], 12)
	data[3] = rol16(data[3], 13)

def s_layer(data):
   temp0 = data[1] ^ 0xffff;
   temp1 = data[0] & temp0;
   temp2 = data[2] ^ data[3];
   temp3 = temp1 ^ temp2;
   temp1 = data[3] | temp0;
   temp0 = data[0] ^ temp1;
   temp1 = data[2] ^ temp0;
   temp4 = data[1] ^ data[2];
   data[3] = temp2 & temp0;
   data[3] = data[3] ^ temp4;
   data[2] = temp3 | temp4;
   data[2] = data[2] ^ temp0;
   data[0] = temp3;
   data[1] = temp1;

def round_function(data, rkey):
	print("data = ", end = '')
	disp(data)
	print("rkey = ", end = '')
	disp(rkey)
	for i in range(4):
		data[i] ^= rkey[i]
	print("xor  = ", end = '')
	disp(data)
	s_layer(data)
	p_layer(data)

def encrypt(block, round_keys):
	for i in range(N_ROUND):
		print("-- round {}".format(i))
		round_function(block, round_keys[i])
	for i in range(4):
		block[i] ^= round_keys[N_ROUND][i]

def disp(data):
	for i in range(4):
		print("0x{:04x} ".format(data[i]), end = '')
	print("")

def cmp(l, r):
	for i in range(4):
		if l[i] != r[i]:
			return false
	else:
		return true

rk = (
	(0xffff, 0xffff, 0xffff, 0xffff), #0
	(0x01ff, 0x00ff, 0xff00, 0x00ff), #1
	(0x0301, 0x0100, 0xfeff, 0xfeff), #2
	(0xf802, 0x02ff, 0x0100, 0x0201), #3
	(0x0f04, 0xfc00, 0x02fe, 0xfb02), #4
	(0xe5fa, 0x0afe, 0xf2fd, 0xfa04), #5
	(0xee13, 0xe7fd, 0x04fa, 0xedfa), #6
	(0xe51c, 0xf0fa, 0xf906, 0xe113), #7
	(0x1de7, 0x1706, 0xf7ea, 0x1d1c), #8
	(0x00e4, 0x02ea, 0x11e3, 0xe2e7), #9
	(0x0919, 0x0ce3, 0x05f5, 0xf3e4), #10
	(0x0214, 0x05f5, 0xf01c, 0xf719), #11
	(0xedf0, 0x081c, 0xe412, 0x0514), #12
	(0xf518, 0xf612, 0x11e6, 0x04f0), #13
	(0xe206, 0x0fe6, 0xfd05, 0x1418), #14
	(0xe3ef, 0xed05, 0xe2fa, 0x0906), #15
	(0x16ec, 0x17fa, 0xee17, 0xe9ef), #16
	(0xe7fd, 0x0017, 0x0c03, 0x07ec), #17
	(0xebfb, 0xf403, 0x08eb, 0xecfd), #18
	(0x06ec, 0xfbeb, 0xf310, 0xeffb), #19
	(0x14f3, 0xf110, 0x1efe, 0x18ec), #20
	(0x0312, 0xe5fe, 0x13f5, 0x02f3), #21
	(0xf3ed, 0xeef5, 0xf505, 0x1312), #22
	(0xf602, 0x1f05, 0xf2fd, 0xf7ed), #23
	(0xe7e0, 0xecfd, 0x1e0a, 0xe502), #24
	(0x0305, 0xea0a, 0xe20b, 0xf8e0)  #25
)
#p = [0xffff, 0xffff, 0xffff, 0xffff]
p = [0x1234, 0x5678, 0x9a0b, 0xcdef]
expected_c = [0x3ee8, 0xeeef, 0x154a, 0x467a]

disp(p)
encrypt(p, rk)
disp(p)
if (expected_c == p):
	print("ok")
else:
	print("ERROR!")
