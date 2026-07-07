# Program Validation Tool

A C-based utility created during the second semester of my studies for the Programming 2 class, that automates the testing and validation of other C programs. It evaluates code based on compilation success, execution termination, output accuracy, and memory access. Linux system calls are utlized.

## Core Concepts & Features
* The tool automatically compiles the target source file and tracks compilation errors or warnings to adjust the final score.
* It executes the compiled program using arguments read from a specified text file and feeds it input from a standard input file.
* The testing framework enforces time limits using system timers and signals to ensure the target program terminates appropriately.
* It monitors the execution process to detect memory access issues, such as segmentation faults, and applies score penalties accordingly.
* A custom difference tool, p4diff, is used to compare the tested program's output to an expected output file byte-by-byte.
* The p4diff utility calculates a similarity percentage based on matching bytes, which contributes to the final output score.
* The program outputs a final score summary detailing the points awarded or deducted for compilation, termination, output, and memory access.

## Compilation and Execution
The project is configured with a Makefile for straightforward compilation using the gcc compiler.

### Building the Tool
* To compile the main testing program and the difference utility, use the command: `make hw4`.
* Alternatively, the difference utility can be compiled individually using the command: `make p4diff`.
* To remove the compiled executables, use the command: `make clean`.

### Running the Tool
* The main executable requires exactly five arguments to run correctly.
* These arguments must include the target C file, the arguments text file, the standard input text file, the expected output text file, and a timeout limit in seconds.