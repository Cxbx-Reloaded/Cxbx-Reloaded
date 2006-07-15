# Microsoft Developer Studio Project File - Name="CxbxKrnl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CxbxKrnl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CxbxKrnl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CxbxKrnl.mak" CFG="CxbxKrnl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CxbxKrnl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CxbxKrnl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "CxbxKrnl"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CxbxKrnl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CxbxKrnl___Win32_Release"
# PROP BASE Intermediate_Dir "CxbxKrnl___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\release"
# PROP Intermediate_Dir "..\..\bin\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\import\OpenXDK" /I "..\..\src" /I "..\..\src\Common" /I "..\..\src\Common\Win32" /I "..\..\import\OpenXDK\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\CxbxKrnl" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ws2_32.lib dsound.lib winmm.lib ddraw.lib d3dx8.lib d3d8.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\bin\release\Cxbx.dll"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "CxbxKrnl - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\src\Common" /I "..\..\import\OpenXDK\include" /I "..\..\src" /I "..\..\src\Common\Win32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\CxbxKrnl" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib dsound.lib winmm.lib ddraw.lib d3dx8.lib d3d8.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CxbxKrnl - Win32 Release"
# Name "CxbxKrnl - Win32 Debug"
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Group "HLEDataBase (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.3925.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4034.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4134.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4432.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.5558.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.5849.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.3936.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.4432.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.5849.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.3911.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4034.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4134.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4432.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.5558.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.5849.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XG.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XG.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XNet.1.0.3911.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XOnline.1.0.4361.h
# End Source File
# End Group
# Begin Group "EmuD3D8 (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\Convert.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\Common\Win32\AlignPosfix1.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\AlignPrefix1.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Cxbx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\CxbxKrnl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\DbgConsole.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\Emu.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuAlloc.h
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

SOURCE=..\..\src\CxbxKrnl\EmuFile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuFS.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuLDT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuNtDll.h
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

SOURCE=..\..\src\CxbxKrnl\HLEIntercept.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\OOVPA.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\PushBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\ResourceTracker.h
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

SOURCE=..\..\resource\Cxbx.ico
# End Source File
# Begin Source File

SOURCE=..\..\resource\CxbxDll.rc
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "HLEDataBase (inl)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.3925.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4034.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4134.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4432.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.5558.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\D3D8.1.0.5849.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.3936.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.4432.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.5558.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\DSound.1.0.5849.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.3911.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4034.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4134.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4432.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.5558.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\Xapi.1.0.5849.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XG.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XG.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XNet.1.0.3911.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase\XOnline.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "EmuD3D8 (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\Convert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\PushBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\State.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\VertexBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8\VertexShader.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\CxbxKrnl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\DbgConsole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\Emu.cpp

!IF  "$(CFG)" == "CxbxKrnl - Win32 Release"

!ELSEIF  "$(CFG)" == "CxbxKrnl - Win32 Debug"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuAlloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuD3D8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuDInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuDSound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuFS.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuKrnl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuLDT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuNtDll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\EmuShared.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXapi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\EmuXOnline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEDataBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\HLEIntercept.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\KernelThunk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\Win32\Mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Common\OpenXDK.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CxbxKrnl\ResourceTracker.cpp
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
