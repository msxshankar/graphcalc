#!/bin/sh

# Performs release build
cmakeRelease () {
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S . -B cmake-build-release
}

# Packages for debian
debian () {
	dpkg-deb --build graphcalc
}

cmakeRelease

if [ "$1" = "debian" ]; then
    cd ..
	debian
	mv graphcalc.deb graphcalc/
    cd graphcalc
fi
