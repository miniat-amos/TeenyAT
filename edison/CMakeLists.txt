cmake_minimum_required(VERSION 3.10)
project(edison LANGUAGES C CXX)

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


#-------------- Asset generation ----------------

add_executable(generate_assets generate_assets.cpp)
set_property(TARGET generate_assets PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/temp")

# Define the files created by the asset generator
set(ASSET_OUTPUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/bin_assets.h")
set(ASSET_OUTPUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/bin_assets.cpp")

file(GLOB ASSET_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/exp_board_images/*.png")

# Define the command to run the generator
add_custom_command(
    OUTPUT ${ASSET_OUTPUT_HEADER} ${ASSET_OUTPUT_SOURCE}
    COMMAND $<TARGET_FILE:generate_assets> ${ASSET_IMAGES}
    DEPENDS generate_assets ${ASSET_IMAGES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BIN_DIR}
)


#-------------- Edison build  ----------------

add_executable(edison
    audio.c
    tigr.c

    ${ASSET_OUTPUT_SOURCE}  # the generated cpp version of board images
    edison_board.cpp
    lcd_screen.cpp
    leds.cpp
    main.cpp
    segment.cpp
    utils.cpp
)

set_property(TARGET edison PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_include_directories(edison PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # for generated bin_assets.h
)

# Using -O2 instead of -O3 to reduce code size and avoid issues with
# stronger optimizations failing with miniaudio (audio.c)
target_compile_options(edison PRIVATE $<$<NOT:$<CONFIG:Debug>>:-O2>)

# tigr.c generates lots of warnings with -Wall -Wextra -Wpedantic,
# so set these options only for all other source files.
set(WARNING_OPTIONS -Wall -Wextra -Wpedantic)
set(WARNING_SOURCES
    #audio.c
    ${ASSET_OUTPUT_SOURCE}  # the generated cpp version of board images
    edison_board.cpp
    lcd_screen.cpp
    leds.cpp
    main.cpp
    segment.cpp
    utils.cpp
)
set_property(SOURCE ${WARNING_SOURCES} PROPERTY COMPILE_OPTIONS ${WARNING_OPTIONS})

target_link_libraries(edison PRIVATE teenyat)

if(WIN32)
    target_link_libraries(edison PRIVATE opengl32 gdi32)
elseif(APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(Cocoa_LIBRARY Cocoa)
    target_link_libraries(edison PRIVATE ${OpenGL_LIBRARY} ${Cocoa_LIBRARY})
elseif(UNIX)
    target_link_libraries(edison PRIVATE GLU GL X11 dl pthread)
endif()
