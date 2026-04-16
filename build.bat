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
cmake --build . --parallel

popd

if %errorlevel% NEQ 0 (
    goto :EOF
)

:: Add various environment variables progamatically for the user
@echo off
set TEENYAT_BUILD_ROOT=%CD%\build
set BIN_PATH=%TEENYAT_BUILD_ROOT%\out\bin

echo Setting up TeenyAT build environment...
echo Build root: %TEENYAT_BUILD_ROOT%
echo.

echo 1) Setting TEENYAT_BUILD_ROOT directory.
setx TEENYAT_BUILD_ROOT "%TEENYAT_BUILD_ROOT%"

echo.
echo 2) Setting CPATH for headers when compiling.
setx CPATH "%TEENYAT_BUILD_ROOT%\out\include"

echo.
echo 3) Setting LIBRARY_PATH for static library linking (.a).
setx LIBRARY_PATH "%TEENYAT_BUILD_ROOT%\out\lib"

echo.
echo 4) Updating PATH to enable running tnasm, lcd or edison and use the DLL.

for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v PATH') do set "CURRENT_USER_PATH=%%B"

echo %CURRENT_USER_PATH% | findstr /C:"%BIN_PATH%" >nul

::Only add to the PATH if it doesn't exist
if %errorlevel%==0 (
    echo.
    echo Already in PATH, skipping.
) else (
    setx PATH "%CURRENT_USER_PATH%;%BIN_PATH%"
)

echo.
echo Done. Restart your terminal for changes to take effect.

endlocal
