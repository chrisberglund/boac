#!/bin/bash
gcc -std=gnu99 -c -fPIC -pthread -o filter.o filter.c
gcc -std=gnu99 -c -fPIC -pthread -o boa.o boa.c
gcc -std=gnu99 -c -fPIC -pthread -o helpers.o helpers.c
gcc -std=gnu99 -c -fPIC -pthread -o prefilter.o prefilter.c
gcc -std=gnu99 -c -fPIC -pthread -o sobel.o sobel.c
gcc -std=gnu99 -c -fPIC -pthread -o quicksort.o quicksort.c
gcc -std=gnu99 -c -fPIC -pthread -o threshold.o threshold.c
gcc -shared -fPIC -pthread -o boa.so boa.o filter.o helpers.o prefilter.o sobel.o quicksort.o threshold.o

