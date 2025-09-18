@echo off
setlocal

pushd .

set BUILD_DIR=build
set BUILD_TYPE=Release

if "%1"=="debug" (
    echo Configuring for Debug build...
    :: unsure if we want separate directories for this
    :: set BUILD_DIR=build_debug
    set BUILD_TYPE=Debug
) else if "%1"=="clean" (
    echo Cleaning build directory...
    :: again, unsure if we want separate directories for this
    :: rd /s /q build build_debug
    rd /s /q build
    echo.
    echo Clean complete.
    echo.

    popd
    exit /b
) else if "%1" neq "" (
    echo.
    echo Invalid parameter: %1
    echo Usage: build.bat [debug ^| clean]
    echo     No parameter - Default Release build
    echo     debug - Build with debug symbols
    echo     clean - Remove build directory
    echo.

    popd
    exit /b
) else (
    echo Using default build configuration...
)

if not exist "%BUILD_DIR%" (
    echo Creating directory: "%BUILD_DIR%"
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

echo Running CMake configuration...
cmake -DCMAKE_BUILD_TYPE="%BUILD_TYPE%" ..

echo Building project...
cmake --build .

popd

echo.
echo Build complete!
echo.

endlocal
