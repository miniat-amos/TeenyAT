cmake_minimum_required(VERSION 3.10)
project(teenyat C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin")

add_library(teenyat STATIC teenyat.c)

add_library(teenyat_d SHARED teenyat.c)
set_target_properties(teenyat_d PROPERTIES OUTPUT_NAME teenyat_d)

file(COPY teenyat.h DESTINATION "${CMAKE_BINARY_DIR}/out/include")
