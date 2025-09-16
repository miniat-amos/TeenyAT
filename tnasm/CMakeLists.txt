cmake_minimum_required(VERSION 3.10)
project(tnasm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin")

add_executable(tnasm
    parser.cpp
    lexer.cpp
    listing.cpp
    warnings.cpp
)

target_compile_options(tnasm PRIVATE -Wall -Wextra -Wpedantic)
