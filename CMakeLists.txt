cmake_minimum_required(VERSION 2.8.12.2)

project(hpc22)

#set(CMAKE_PREFIX_PATH /usr/bin/mpiexec)

# fuck it set c compiler to c++ compiler
set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpic++)

#set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
add_compile_options(-Wall -Wextra -pedantic)

# add_subdirectory(hw02/task1/sequential)
# add_subdirectory(hw02/task1/parallel)
# add_subdirectory(hw02/task2)
# add_subdirectory(hw03/sequential)
# add_subdirectory(hw03/parallel)
add_subdirectory(hw05/1D_heatstencil)
