# Microsoft Developer Studio Project File - Name="Cxbx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Cxbx - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cxbx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cxbx.mak" CFG="Cxbx - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cxbx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Cxbx - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Cxbx"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cxbx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\release"
# PROP Intermediate_Dir "..\..\bin\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\src" /I "..\..\src\Common" /I "..\..\src\Common\Win32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\Cxbx" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libjpeg.lib d3d8.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\..\import\libjpeg\win32\lib"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "Cxbx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin\debug"
# PROP Intermediate_Dir "..\..\bin\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\src\Common" /I "..\..\src" /I "..\..\src\Common\Win32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\Cxbx" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libjpeg.lib d3d8.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /pdbtype:sept /libpath:"..\..\import\libjpeg\win32\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Cxbx - Win32 Release"
# Name "Cxbx - Win32 Debug"
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Group "jpegdec (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Cxbx\jpegdec\jconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\jpegdec\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\jpegdec\jpegdec.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\jpegdec\jpeglib.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\Common\Win32\AlignPosfix1.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\AlignPrefix1.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\Convert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\CxbxKrnl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\DlgControllerConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\DlgVideoConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\Emu.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8Types.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuDInput.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuDSound.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\EmuExe.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuShared.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXG.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXOnline.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXTL.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Error.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Exe.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\Prolog.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\PushBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\State.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\VertexBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\VertexShader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\Wnd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\WndAbout.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\WndMain.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\XBController.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Xbe.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\XBVideo.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\resource\About.jpg
# End Source File
# Begin Source File

SOURCE=..\..\resource\Cxbx.ico
# End Source File
# Begin Source File

SOURCE=..\..\resource\Cxbx.rc
# End Source File
# Begin Source File

SOURCE=..\..\resource\Logo.bmp
# End Source File
# Begin Source File

SOURCE=..\..\resource\Splash.jpg
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "jpegdec (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Cxbx\jpegdec\jpegdec.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\Cxbx\DlgControllerConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\DlgVideoConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\EmuExe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Exe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\Mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\OpenXDK.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\Prolog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\WinMain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\Wnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\WndAbout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx\WndMain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\XBController.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Xbe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\XBVideo.cpp
# End Source File
# End Group
# End Target
# End Project
