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
# Common Makefile fragment for simulators
#
# The variable TEST_NAME must be set before including this makefile fragment
#
################################################################################

SIMULATOR := sim_$(TEST_NAME).exe

vpath %.cpp ../src

LIB_DIR := ../../../install/lib
INC_DIR := ../../../install/include

CFLAGS := -std=gnu++11 -O3 -Wall -I../src -I$(INC_DIR)
LDFLAGS := -L$(LIB_DIR)
LIBS := -lsim

%.o: %.cpp
	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all
all: $(SIMULATOR)

.PHONY: clean
clean:
	/bin/rm -f *.o *.exe *.npy

.PHONY: .FORCE

.PHONY: data
data: ../../data/$(TEST_NAME).npy

 ../../data/$(TEST_NAME).npy: $(SIMULATOR) .FORCE
	./$< -n 1000000 -o $@

$(SIMULATOR): $(TEST_NAME).o
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)
