if (APPLE)
    add_compile_options(-O3 -ffast-math)
else ()
    add_compile_options(-O3 -ffast-math -g3 -msse4.1 -march=native -mtune=native)
endif ()

find_package(MPI REQUIRED)

link_directories(../../../boost/lib)
include_directories(../../../boost/include)

add_executable(main mandelbrot_seq.cpp stb_image_write.h hsv2rgbSSE.h)
#add_executable(8-mandel-par mandelbrot_par.c stb_image_write.h)




