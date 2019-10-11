#!/bin/bash
gcc -std=gnu99 -c -fPIC -o filter.o filter.c
gcc -std=gnu99 -c -fPIC -o boa.o boa.c
gcc -std=gnu99 -c -fPIC -o helpers.o helpers.c
gcc -std=gnu99 -c -fPIC -o prefilter.o prefilter.c
gcc -std=gnu99 -c -fPIC -o sobel.o sobel.c
gcc -std=gnu99 -c -fPIC -o threshold.o threshold.c
gcc -shared -fPIC -o boa.so boa.o filter.o helpers.o prefilter.o sobel.o threshold.o

