@echo off

REM Cxbx-Reloaded setup script
REM
REM Depends on git, cmake and Visual Studio being installed.

echo Pulling latest version from git...
REM git clone --recurse-submodules https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/
git pull --recurse-submodules

REM echo Synchronizing submodules...
REM git submodule update --init --recursive

echo Initializing most recent Visual Studio build environment...
@call "%VS140COMNTOOLS%vsvars32.bat"

echo Generating solution...
mkdir build
cd build
REM cmake .. -G "Visual Studio 16 2019" -A Win32
cmake .. -A Win32

echo Building solution...
cmake --build . -j %NUMBER_OF_PROCESSORS%

echo Done! Enjoy using Cxbx-Reloaded!
