find_package(MPI REQUIRED)

add_compile_options(-O3 -march=native -mtune=native)

add_executable(3_heat_stencil_2d_par heat_stencil_2d.cpp ../Matrix2D.h ../Matrix2D.cpp)

target_link_libraries(3_heat_stencil_2d_par PUBLIC MPI::MPI_CXX)