find_package(OpenMP REQUIRED)

if (APPLE)
    add_compile_options(-O3)
else()
    add_compile_options(-O3 -march=native -mtune=native)
endif()

add_executable(3_heat_stencil_2d_seq ../Matrix2D.h ../Matrix2D.cpp heat_stencil_2d.cpp)

target_link_libraries(3_heat_stencil_2d_seq PRIVATE OpenMP::OpenMP_C)