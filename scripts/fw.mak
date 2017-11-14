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
# Common Makefile fragment for firmware
#
# The variable TEST_NAME must be set before including this makefile fragment
# The variable N_MEASURE may be set if testbench requires a non-default value
#
################################################################################

N_MEASURE ?= 10000
FIRMWARE := $(TEST_NAME).bin
SIMULATOR := ../../sim/build/sim_$(TEST_NAME).exe
TRACE := $(TEST_NAME).npy

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
	-g -O3

%.o: %.c
	$(GCC) -c $(CFLAGS) -o $@ $< && $(OBJDUMP) -D $@ > $(addsuffix .lst, $(basename $@))

%.bin: %.o
	$(OBJCOPY) -O binary $< $@

%.npy %.trace_index: %.bin
	$(SIMULATOR) -n $(N_MEASURE) -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PRECIOUS: %.o

.PHONY: all
all: $(FIRMWARE)

.PHONY: clean
clean:
	/bin/rm -f *.o *.lst *.bin *.npy *.trace_index

.PHONY: check
check: $(FIRMWARE)
	$(SIMULATOR) -t

.PHONY: sim
sim: experiment.npy

.PHONY: debug
debug: $(TEST_NAME).npy $(TEST_NAME).trace_index
	../../../python/draw_t_test.py $(TRACE) --trace_index=$(addsuffix .trace_index, $(basename $<))

.PHONY: data
data: ../../data/$(TRACE)

../../data/$(TRACE): $(FIRMWARE)
	time -p $(SIMULATOR) -n 1000000 -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PHONY: cleandata
cleandata:
	/bin/rm -f ../../data/$(TRACE) ../../data/$(TEST_NAME).trace_index
