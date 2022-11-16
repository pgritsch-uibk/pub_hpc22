if (ENV{USER} STREQUAL "philipp")
    FIND_PACKAGE(Boost REQUIRED)
endif ()

include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(5_heat_stencil_2d_par_nb LINK_PUBLIC ${Boost_LIBRARIES})