# Microsoft Developer Studio Project File - Name="Cxbe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Cxbe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cxbe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cxbe.mak" CFG="Cxbe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cxbe - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Cxbe - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cxbe - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "Include" /I "Include/Core/" /I "Include/Standard" /I "Include/Standard/Cxbe" /I "Include/Win32/" /I "Include/Win32/CxbxKrnl/" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Cxbe - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bin/Debug"
# PROP Intermediate_Dir "Bin/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /Zi /Od /I "Include" /I "Include/Core/" /I "Include/Standard" /I "Include/Standard/Cxbe" /I "Include/Win32/" /I "Include/Win32/CxbxKrnl/" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Cxbe - Win32 Release"
# Name "Cxbe - Win32 Debug"
# Begin Group "Bin"

# PROP Default_Filter ""
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bin\Debug\Cxbe.exe
# End Source File
# End Group
# Begin Source File

SOURCE=.\Bin\Cxbe.exe
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

SOURCE=.\Include\Core\Error.h
# End Source File
# Begin Source File

SOURCE=.\Include\Core\Exe.h
# End Source File
# Begin Source File

SOURCE=.\Include\Core\Xbe.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Core\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\Exe.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Standard\Cxbe\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\OpenXDK.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Core\Xbe.cpp
# End Source File
# End Group
# End Target
# End Project
