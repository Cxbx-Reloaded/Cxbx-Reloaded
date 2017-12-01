if exist Release\Cxbx.exe (
cd Release
..\..\..\import\7za\7za.exe a ..\..\..\export\Release.zip Cxbx.exe glew32.dll subhook.dll ..\..\..\COPYING ..\..\..\README.md
cd ..\
)
if exist Debug\Cxbx.exe (
cd Debug
..\..\..\import\7za\7za.exe a ..\..\..\export\Debug.zip Cxbx.exe glew32.dll subhook.dll ..\..\..\COPYING ..\..\..\README.md
cd ..\
)
