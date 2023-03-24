# Various scripts to help with packaging and debugging

## release.sh

To build packages, run the script as shown below and add relevant targets in square brackets
First, make script executable
```
   chmod 755 release.sh
```

Then, to run
```
   cd graphcalc/
   . scripts/release.sh [debian]
```

## valgrind.sh

Simple script using valgrind to detect memory leaks. Requires valgrind to be installed
First, make script executable
```
   chmod 755 valgrind.sh
```

Then, to run
```
   cd graphcalc/
   . scripts/valgrind.sh
```
