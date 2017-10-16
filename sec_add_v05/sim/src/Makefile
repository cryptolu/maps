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
all: sim_sec_add_v05.exe

.PHONY: clean
clean:
	/bin/rm -f *.o *.exe *.npy

.PHONY: .FORCE

.PHONY: data
data: ../../data/sec_add_v05.npy 

 ../../data/sec_add_v05.npy: sim_sec_add_v05.exe .FORCE
	./$< -n 1000000 -o $@

sim_sec_add_v05.exe: sec_add_v05.o
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)
