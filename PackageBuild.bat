@echo off

@if "%1"=="" goto no_arg
@if "%~2"=="Any CPU" goto eof

set BUILD_PATH=build\win32\%1\

if not exist COPYING goto no_preq
if not exist README.md goto no_preq
if not exist %BUILD_PATH%Cxbx.exe goto no_build
if not exist %BUILD_PATH%glew32.dll goto no_build
if not exist %BUILD_PATH%subhook.dll goto no_build

set ZIP_APP="import\7za\7za.exe"
set EXPORT_ZIP=export\%1.zip

@call %ZIP_APP% a %EXPORT_ZIP% COPYING README.md
cd %BUILD_PATH%
@call ..\..\..\%ZIP_APP% u ..\..\..\%EXPORT_ZIP% Cxbx.exe glew32.dll subhook.dll
@call ..\..\..\%ZIP_APP% u ..\..\..\%EXPORT_ZIP% CxbxDebugger.exe capstone.dll cs_x86.dll
cd /d %~dp0
echo Finished packaging %EXPORT_ZIP%!
goto eof

:no_preq
echo Missing files from repo
goto eof

:no_build
echo Missing build at path %BUILD_PATH%
goto eof

:no_arg
echo "Usage: %0 <build_configuration>"
goto eof

:eof