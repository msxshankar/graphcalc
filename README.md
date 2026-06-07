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

## Building with Make

Alternatively, you can build the project directly using the provided `Makefile`:

```bash
make clean && make
```

And run:

```bash
./graphcalc
```

## Running E2E Tests

To run the complete suite of E2E verification tests:

```bash
python3 tests/e2e_test.py
```

## Packaging and Release

To build the release binary and package it as a Debian package:

```bash
bash scripts/release.sh debian
```

The resulting package `graphcalc.deb` will be generated in the repository root.

## Features & Optimizations

* **TUI Scrolling:** Vertical scrolling for template descriptions in the TUI menu box using the `[` and `]` keys.
* **Full-Screen Responsive Renderer:** The terminal simulation plotter dynamically scales to match the size of your terminal window.
* **Math Parser Optimizations:** Numeric values are pre-parsed on tokenization, removing `std::stod` from the evaluation hot path.
* **OpenGL Batching:** Optimized 3D wireframe render loops to draw via batched lines, drastically decreasing GPU driver state validation overhead.
