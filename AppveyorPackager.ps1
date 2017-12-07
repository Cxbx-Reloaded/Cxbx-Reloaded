if exist build\win32\$(configuration)\Cxbx.exe (
7z a export\$(configuration).zip COPYING README.md
cd build\win32\$(configuration)
7z u ..\..\..\export\$(configuration).zip Cxbx.exe glew32.dll subhook.dll
cd ..\..\..\
)
