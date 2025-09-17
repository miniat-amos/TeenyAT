cmake_minimum_required(VERSION 3.10)
project(teenyat C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# TODO: We cannot disable C extensions until we eliminate
#       clock_gettime() and CLOCK_MONOTONIC.  See below.
#set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: We cannot disable extensions for C until we eliminate
#       clock_gettime() and CLOCK_MONOTONIC as they are not standard
#       and require _POSIX_C_SOURCE 200809L.  Once this is resolved,
#       we can eliminate this entire section below.
set(CMAKE_REQUIRED_DEFINITIONS -D_POSIX_C_SOURCE=200809L)
include(CheckSymbolExists)
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
check_symbol_exists(CLOCK_MONOTONIC "time.h" HAVE_CLOCK_MONOTONIC)
if(NOT HAVE_CLOCK_GETTIME OR NOT HAVE_CLOCK_MONOTONIC)
    message(FATAL_ERROR 
            "clock_gettime() and CLOCK_MONOTONIC are required but not found.\n"
            "Consider forcing CMake to use GCC/G++ or Clang/Clang++ compilers."
    )
endif()
# END TODO

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin")

add_library(teenyat STATIC teenyat.c)
target_compile_options(teenyat PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(teenyat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(teenyat_d SHARED teenyat.c)
set_target_properties(teenyat_d PROPERTIES OUTPUT_NAME teenyat_d)
target_compile_options(teenyat_d PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(teenyat_d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(COPY teenyat.h DESTINATION "${CMAKE_BINARY_DIR}/out/include")

add_subdirectory(tnasm)
add_subdirectory(lcd)
add_subdirectory(edison)
