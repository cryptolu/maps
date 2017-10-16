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
all: sim_sec_add_v06.exe

.PHONY: clean
clean:
	/bin/rm -f *.o *.exe *.npy

.PHONY: .FORCE

.PHONY: data
data: ../../data/sec_add_v06.npy 

 ../../data/sec_add_v06.npy: sim_sec_add_v06.exe .FORCE
	./$< -n 1000000 -o $@

sim_sec_add_v06.exe: sec_add_v06.o
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)
