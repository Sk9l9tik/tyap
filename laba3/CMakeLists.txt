cmake_minimum_required(VERSION 3.12)

set(project_name laba3)
project(${project_name})

set(SOURCES "mmain2.cc")

set(HEADERS "Log.hh")

#compile options
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


add_executable(${project_name} ${SOURCES} ${HEADERS})