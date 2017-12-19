cmake_minimum_required(VERSION 3.9)
project(aso2 C)

set(CMAKE_C_STANDARD 99)

add_executable(aso2 libsem.h libsem.c main.c)