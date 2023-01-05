#!/bin/sh

# Performs release build
cmakeRelease () {
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S . -B cmake-build-release
}

# Packages for debian
debian () {
	dpkg-deb --build graphcalc

	mv graphcalc.deb graphcalc/
  cd graphcalc || return


# Build release package
cmakeRelease

if [ "$1" = "debian" ]; then

  # Creates usr/bin/ if there isn't already one
  # And copies over binary
  DIRECTORY=usr/bin/
  if [ ! -d "$DIRECTORY" ]; then
    mkdir -p usr/bin/
  fi
  cp cmake-build-release/graphcalc usr/bin/

  # Creates deb and returns to graphcalc directory
  cd ..
	debian
fi
