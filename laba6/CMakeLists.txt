cmake_minimum_required(VERSION 3.12)

set(project_name laba6)
project(${project_name})

set(SOURCES "mmain.cc")

set(HEADERS "Log.hh")

#compile options
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_compile_options(
    #-Werror

    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo

    -g
)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


add_executable(${project_name} ${SOURCES} ${HEADERS})