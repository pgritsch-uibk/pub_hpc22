if (APPLE)
    add_compile_options(-O2)
else ()
    add_compile_options(-O2 -march=native -mtune=native)
endif ()


add_executable(5_heat_stencil_2d_par_nb heat_stencil_2d_nonblocking.cpp Matrix2D.h Matrix2D.cpp MPIMatrix2D.h MPIMatrix2D.cpp Location.h heat_stencil_common_2d.h)
add_executable(5_heat_stencil_2d_par_sr heat_stencil_2d_sendrecv.cpp Matrix2D.h Matrix2D.cpp MPIMatrix2D.h MPIMatrix2D.cpp Location.h heat_stencil_common_2d.h)
add_executable(5_heat_stencil_2d_seq heat_stencil_2d_seq.cpp Matrix2D.h Matrix2D.cpp heat_stencil_common_2d.h)
