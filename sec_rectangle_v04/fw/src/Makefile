
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
	-g

%.o: %.c
	$(GCC) -c $(CFLAGS) -o $@ $< && $(OBJDUMP) -D -M reg-names-raw $@ > $(addsuffix .lst, $(basename $@))

%.bin: %.o
	$(OBJCOPY) -O binary $< $@

%.npy %.trace_index: %.bin
	../../sim/build/sim_sec_rectangle_v04.exe -n $(N_MEASURE) -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PRECIOUS: %.o

.PHONY: all
all: sec_rectangle_v04.bin

.PHONY: clean
clean:
	/bin/rm -f *.o *.lst *.bin *.npy *.trace_index

.PHONY: sim
sim: sec_rectangle_v04.npy

.PHONY: debug
debug: sec_rectangle_v04.npy sec_rectangle_v04.trace_index
	../../../python/draw_t_test.py $< --trace_index=$(addsuffix .trace_index, $(basename $<))

.PHONY: data
data: ../../data/sec_rectangle_v04.npy

../../data/sec_rectangle_v04.npy: sec_rectangle_v04.bin
	time -p ../../sim/build/sim_sec_rectangle_v04.exe -n 1000000 -o $@ -i $(addsuffix .trace_index, $(basename $@))

.PHONY: cleandata
cleandata:
	/bin/rm -f ../../data/sec_rectangle_v04.npy ../../data/sec_rectangle_v04.trace_index
