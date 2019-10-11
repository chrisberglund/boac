#!/bin/bash
gcc -c -fPIC -o filter.o filter.c
gcc -c -fPIC -o boa.o boa.c
gcc -c -fPIC -o helpers.o helpers.c
gcc -c -fPIC -o prefilter.o prefilter.c
gcc -c -fPIC -o sobel.o sobel.c
gcc -c -fPIC -o threshold.o threshold.c
gcc -shared -fPIC -o boa.so boa.o filter.o helpers.o prefilter.o sobel.o threshold.o

