set (CMAKE_CXX_STANDARD 14)

cmake_minimum_required(VERSION 3.1)

project(compressor)

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB utils.cpp ari.cpp ppm.cpp)

add_library(utils STATIC ${SOURCE_LIB})

add_executable(compress ${SOURCE_EXE})

target_link_libraries(compress utils)
