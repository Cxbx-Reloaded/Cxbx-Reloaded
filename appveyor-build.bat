:: Cxbx-Reloaded build script

@if "%1"=="" goto show_usage

@set SLN="%cd%\build\win32\Cxbx.sln"
@set VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsMSBuildCmd.bat"

:: Setup build environment
@if not exist %VS_ENV% goto missing_env

@call %VS_ENV%

:: Build native projects (emulator, debugger dlls)
@call MSBuild.exe %SLN% /p:Platform="Win32" /p:Configuration=%1% /m
@if %errorlevel% neq 0 goto failure

@goto success

:missing_env
@echo Unable to locate MSBuild environment command
@set ERRORLEVEL=1
@goto eof

:show_usage
@echo Usage: %~0% build_config
@set ERRORLEVEL=1
@goto eof

:failure
@echo Failed to build at least one configuration, exiting...
@goto eof

:success
@echo All builds finished successfully
@goto eof

:eof

