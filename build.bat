@echo off
setlocal

pushd .

:: check for and prefer GCC/G++
echo Checking for GCC/G++
where gcc >nul 2>&1
if %errorlevel%==0 (
    where g++ >nul 2>&1
    if %errorlevel%==0 (
        set C=gcc
        set CXX=g++
        goto :end_compiler_check
    )

)

echo Missing either GCC or G++
:: check for Clang/Clang++
echo Checking for Clang/Clang++
where clang >nul 2>&1
if %errorlevel%==0 (
    where clang++ >nul 2>&1
    if %errorlevel%==0 (
        set C=clang
        set CXX=clang++
        goto :end_compiler_check
    )

)

:: Niether GCC/G++ or Clang/Clang++ was found
echo Error: No suitable compiler (GCC/G++ or Clang/Clang++) found
exit /b

:end_compiler_check


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
set MAKE_TARGET=-G "MinGW Makefiles"
set C_COMPILER=-DCMAKE_C_COMPILER="!C!"
set CXX_COMPILER=-DCMAKE_CXX_COMPILER="!CXX!"
set BUILD=-DCMAKE_BUILD_TYPE="%BUILD_TYPE%"
cmake %MAKE_TARGET% %C_COMPILER% %CXX_COMPILER% %BUILD% ..

echo Building project...
cmake --build .

popd

endlocal
