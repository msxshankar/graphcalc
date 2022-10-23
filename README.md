# graphcalc

WIP

# Introduction
Graphcalc is a personal University project that I am working on. It combines skills from various modules that I am doing in my second year.
Its aim is to map complex graphs and find various statistics about them.

## Key Goals
* A simple and clear to use interface 
* Efficient memory and resource management
* Modular and extendable

# Installation

## Linux

Make sure you have these dependencies installed:
* CMake
* Ninja

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
