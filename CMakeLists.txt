cmake_minimum_required(VERSION 3.9)

project(hpc22)

set(CMAKE_PREFIX_PATH /usr/bin/mpiexec)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory(hw02/task1/sequential)
add_subdirectory(hw02/task1/parallel)
add_subdirectory(hw03/sequential)
add_subdirectory(hw03/parallel)
