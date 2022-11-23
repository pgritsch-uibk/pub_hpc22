if (ENV{USER} STREQUAL "philipp")
    FIND_PACKAGE(Boost REQUIRED)
endif ()


if (APPLE) 
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif ()

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
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
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v4.2.6
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

if (APPLE)
    add_compile_options(-O2)
else()
    add_compile_options(-O2 -march=native -mtune=native)
endif()


# link_directories(../../boost/lib)
# include_directories(../../boost/include)

add_executable(main main.cpp Game.hpp Game.cpp NBody.hpp NBody.cpp Particle.hpp Vector3D.hpp)
target_link_libraries(main PUBLIC raylib raylib_cpp)


# target_link_libraries(main boost_mpi boost_serialization)
