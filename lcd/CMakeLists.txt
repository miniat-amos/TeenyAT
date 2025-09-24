cmake_minimum_required(VERSION 3.10)
project(lcd LANGUAGES C CXX)

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
# TODO: We cannot disable C extensions unless we eliminate
#       strdup().  See below.
#set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: We cannot disable extensions for C until we eliminate
#       strdup() as it is not standard and require _POSIX_C_SOURCE 200809L.
#       Once this is resolved, we can eliminate this entire section below.
set(CMAKE_REQUIRED_DEFINITIONS -D_POSIX_C_SOURCE=200809L)
include(CheckSymbolExists)
check_symbol_exists(strdup "string.h" HAVE_STRDUP)
if(NOT HAVE_STRDUP)
    message(FATAL_ERROR 
            "strdup()is required but not found.\n"
            "Consider forcing CMake to use GCC/G++ or Clang/Clang++ compilers."
    )
endif()
# END TODO

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin")

add_executable(lcd
    tigr.c

    color.cpp
    main.cpp
    screen.cpp
    util.cpp
)

target_include_directories(lcd PRIVATE ${CMAKE_SOURCE_DIR})

target_compile_options(lcd PRIVATE $<$<NOT:$<CONFIG:Debug>>:-O3>)

# tigr.c generates lots of warnings with -Wall -Wextra -Wpedantic,
# so set these options only for all other source files.
set(WARNING_OPTIONS -Wall -Wextra -Wpedantic)
set(WARNING_SOURCES color.cpp main.cpp screen.cpp util.cpp)
set_property(SOURCE ${WARNING_SOURCES} PROPERTY COMPILE_OPTIONS ${WARNING_OPTIONS})

target_link_libraries(lcd PRIVATE teenyat)

if(WIN32)
    target_link_libraries(lcd PRIVATE opengl32 gdi32)
elseif(APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(Cocoa_LIBRARY Cocoa)
    target_link_libraries(lcd PRIVATE ${OpenGL_LIBRARY} ${Cocoa_LIBRARY})
elseif(UNIX)
    target_link_libraries(lcd PRIVATE GLU GL X11)
endif()
