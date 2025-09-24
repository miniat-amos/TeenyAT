cmake_minimum_required(VERSION 3.10)
project(teenyat C CXX)

if(MSVC)
    message(FATAL_ERROR
            "MSVC detected as the compiler, which is not supported.\n"
            "Please reconfigure with CMake to use GCC/G++ or Clang.\n"
            "Once you've installed one of these compiler suites, the\n"
            "easiest way to do this on Windows is to run the\n"
            "build.bat file in the TeenyAT root directory.")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# TODO: We cannot disable C extensions until we eliminate
#       clock_gettime() and CLOCK_MONOTONIC.  See below.
#set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin")

set(WARNING_OPTIONS -Wall -Wextra -Wpedantic)

add_library(teenyat STATIC teenyat.c)
target_compile_options(teenyat PRIVATE ${WARNING_OPTIONS})
target_include_directories(teenyat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(teenyat_d SHARED teenyat.c)
set_target_properties(teenyat_d PROPERTIES OUTPUT_NAME teenyat_d)
target_compile_options(teenyat_d PRIVATE ${WARNING_OPTIONS})
target_include_directories(teenyat_d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(COPY teenyat.h DESTINATION "${CMAKE_BINARY_DIR}/out/include")

add_subdirectory(tnasm)
add_subdirectory(lcd)
add_subdirectory(edison)
