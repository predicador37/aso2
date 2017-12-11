cmake_minimum_required(VERSION 3.9)
project(aso2 C)

set(CMAKE_C_STANDARD 99)

add_executable(aso2 main.c libsem.h libsem.c)