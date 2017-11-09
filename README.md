#arm_v7_m leakage simulator

## Overview

Fast arm_v7m simulator. Can be use to check assembly secure implementations for leakage.

* written in C++ for speed
* reads and simulates a .bin file created from an assembly/C source with GNU arm toolchain 

Only instructions used typically in the coding of crypto primitives are supported.
Unsupported instructions might be added in cpu.cpp.

## Compiling

1. Compile simulator:
  The simulator (including) the main function, is stored in a static library.
  1. cd libsim/build
  2. make
  3. make install
2. Compile an implementation firmware. We use sec_add_v05 as an example:
  1. cd sec_add_v05/fw/build
  2. make
3. Compile an implementation simulator (still using sec_add_v05 as an example):
  1. ccd sec_add_v05/sim/build
  2. make

### Using the simulator

The simulator read a .bin file that must be located in the current directory. The name of the
.bin file depends on what was specified in the simulator sources.

The option '-h' shows the accepted options and parameters

### Coding a new fw implementation

A FW implementation is simply a C function (possibly containing assembly code), following the ARM ABI (1st parameter in r0, etc ...)
There is no main function. All C and pre-processor functionalities may be used.

### Coding a new simulator

It is best to start and modify an already exisiting simulator. The simulator must contain 3 functions:

1. void check_sec_algo(void): this function applies some test vectors and prints wether the test passes or not.
2. void t_test_sec_algo(Options &options): this function runs the t_test by generating inputs and collecting traces
3. a wrapper to call the FW function (that will be simulated). This wrapper (whose signature depends on the FW function) must write the arguments in the simulator memory and set the processor registers accordingly. Then, it starts the simulation. After the simulation, it must copy the results from the simulated memory.
TEST