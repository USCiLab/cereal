@echo off
setlocal enabledelayedexpansion

if not defined APPVEYOR (
    @echo This script is meant to be used with AppVeyor CI. This can be used as reference.
    @echo I sincerely recommend not using it for building/testing cereal locally.
    exit /b 0
)

if not defined BOOST_ROOT (
    set BOOST_ROOT=C:\Libraries\boost
)

if not defined VS_VERSION_MAJOR (
    set VS_VERSION_MAJOR=14
)
if not defined VS_VERSION_YEAR (
    if "%VS_VERSION_MAJOR%" == "12" (
        set VS_VERSION_YEAR=2013
    ) else if "%VS_VERSION_MAJOR%" == "14" (
        set VS_VERSION_YEAR=2015
    ) else if "%VS_VERSION_MAJOR%" == "15" (
        set VS_VERSION_YEAR=2017
    ) else if "%VS_VERSION_MAJOR%" == "16" (
        set VS_VERSION_YEAR=2019
    ) else if "%VS_VERSION_MAJOR%" == "17" (
        set VS_VERSION_YEAR=2022
    ) else (
        @echo Cannot use Visual Studio version %VS_VERSION_MAJOR%
        exit /b 1
    )
)
if not defined CMAKE_GENERATOR_PREFIX (
    set CMAKE_GENERATOR_PREFIX=Visual Studio %VS_VERSION_MAJOR% %VS_VERSION_YEAR%
)

@rem CONFIGURATION is (one of the entries) defined in appveyor.yml
if not defined CONFIGURATION (
    set CONFIGURATION=Release
)
@rem PLATFORM is (one of the entries) defined in appveyor.yml
if "%PLATFORM%"=="x64" (
    set BIT_COUNT=64
) else (
    set BIT_COUNT=32
)

set BOOST_LIBRARYDIR=%BOOST_ROOT%\lib%BIT_COUNT%-msvc-%VS_VERSION_MAJOR%.0

set START_DIR=%CD%

if not exist build\NUL mkdir build
cd build

if "%~1" == "test" (
    @rem overloading the batch script; Run tests if the first argument is `test` (without quotes).
    @rem   Cereal uses Boost Unit test framework. Rather than modifying the code to load boost test
    @rem dll from its location OR copying the boost dlls to the directory of every test being run,
    @rem we use another option Windows leaves us - modify the PATH.
    for %%i in (ctest.exe) do set CTEST_EXE=%%~$PATH:i
    PATH %BOOST_LIBRARYDIR%
    "!CTEST_EXE!" -C %CONFIGURATION%
    if %errorlevel% neq 0 exit /b %errorlevel%
    goto done
)

if "%PLATFORM%" == "x64" (
    @rem please excuse the hack - CMake is unable to produce multiarch MSVC projects
    cmake -G "%CMAKE_GENERATOR_PREFIX%" -DBOOST_ROOT=%BOOST_ROOT% -DBOOST_LIBRARYDIR=%BOOST_LIBRARYDIR% ..
    cmake --build . --config %CONFIGURATION% --target portability_test32
    del CMakeCache.txt
    rmdir /s /q CMakeFiles
)

cmake -G "%CMAKE_GENERATOR_PREFIX%" -A %PLATFORM% -DBOOST_ROOT=%BOOST_ROOT% -DBOOST_LIBRARYDIR=%BOOST_LIBRARYDIR% ..
@rem left the actual build for later - AppVeyor enables parallel jobs in a much cleaner way than msbuild

:done
@REM go back home
cd %START_DIR%
