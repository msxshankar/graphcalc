#!/bin/sh

# Change directory to repository root
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT" || exit 1

# Performs release build
cmakeRelease () {
    make clean && make
}

# Build release package
cmakeRelease

if [ "$1" = "debian" ]; then
    mkdir -p usr/bin
    cp graphcalc usr/bin/
    dpkg-deb --build . graphcalc.deb
    echo "Created graphcalc.deb successfully."
fi

