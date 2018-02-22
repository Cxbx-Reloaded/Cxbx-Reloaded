:: Cxbx-Reloaded build script

@if "%1"=="" goto show_usage

@set SLN="%cd%\build\win32\Cxbx.sln"
:: See https://www.appveyor.com/docs/lang/cpp/
@set VS_ENV32="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

:: Setup build environment
@if not exist %VS_ENV32% goto missing_env

@call %VS_ENV32%

:: Build managed projects (debugger)
@call MSBuild %SLN% /p:Platform="Any CPU" /p:Configuration=%1%
@if %errorlevel% neq 0 goto failure

:: Build native projects (emulator, debugger dlls)
@call MSBuild %SLN% /p:Platform="Win32" /p:Configuration=%1%
@if %errorlevel% neq 0 goto failure

@goto success

:missing_env
@echo Unable to locate MSBuild environment command
@set ERRORLEVEL=1
@goto eof

:missing_log
@echo Unable to locate AppVeyor build logger
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

