cd ..\..\Bin\release
..\Import\upx\bin\upx -9 Cxbx.exe
..\Import\upx\bin\upx -9 Cxbx.dll
copy Cxbx.exe ..\Export\Win32\Bin\
copy Cxbx.dll ..\Export\Win32\Bin\
cd ..\Build\win32

