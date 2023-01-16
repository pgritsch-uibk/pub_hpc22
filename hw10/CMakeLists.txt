if (APPLE)
    add_compile_options(-O3 -g3 -march=armv8)
else ()
    #add_compile_options(-g3 -g -msse4.1 -march=native -mtune=native)
    add_compile_options(-g3 -g -march=native -mtune=native)
endif ()

find_package(MPI REQUIRED)

link_directories(../../../boost/lib)
include_directories(../../../boost/include)

# add_executable(multiple_files_individual 1_mf_ip.cpp)
# add_executable(single_file_individual 1_sf_ip.cpp)
# add_executable(single_file_shared_nonc 1_sf_sp_nc.cpp)
# add_executable(single_file_shared_c 1_sf_sp_c.cpp)
add_executable(rank_0_communicator 2_rank_0_f.cpp)
#add_executable(8-mandel-par mandelbrot_par.c stb_image_write.h)




