if (APPLE)
    add_compile_options(-O3 -ffast-math)
else()
    add_compile_options(-O3 -ffast-math -march=native -mtune=native)
endif()

find_package(MPI REQUIRED)

link_directories(../../../boost/lib)
include_directories(../../../boost/include)

add_executable(main mandelbrot_seq.cpp stb_image_write.h)

#target_link_libraries(main boost_mpi boost_serialization)