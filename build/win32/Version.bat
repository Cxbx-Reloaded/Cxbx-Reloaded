@ECHO OFF
SET GIT_VERSION=
git describe --always > GIT_VERSION
for /f "delims=" %%x in (GIT_VERSION) do set GIT_VERSION=%%x
ECHO #define _GIT_VERSION "%GIT_VERSION%" > "%~1"
del GIT_VERSION