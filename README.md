# Micro-Architectural Power Simulator (MAPS) for Cortex-M3

## Overview

Fast Cortex-M3 simulator that creates power traces. More information can be found in https://eprint.iacr.org/2017/1253.pdf

* written in C++ for speed
* reads and simulates a .bin file created from an assembly/C source with GNU arm toolchain 

Only instructions typically found in the coding of crypto primitives are supported.
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

The firmware may be compiled by any ARM compiler supporting the Cortex-M3. Only ARM gcc has been tested. The path to the ARM compiler executable may be changed in scripts/fw.mak by modifying the variable "DIR".

### Coding a new simulator

It is best to start and modify an already exisiting simulator. The simulator must contain 3 functions:

1. void check_sec_algo(void): this function applies some test vectors and prints wether the test passes or not.
2. void t_test_sec_algo(Options &options): this function runs the t_test by generating inputs and collecting traces
3. a wrapper to call the FW function (that will be simulated). This wrapper (whose signature depends on the FW function) must write the arguments in the simulator memory and set the processor registers accordingly. Then, it starts the simulation. After the simulation, it must copy the results from the simulated memory.

## Supporting more ARM v7-M instructions

Follow those steps to support for an instruction in the simulator:

1. Add the decoding and mask values in the file libsim/src/opcodes.h
2. Decode the instruction in the the function step() in libsim/src/cpu.cpp
3. Add the execution of the function in the same file
4. Don't forget to add this new function to the list of methods in cpu.h

The macros TEST_INS32 and TEST_INS16 simplify the decoding of the instruction
Also, don't forget to validate the behaviour of the new simulated instruction, especially the behaviour
of the pipeline registers reg_a and reg_b !

## Validations

Each instruction supported by the simulator must be validated against the RTL simulation. The RTL tree
is not stored in this repository because it belongs to ARM Limited. Most of the procedure described
below is only there for my own documentation.

1. Add the new instruction in the file experiment.c in the simulator tree
2. Execute make check > sim_trace.log 2>&1 in the fw build direction in the simulator tree
3. Add the new instruction in the file leakage.c in the RTL tree
4. Compile: make testcode TESTNAME=leakage
5. Simulate: make run TESTNAME=leakage
6. Convert the tarmac.log trace file into a register trace file: ../../../../../python/gen_trace.py > verilog_trace.log
7. Copy the register trace file in the simulator tree: cp ~/Documents/repos/maps/rendered/sse050/logical/testbench/execution_tb/verilog_trace.log .
8. Compare the simulator trace and the RTL trace. Either visually using gvim -d sim_trace.log verilog_trace.log, or using: ./../../python/compare_traces.py

## Bugs/limitations

Know limitations are:
* The pipeline for ldrb/strb instructions is more complex than what is implemented in the simulator. For example, for the following code:
```
ldrb r2, [r0]
strb r2, [r0]
```
reg_a and reg_b will not be simulated correctly by the simulator. The functionality is still correct though.
When an other instruction is inserted between the ldrb and the strb instructions, the simulation is correct.
