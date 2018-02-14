:: Cxbx-Reloaded artifact script

@if "%1"=="" goto no_arg

@set ZIP_APP="%cd%\import\7za\7za.exe"
@set BUILD_PATH="%cd%\build\win32\%1\"
@set ARTIFACT_PATH="%cd%\export\%1.zip"

:: Add generic resources
@call %ZIP_APP% u %ARTIFACT_PATH% COPYING
@call %ZIP_APP% u %ARTIFACT_PATH% README.md

:: Check for build resources
@if not exist %BUILD_PATH% goto missing_build
@pushd %BUILD_PATH%

:: Add Cxbx binaries, ignoring errors
@call %ZIP_APP% u %ARTIFACT_PATH% Cxbx.exe glew32.dll subhook.dll

:: Add debugger binaries, ignoring errors
@call %ZIP_APP% u %ARTIFACT_PATH% CxbxDebugger.exe capstone.dll cs_x86.dll

@popd
@goto eof

:missing_build
@echo Missing build directory %BUILD_PATH%
@goto eof

:no_arg
@echo Usage: %0 build_configuration
@goto eof

:eof
@set ERRORLEVEL=0
@echo Gather complete

