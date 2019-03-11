@ECHO off

:: LICENSE: Public Domain
:: MADE BY: RadWolfie (2019)

SETLOCAL

IF "%1"=="" (
GOTO :helpInfo
)
IF /I "%1"=="-h" (
GOTO :helpInfo
)
IF /I "%1"=="-help" (
GOTO :helpInfo
)

IF /I "%1"=="all" (
SET x86=1
SET x64=1
SET ARM=1
SET ARM64=1
ECHO Generating all available projects...
)
IF /I "%1"=="x86" (
SET x86=1
)
IF /I "%1"=="x64" (
SET x64=1
)
IF /I "%1"=="ARM" (
SET ARM=1
)
IF /I "%1"=="ARM64" (
SET ARM64=1
)

IF DEFINED x86 (
ECHO Generating x86 project...
mkdir build-x86 & pushd build-x86
cmake -G "Visual Studio 15 2017" -A Win32 ..
popd
)
IF DEFINED x64 (
ECHO Generating x64 project...
mkdir build-x64 & pushd build-x64
cmake -G "Visual Studio 15 2017" -A x64 ..
popd
)
IF DEFINED ARM (
ECHO Generating ARM project...
mkdir build-arm & pushd build-arm
cmake -G "Visual Studio 15 2017" -A ARM ..
popd
)
IF DEFINED ARM64 (
ECHO Generating ARM64 project...
mkdir build-arm64 & pushd build-arm64
cmake -G "Visual Studio 15 2017" -A ARM64 ..
popd
)
GOTO :end

:helpInfo
ECHO Available options are:
ECHO - all
ECHO - x86
ECHO - x64
ECHO - ARM
ECHO - ARM64
PAUSE
GOTO :end

:end
:: Clear local variables.
ENDLOCAL
