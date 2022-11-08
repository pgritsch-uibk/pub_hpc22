add_executable(monte_carlo_seq monte_carlo_seq.c)

if (APPLE)
    add_compile_options(-O3)
else()
    add_compile_options(-O3 -march=native -mtune=native)
endif()


add_definitions(-DSOURCEFOLDER="${CMAKE_CURRENT_SOURCE_DIR}")
