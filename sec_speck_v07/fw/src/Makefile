
N_MEASURE ?= 100000

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
	-g

%.o: %.c arm_macros.h
	$(GCC) -c $(CFLAGS) -o $@ $< && $(OBJDUMP) -D $@ > $(addsuffix .lst, $(basename $@))

%.bin: %.o
	$(OBJCOPY) -O binary $< $@

%.npy %.trace_index: %.bin
	../../sim/build/sim_sec_speck_v07.exe -n $(N_MEASURE) -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PRECIOUS: %.o

.PHONY: all
all: sec_speck_v07.bin

.PHONY: clean
clean:
	/bin/rm -f *.o *.lst *.bin *.npy *.trace_index

.PHONY: sim
sim: sec_speck_v07.npy

.PHONY: debug
debug: sec_speck_v07.npy sec_speck_v07.trace_index
	../../../python/draw_t_test.py $< --trace_index=$(addsuffix .trace_index, $(basename $<))

.PHONY: data
data: ../../data/sec_speck_v07.npy

../../data/sec_speck_v07.npy: sec_speck_v07.bin
	time -p ../../sim/build/sim_sec_speck_v07.exe -n 1000000 -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PHONY: cleandata
cleandata:
	/bin/rm -f ../../data/sec_speck_v07.npy ../../data/sec_speck_v07.trace_index
