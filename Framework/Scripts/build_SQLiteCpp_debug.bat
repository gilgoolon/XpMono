@echo off
setlocal enabledelayedexpansion

set "ORIGINAL_DIR=%cd%"
set "SUBMODULE_DIR=.\Include\External\SQLiteCpp"
set "TARGET_PATH=%SUBMODULE_DIR%\build\Debug\SQLiteCpp.lib"

IF EXIST "%TARGET_PATH%" (
    echo SQLiteCpp is already built, skipping...
    EXIT /B
)

cd "%SUBMODULE_DIR%"

mkdir -p build
cd build

cmake -G "Visual Studio 17 2022" -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DSQLITECPP_USE_STATIC_RUNTIME=ON -DCMAKE_CXX_FLAGS="-DSQLITECPP_STATIC_DEFINE /EHa" -A Win32 -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DSQLITECPP_USE_ASAN=OFF -DSQLITECPP_USE_GCOV=OFF -DBUILD_SHARED_LIBS=OFF -DSQLITECPP_BUILD_TESTS=OFF -DSQLITECPP_BUILD_EXAMPLES=OFF ..
cmake --build . --config Debug

cd "%ORIGINAL_DIR%"
