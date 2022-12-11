if (APPLE)
    add_compile_options(-O3 -ffast-math -g3 -march=armv8-a+fp+simd+crypto+crc)
else ()
    #add_compile_options(-g3 -g -msse4.1 -march=native -mtune=native)
    add_compile_options(-O3 -ffast-math -g3 -msse4.1 -march=native -mtune=native)
endif ()

find_package(MPI REQUIRED)

link_directories(../../../boost/lib)
include_directories(../../../boost/include)

add_executable(main_seq mandelbrot_seq.cpp stb_image_write.h hsv2rgbSSE.h)
add_executable(main_par mandelbrot_mpi_naive.cpp stb_image_write.h hsv2rgbSSE.h)
add_executable(main_par_dlb mandelbrot_mpi_dlb.cpp stb_image_write.h hsv2rgbSSE.h dlb_specifics/Image.h dlb_specifics/LoadBalancer.h)
#add_executable(8-mandel-par mandelbrot_par.c stb_image_write.h)




