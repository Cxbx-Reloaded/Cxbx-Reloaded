# Microsoft Developer Studio Project File - Name="Cxbx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Cxbx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cxbx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cxbx.mak" CFG="Cxbx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cxbx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Cxbx - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
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
# PROP Output_Dir "Bin"
# PROP Intermediate_Dir "Bin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "Include" /I "Include/Win32/" /I "Include/Win32/Cxbxkrnl" /I "Include/Win32/Cxbx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\Cxbx" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=cd PostBuild	upxCxbx.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Cxbx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bin\Debug"
# PROP Intermediate_Dir "Bin\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "Include" /I "Include/Win32/" /I "Include/Win32/Cxbxkrnl" /I "Include/Win32/Cxbx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\Cxbx" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Cxbx - Win32 Release"
# Name "Cxbx - Win32 Debug"
# Begin Group "Bin"

# PROP Default_Filter ""
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bin\Debug\Cxbx.exe
# End Source File
# End Group
# Begin Source File

SOURCE=.\Bin\Cxbx.exe
# End Source File
# End Group
# Begin Group "Doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Doc\Changelog.txt
# End Source File
# Begin Source File

SOURCE=.\Doc\Input.txt
# End Source File
# Begin Source File

SOURCE=.\Doc\RemovedCode.txt
# End Source File
# Begin Source File

SOURCE=.\Doc\Thanks.txt
# End Source File
# Begin Source File

SOURCE=.\Doc\Todo.txt
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\Win32\AlignPosfix1.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\AlignPrefix1.h
# End Source File
# Begin Source File

SOURCE=.\Include\Cxbx.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\Cxbx\DlgControllerConfig.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\Cxbx\EmuExe.h
# End Source File
# Begin Source File

SOURCE=.\Include\Core\Error.h
# End Source File
# Begin Source File

SOURCE=.\Include\Core\Exe.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\InputConfig.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\Cxbx\Prolog.h
# End Source File
# Begin Source File

SOURCE=.\Resource\ResCxbx.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\Cxbx\Wnd.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\Cxbx\WndAbout.h
# End Source File
# Begin Source File

SOURCE=.\Include\Win32\Cxbx\WndMain.h
# End Source File
# Begin Source File

SOURCE=.\Include\Core\Xbe.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource\About.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Cxbx.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Cxbx.rc
# End Source File
# Begin Source File

SOURCE=.\Resource\Logo.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Splash.bmp
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\DlgControllerConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\EmuExe.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\Exe.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\InputConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\OpenXDK.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\Prolog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\WinMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\Wnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\WndAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Win32\Cxbx\WndMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\Xbe.cpp
# End Source File
# End Group
# End Target
# End Project
