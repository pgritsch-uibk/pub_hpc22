add_executable(monte_carlo_seq monte_carlo_seq.c)

add_compile_options(-march=native -mtune=native)

add_definitions(-DSOURCEFOLDER="${CMAKE_CURRENT_SOURCE_DIR}")
