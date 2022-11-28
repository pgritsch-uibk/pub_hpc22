set(SRC_HEADER NBody.hpp Vector3D.hpp Particle.hpp Octree/Octree.h Octree/OctreeNodePool.h)
set(SRC_SOURCE NBody.cpp Octree/Octree.cpp Octree/OctreeNodePool.h)

option(COMPILE_WITH_GRAPHICS "Compiles the graphical User Interface" ON)
option(PERFORMANCE_TEST "Compiles for performance test" OFF)

if (PERFORMANCE_TEST)
    set(COMPILE_WITH_GRAPHICS OFF)
    add_compile_options(-DPERFORMANCE_TEST)
endif ()

if (COMPILE_WITH_GRAPHICS)
    include(FetchContent)

    # raylib
    find_package(raylib QUIET)
    if (NOT raylib_FOUND)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 4.2.0
        )
        FetchContent_MakeAvailable(raylib)
    endif()

    # raylib-cpp
    find_package(raylib_cpp QUIET)
    if (NOT raylib_cpp_FOUND)
        FetchContent_Declare(
            raylib_cpp
            GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
            GIT_TAG v4.2.6
        )
        FetchContent_MakeAvailable(raylib_cpp)
    endif()
    set(SRC_HEADER ${SRC_HEADER} Game.hpp Utility.hpp Sphere.hpp Light.hpp)
    set(SRC_SOURCE ${SRC_SOURCE} main_graphic.cpp Game.cpp Sphere.cpp Light.cpp)
else()
    set(SRC_HEADER ${SRC_HEADER})
    set(SRC_SOURCE ${SRC_SOURCE} main_no_graphic.cpp Octree/OctreeNodePool.cpp Octree/OctreeNodePool.h)
endif ()

if (APPLE)
    add_compile_options(-O2)
else()
    add_compile_options(-O0 -g -march=native -mtune=native)
endif()

# link_directories(../../boost/lib)
# include_directories(../../boost/include)

add_executable(main ${SRC_HEADER} ${SRC_SOURCE})

if (COMPILE_WITH_GRAPHICS)
    target_link_libraries(main PUBLIC raylib raylib_cpp)

    # create shader folder
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:main>/shader)

    # copy shaders to build directory only if shader files are newer than the ones in the build directory
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/*
        $<TARGET_FILE_DIR:main>/shader/)
endif()



# target_link_libraries(main boost_mpi boost_serialization)
