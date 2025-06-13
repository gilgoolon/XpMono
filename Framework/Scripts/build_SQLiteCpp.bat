@echo off
setlocal enabledelayedexpansion

set "ORIGINAL_DIR=%cd%"
set "SUBMODULE_DIR=.\Include\External\SQLiteCpp"

cd "%SUBMODULE_DIR%"

mkdir -p build
cd build

cmake -G "Visual Studio 17 2022" -DSQLITECPP_USE_ASAN=OFF -DSQLITECPP_USE_GCOV=OFF -DBUILD_SHARED_LIBS=OFF -DSQLITECPP_BUILD_TESTS=OFF -DSQLITECPP_BUILD_EXAMPLES=OFF ..
cmake --build . --config Release

cd "%ORIGINAL_DIR%"
