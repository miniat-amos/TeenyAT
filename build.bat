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
    :: rd /s /q build build_debug 2>NUL
    rd /s /q build 2>NUL

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


if not exist "%BUILD_DIR%" (
    echo Creating directory: "%BUILD_DIR%"
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

echo Running CMake configuration...
set MAKE_TARGET=-G "MinGW Makefiles"
set C_COMPILER=-DCMAKE_C_COMPILER="%C%"
set CXX_COMPILER=-DCMAKE_CXX_COMPILER="%CXX%"
set BUILD=-DCMAKE_BUILD_TYPE="%BUILD_TYPE%"
cmake %MAKE_TARGET% %C_COMPILER% %CXX_COMPILER% %BUILD% ..

echo Building project...
cmake --build .

popd

if %errorlevel% NEQ 0 (
    goto :EOF
)

echo.
echo ========================================================================
echo To complete your setup of the TeenyAT and make it easier to use the
echo library in code or use its systems and assembler, consider setting up
echo and editing the enviornment variables below.  From the Windows menu,
echo start typing "environment" to access the application to edit your
echo environment variables.
echo ========================================================================
echo.

set TEENYAT_BUILD_ROOT=%CD%

echo 1) Define the root of your build directory (modify as necessary).
echo Create a variable, TEENYAT_BUILD_ROOT and set it to %TEENYAT_BUILD_ROOT%\build
echo.
echo 2) Set up the include directory for headers when compiling.
echo Edit/add CPATH to include %%TEENYAT_BUILD_ROOT%%\out\include
echo.
echo 3) Update your PATH to run tnasm, lcd or edison and use the DLL.
echo Edit PATH to include %%TEENYAT_BUILD_ROOT%%\out\bin
echo.
echo 4) Set up the library directory for static library linking (.a).
echo Edit/add LIBRARY_PATH to include %%TEENYAT_BUILD_ROOT%%\out\lib
echo.

endlocal
