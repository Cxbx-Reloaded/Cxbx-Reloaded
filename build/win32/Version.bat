@echo off

git describe --always --tags --first-parent > GIT_VERSION
@if not exist %~1 (
@goto update
)

@if exist LAST_GIT_VERSION (
fc GIT_VERSION LAST_GIT_VERSION > nul
@if errorlevel 1 goto update
@del GIT_VERSION
@goto eof
)

:update
@for /f "delims=" %%x in (GIT_VERSION) do set GIT_VERSION=%%x
@echo #define _GIT_VERSION "%GIT_VERSION%" > "%~1"
echo Updated %~1 with latest git revision number
@move /Y GIT_VERSION LAST_GIT_VERSION > nul

:eof
