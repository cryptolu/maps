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

N_MEASURE ?= 10000

vpath %.c ../src
vpath %.h ../src

DIR := /home/yann/tools/launchpad/current/bin
GCC := $(DIR)/arm-none-eabi-gcc
OBJDUMP:= $(DIR)/arm-none-eabi-objdump
OBJCOPY:= $(DIR)/arm-none-eabi-objcopy

CFLAGS := \
	-mcpu=cortex-m3 \
	-mthumb \
	-march=armv7-m \
    -fno-exceptions \
	-fomit-frame-pointer \
	-I../src \
	-O3 \
	-g

%.o: %.c
	$(GCC) -c $(CFLAGS) -o $@ $< && $(OBJDUMP) -D -M reg-names-raw $@ > $(addsuffix .lst, $(basename $@))

%.bin: %.o
	$(OBJCOPY) -O binary $< $@

%.npy %.trace_index: %.bin
	../../sim/build/sim_sec_rectangle_v02.exe -n $(N_MEASURE) -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PRECIOUS: %.o

.PHONY: all
all: sec_rectangle_v02.bin

.PHONY: clean
clean:
	/bin/rm -f *.o *.lst *.bin *.npy *.trace_index

.PHONY: sim
sim: sec_rectangle_v02.npy

.PHONY: debug
debug: sec_rectangle_v02.npy sec_rectangle_v02.trace_index
	../../../python/draw_t_test.py $< --trace_index=$(addsuffix .trace_index, $(basename $<))

.PHONY: data
data: ../../data/sec_rectangle_v02.npy

../../data/sec_rectangle_v02.npy: sec_rectangle_v02.bin
	time -p ../../sim/build/sim_sec_rectangle_v02.exe -n 1000000 -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PHONY: cleandata
cleandata:
	/bin/rm -f ../../data/sec_rectangle_v02.npy ../../data/sec_rectangle_v02.trace_index
