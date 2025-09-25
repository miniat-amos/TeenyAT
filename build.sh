#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e
pushd . > /dev/null # backup the inital directory

print_usage() {
    echo ""
    echo "Usage: ./build.sh [OPTION]..."
    echo ""
    echo "Options:"
    echo "  <no option> Default release build"
    echo "    debug     Build with debug symbols"
    echo "    verbose   Enable verbose output during build"
    echo "    clean     Remove build directory (must be used alone)"
    echo ""
    echo "Examples:"
    echo "    ./build.sh                  # Release build"
    echo "    ./build.sh debug verbose    # Debug build with verbose output"
    echo "    ./build.sh clean            # Clean the project"
    echo ""
}

# Default build directory, type, and verbosity
BUILD_DIR="build"
BUILD_TYPE="Release"
VERBOSE_FLAG=""
ARGS_FOUND=()

for arg in "$@"; do
    for found_arg in "${ARGS_FOUND[@]}"; do
        if [ "$found_arg" == "$arg" ]; then
            echo -e "\nError: Argument '$arg' provided more than once."
            print_usage
            popd > /dev/null
            exit 1
        fi
    done
    ARGS_FOUND+=("$arg")

    case "$arg" in
        debug)
            if [ "$BUILD_TYPE" == "Release" ]; then
                BUILD_TYPE="Debug"
                echo "Configuring for Debug build..."
            else
                echo -e "\nError: 'debug' mode requested when mode was already set."
                print_usage
                popd > /dev/null
                exit 1
            fi
            ;;
        verbose)
            VERBOSE_FLAG="-- VERBOSE=1"
            echo "Enabling verbose output..."
            ;;
        clean)
            if [ $# != 1 ]; then
                echo -e "\nError: Cannot combine 'clean' with other arguments."
                print_usage
                popd > /dev/null
                exit 1
            fi
            echo "Cleaning build directories..."
            rm -rf build
            popd > /dev/null
            exit 0
            ;;
        *)
            echo -e "\nError: Invalid parameter, '$arg'"
            print_usage
            popd > /dev/null
            exit 1
            ;;
    esac
done

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating directory: $BUILD_DIR"
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
cmake --build . $VERBOSE_FLAG

popd > /dev/null  # restore the initial directory

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