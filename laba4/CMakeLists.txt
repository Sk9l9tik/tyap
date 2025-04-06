cmake_minimum_required(VERSION 3.12)

set(project_name laba3)
project(${project_name})

set(SOURCES "mmain.cc")

set(HEADERS "Log.hh")

#compile options
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${project_name} ${SOURCES} ${HEADERS})