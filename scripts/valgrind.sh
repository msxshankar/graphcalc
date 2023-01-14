#!/bin/bash

valgrind () {
	cd cmake-build-debug/
	valgrind --leak-check=full ./graphcalc
}

valgrind
