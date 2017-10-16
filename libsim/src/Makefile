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

INSTALL_DIR := ../../install

CFLAGS := -std=gnu++11 -O3 -I../src -Wall

%.o: %.cpp
	g++ -c $(CFLAGS) -o $@ $<

%.a : %.o
	ar rvs $@ $^

.PHONY: all
all: libsim.a

.PHONY: clean
clean:
	/bin/rm -f *.o *.a

.PHONY: cleaninstall
cleaninstall:
	/bin/rm -f $(INSTALL_DIR)/include/*.h
	/bin/rm -f $(INSTALL_DIR)/lib/*.a

.PHONY: install
install: libsim.a
	cp libsim.a $(INSTALL_DIR)/lib
	cp ../src/cpu.h $(INSTALL_DIR)/include
	cp ../src/register.h $(INSTALL_DIR)/include
	cp ../src/alu.h $(INSTALL_DIR)/include
	cp ../src/tracer.h $(INSTALL_DIR)/include
	cp ../src/memory.h $(INSTALL_DIR)/include
	cp ../src/npy.h $(INSTALL_DIR)/include
	cp ../src/t_test.h $(INSTALL_DIR)/include
	cp ../src/progress_bar.h $(INSTALL_DIR)/include
	cp ../src/sim_sec_algo.h $(INSTALL_DIR)/include

libsim.a: utils.o tracer.o register.o memory.o alu.o cpu.o t_test.o npy.o progress_bar.o sim_sec_algo.o
	ar rvs $@ $^

