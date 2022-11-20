if (APPLE)
    add_compile_options(-O3)
else()
    add_compile_options(-O3 -march=native -mtune=native)
endif()

add_executable(heat_stencil_1D_par_non_blocking_task_2 heat_stencil_1D_par_non_blocking.c)
add_executable(heat_stencil_1D_par_task_2 heat_stencil_1D_par.c)
add_executable(heat_stencil_1D_seq_task_2 heat_stencil_1D_seq.c)
