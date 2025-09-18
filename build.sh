#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e
pushd . > /dev/null # backup the inital directory

# Default build directory and type
BUILD_DIR="build"
BUILD_TYPE="Release"

# Handle command line arguments
if [ "$1" == "debug" ]; then
    #BUILD_DIR="build_debug"  # unsure if we want separate directories for this
    BUILD_TYPE="Debug"
    echo "Configuring for Debug build..."
elif [ "$1" == "clean" ]; then
    # again, unsure if we want separate directories for this
    #echo "Cleaning build and build_debug directories..."
    echo "Cleaning build directories..."
    rm -rf build  # could delete build_debug if we used it
    echo ""
    echo "Clean complete."
    echo ""
    BUILD_EXIT=1
elif [ "$1" == "" ]; then
    echo "Using default build configuration..."
else
    echo ""
    echo "Invalid parameter: $1"
    echo "Usage: ./build.sh [debug | clean]"
    echo "    No parameter - Default Release build"
    echo "    debug - Build with debug symbols"
    echo "    clean - Remove build directory"
    echo ""
    BUILD_EXIT=1
fi

if [ -z "$BUILD_EXIT" ]; then
    # Create the build directory if it doesn't exist
    if [ ! -d "$BUILD_DIR" ]; then
        echo "Creating directory: $BUILD_DIR"
        mkdir "$BUILD_DIR"
    fi

    cd "$BUILD_DIR"
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
    cmake --build .

    popd > /dev/null  # restore the initial directory

    echo ""
    echo "Build complete!"
    echo ""


    OS_NAME=$(uname -s)

    if [ "$OS_NAME" = "Linux" ]; then
        # Linux
        :
    elif [ "$OS_NAME" = "Darwin" ]; then
        # macOS
        :
    else
        # Windows... probably shouldn't here
        :
    fi

    set +e
fi