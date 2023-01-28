# graphcalc

WIP

# Introduction
Graphcalc is a personal University project that I am working on. It combines skills from various modules that I am doing in my second year.
Its aim is to map graphs and find various statistics about them.

## Key Goals
* A simple terminal interface 
* Efficient memory and resource management
* Modular and extendable

# Installation

## Linux

Make sure you have these dependencies installed:
* CMake
* Ninja
* Gnuplot 

Then clone the repository
```
git clone https://github.com/msxshankar/graphcalc
cd graphcalc
```

Build using CMake and Ninja
```
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S . -B build
cd build && ninja
```
Finally, run the program
```
./graphcalc
```

# Testing
Unit tests is currently being implemented via CTest. You can run them as follows:

Go into the cmake build directory, then run ctest
```
cd cmake-build-debug/
ctest
```

Some useful flags are listed below
```
ctest --verbose (print more details)
ctest --parallel (run tests in parallel)
```