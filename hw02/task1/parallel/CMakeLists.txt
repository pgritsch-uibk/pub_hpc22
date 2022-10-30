find_package(MPI REQUIRED)

add_compile_options(-O3 -msse4.1 -march=native -mtune=native)

add_executable(monte_carlo_mpi monte_carlo_mpi.c xorshiftSSE.h xorshiftSSE.c)

add_definitions(-DSOURCEFOLDER="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(monte_carlo_mpi PUBLIC MPI::MPI_C)
