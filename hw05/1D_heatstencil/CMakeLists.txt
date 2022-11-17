if (APPLE)
    add_compile_options(-O2)
else()
    add_compile_options(-O2 -march=native -mtune=native)
endif()

find_package(MPI REQUIRED)

link_directories(../../../boost/lib)
include_directories(../../../boost/include)

add_executable(heat_stencil_1D_seq ./blocking/heat_stencil_1D_seq.cpp)
add_executable(heat_stencil_1D_par_blocking ./blocking/heat_stencil_1D_par.cpp)
add_executable(heat_stencil_1D_par_blocking_sendrecive ./blocking/heat_stencil_1D_par_send_recive.cpp)
add_executable(heat_stencil_1D_par_non_blocking ./non_blocking/heat_stencil_1D_par.cpp)


target_link_libraries(heat_stencil_1D_seq boost_mpi boost_serialization)
target_link_libraries(heat_stencil_1D_par_blocking boost_mpi boost_serialization)
target_link_libraries(heat_stencil_1D_par_blocking_sendrecive boost_mpi boost_serialization)
target_link_libraries(heat_stencil_1D_par_non_blocking boost_mpi boost_serialization)