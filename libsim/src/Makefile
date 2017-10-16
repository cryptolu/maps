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

