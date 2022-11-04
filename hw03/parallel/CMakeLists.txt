if (APPLE)
    add_compile_options(-O3)
else()
    add_compile_options(-O3 -march=native -mtune=native)
endif()

add_executable(3_heat_stencil_2d_par heat_stencil_2d.cpp ../Matrix2D.h ../Matrix2D.cpp ../MpiConfig.h)