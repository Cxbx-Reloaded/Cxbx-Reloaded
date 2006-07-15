cd ..\..\bin\release
..\..\import\upx\bin\upx -9 Cxbx.exe
..\..\import\upx\bin\upx -9 Cxbx.dll
copy Cxbx.exe ..\..\export\win32\bin\release\
copy Cxbx.dll ..\..\export\win32\bin\release\
cd ..\..\bin\debug
copy Cxbx.exe ..\..\export\win32\bin\debug\
copy Cxbx.pdb ..\..\export\win32\bin\debug\
copy CxbxKrnl.dll ..\..\export\win32\bin\debug\
copy CxbxKrnl.pdb ..\..\export\win32\bin\debug\
cd ..\..\build\win32

