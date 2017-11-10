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
################################################################################
#
#
#
################################################################################

vpath %.cpp ../src

LIB_DIR := ../../../install/lib
INC_DIR := ../../../install/include

CFLAGS := \
	-std=gnu++11 \
	-O3 \
	-Wall \
	-I../src -I$(INC_DIR)

LDFLAGS := \
	-L$(LIB_DIR)

LIBS := -lsim

%.o: %.cpp
	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all
all: sim_sec_speck_v12.exe

.PHONY: clean
clean:
	/bin/rm -f *.o *.exe *.npy

.PHONY: .FORCE

.PHONY: data
data: ../../data/sec_speck_v12.npy

 ../../data/sec_speck_v12.npy: sim_sec_speck_v12.exe .FORCE
	./$< -n 1000000 -o $@

sim_sec_speck_v12.exe: sec_speck_v12.o
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)
