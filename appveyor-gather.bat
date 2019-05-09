:: Cxbx-Reloaded gather script

@if "%1"=="" goto show_usage

@set ZIP_APP="%cd%\import\7za\7za.exe"
@set BUILD_PATH="%cd%\build\win32\%1\"
@set ARTIFACT_PATH="%cd%\export\%1.zip"

:: Add generic resources
@call %ZIP_APP% u %ARTIFACT_PATH% COPYING README.md

:: Check for build resources
@if not exist %BUILD_PATH% goto missing_build
@pushd %BUILD_PATH%

:: Add Cxbx binaries
@call %ZIP_APP% u %ARTIFACT_PATH% Cxbx.exe glew32.dll subhook.dll CxbxVSBC.dll

:: Add debugger binaries
@call %ZIP_APP% u %ARTIFACT_PATH% CxbxDebugger.exe capstone.dll cs_x86.dll

@popd
@goto eof

:missing_build
@echo Missing build directory %BUILD_PATH%
@set ERRORLEVEL=1
@goto eof

:show_usage
@echo Usage: %~0% build_config
@goto eof

:eof
@echo Gather complete

