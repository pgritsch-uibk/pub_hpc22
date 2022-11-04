find_package(MPI REQUIRED)

if (APPLE)
    add_compile_options(-O3)
else()
    add_compile_options(-march=native -mtune=native)
endif()

add_executable(3_heat_stencil_2d_par heat_stencil_2d.cpp ../Matrix2D.h ../Matrix2D.cpp ../MPIVectorConfig.h)

# target_link_libraries(3_heat_stencil_2d_par PUBLIC MPI::MPI_CXX)