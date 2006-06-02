# Microsoft Developer Studio Project File - Name="CxbxKrnl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
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
CPP=xicl6.exe
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
# PROP Output_Dir "Bin"
# PROP Intermediate_Dir "Bin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "Import\OpenXDK" /I "Source" /I "Source\Common\Win32" /I "Import\OpenXDK\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\CxbxKrnl" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ws2_32.lib dsound.lib winmm.lib ddraw.lib d3dx8.lib d3d8.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"Bin/Cxbx.dll" /libpath:"Lib"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "CxbxKrnl - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "Source" /I "Source\Common\Win32" /I "Import\OpenXDK\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CXBXKRNL_EXPORTS" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Include\Win32\CxbxKrnl" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib dsound.lib winmm.lib ddraw.lib d3dx8.lib d3d8.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /pdbtype:sept /libpath:"Lib"
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

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.3925.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4034.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4134.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4432.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.5558.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.5849.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.3936.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.4432.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.5849.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.3911.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4034.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4134.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4432.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.5558.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.5849.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XG.1.0.4361.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XG.1.0.4627.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XNet.1.0.3911.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XOnline.1.0.4361.h
# End Source File
# End Group
# Begin Group "EmuD3D8 (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\Convert.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\Common\Win32\AlignPosfix1.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\AlignPrefix1.h
# End Source File
# Begin Source File

SOURCE=.\Source\Cxbx.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\CxbxKrnl.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\DbgConsole.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\Emu.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuAlloc.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8Types.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuDInput.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuDSound.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuFS.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuLDT.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuNtDll.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuShared.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXapi.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXG.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXOnline.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXTL.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Error.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Exe.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEIntercept.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\Mutex.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\OOVPA.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\PushBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\ResourceTracker.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\State.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\VertexBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\VertexShader.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\XBController.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Xbe.h
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\XBVideo.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource\Cxbx.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\CxbxDll.rc
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "HLEDataBase (inl)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.3925.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4034.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4134.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4432.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.5558.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\D3D8.1.0.5849.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.3936.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.4432.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.5558.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\DSound.1.0.5849.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.3911.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4034.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4134.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4432.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.5558.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\Xapi.1.0.5849.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XG.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XG.1.0.4627.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XNet.1.0.3911.inl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase\XOnline.1.0.4361.inl
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "EmuD3D8 (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\Convert.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\PushBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\State.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\VertexBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8\VertexShader.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\CxbxKrnl\CxbxKrnl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\DbgConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\Emu.cpp

!IF  "$(CFG)" == "CxbxKrnl - Win32 Release"

!ELSEIF  "$(CFG)" == "CxbxKrnl - Win32 Debug"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuAlloc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuD3D8.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuDInput.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuDSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuFS.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuKrnl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuLDT.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuNtDll.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\EmuShared.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXapi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXG.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\EmuXOnline.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEDataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\HLEIntercept.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\KernelThunk.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\Mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\OpenXDK.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CxbxKrnl\ResourceTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\XBController.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Xbe.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Common\Win32\XBVideo.cpp
# End Source File
# End Group
# End Target
# End Project
