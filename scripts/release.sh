#!/bin/sh

# Performs release build
cmakeRelease () {
    make clean && make
}

# Packages for debian
debian () {
	dpkg-deb --build graphcalc

	mv graphcalc.deb graphcalc/
	cd graphcalc || return
}


# Build release package
cmakeRelease

if [ "$1" = "debian" ]; then

  # Creates usr/bin/ if there isn't already one
  # And copies over binary
  DIRECTORY=usr/bin/
  if [ ! -d "$DIRECTORY" ]; then
    mkdir -p usr/bin/
  fi
  cp graphcalc usr/bin/

  # Creates deb and returns to graphcalc directory
  cd ..
	debian
fi
