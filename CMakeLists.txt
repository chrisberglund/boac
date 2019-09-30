cmake_minimum_required(VERSION 3.15)
project(boac C)

set(CMAKE_C_STANDARD 99)

add_library(boac SHARED library.c library.h helpers.c helpers.h)