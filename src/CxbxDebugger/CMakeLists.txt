cmake_minimum_required (VERSION 3.12)
project(cxbxr-debugger LANGUAGES CSharp)

# Output all binary files into one folder
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
 set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${cxbxr-debugger_BINARY_DIR}/bin")
endif()

include(CSharpUtilities)

# Workaround fix since CMake is using static version "3".
add_compile_options(
 /langversion:6
)

# First, we must define .NET Framework version before include cs_x86's projects.
# Which then will pass down version we want unified.
set(DOTNET_TARGET_FRAMEWORK_VERSION "v4.8")

add_subdirectory("${CXBXR_ROOT_DIR}/import/cs_x86" "${CMAKE_BINARY_DIR}/import/cs_x86")

set(CXBXR_DEBUGGER_SRC_DIR "${CXBXR_ROOT_DIR}/src/CxbxDebugger")

file (GLOB SOURCES
 "${CXBXR_DEBUGGER_SRC_DIR}/App.config"
 "${CXBXR_DEBUGGER_SRC_DIR}/Common.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/Debugger.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerCallstack.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerEventInterfaces.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerInstance.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerMessages.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerModule.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerProcess.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerSymbolServer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Debugger/DebuggerThread.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerExtras/CheatTable.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerExtras/CheatTableReader.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerSymbols/DebuggerSymbolProvider.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerSymbols/HLECache/HLECacheFile.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerSymbols/HLECache/HLECacheProvider.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerSymbols/HLECache/Utils/INIReader.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebuggerSymbols/Kernel/KernelSymbolProvider.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/DebugOutputManager.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/FileEventManager.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/FileWatchManager.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/IDebugWindow.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.Designer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.resx"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.Designer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.resx"
 "${CXBXR_DEBUGGER_SRC_DIR}/PatchManager.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Program.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Resources/BreakpointDisable_16x_24.bmp"
 "${CXBXR_DEBUGGER_SRC_DIR}/Resources/BreakpointEnable_16x_24.bmp"
 "${CXBXR_DEBUGGER_SRC_DIR}/Resources/Pause_16x_24.bmp"
 "${CXBXR_DEBUGGER_SRC_DIR}/Resources/Run_16x_24.bmp"
 "${CXBXR_DEBUGGER_SRC_DIR}/RicherTextBox.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/ThreadHelpers.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Constants.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/CONTINUE_STATUS.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/CREATE_PROCESS_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/CREATE_THREAD_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/DEBUG_EVENT.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/DEBUG_EVENT_CODE.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/EXCEPTION_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/EXCEPTION_RECORD.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/EXIT_PROCESS_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/EXIT_THREAD_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/LOAD_DLL_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/NativeMethods.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/OUTPUT_DEBUG_STRING_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/RIP_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Debugging/UNLOAD_DLL_DEBUG_INFO.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Handles/NativeMethods.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Interop/SecurityAttributes.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/IO_COUNTERS.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/JobInformationLimitFlags.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/JobMsgInfoMessages.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/JOBOBJECTINFOCLASS.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/JOBOBJECT_ASSOCIATE_COMPLETION_PORT.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/JOBOBJECT_BASIC_LIMIT_INFORMATION.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/JOBOBJECT_EXTENDED_LIMIT_INFORMATION.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Jobs/NativeMethods.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/LastWin32ErrorException.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/NtStatus.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/NativeEnums.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/NativeMethods.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/NativeStructs.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/ProcessCreationFlags.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/PROCESS_INFORMATION.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/SafeProcessHandle.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/SafeThreadHandle.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Processes/Startupinfo.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/UnicodeString.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Win32/Windows/NativeMethods.cs"
)

file (GLOB PROPERTIES
 "${CXBXR_DEBUGGER_SRC_DIR}/Properties/Settings.settings"
 "${CXBXR_DEBUGGER_SRC_DIR}/Properties/AssemblyInfo.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Properties/Resources.Designer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Properties/Resources.resx"
 "${CXBXR_DEBUGGER_SRC_DIR}/Properties/Settings.Designer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/Properties/Settings.settings"
)

csharp_set_windows_forms_properties(
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.Designer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.resx"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.Designer.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.resx"
)

csharp_set_designer_cs_properties(
 ${PROPERTIES}
)

set_source_files_properties(
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerInstance.cs"
 "${CXBXR_DEBUGGER_SRC_DIR}/CxbxDebuggerMain.cs"
 VS_CSHARP_SubType "Form"
)

set_source_files_properties("${CXBXR_DEBUGGER_SRC_DIR}/RicherTextBox.cs"
 VS_CSHARP_SubType "Component"
)

source_group(TREE ${CXBXR_ROOT_DIR} FILES ${SOURCES})

add_executable(cxbxr-debugger WIN32 ${SOURCES} ${PROPERTIES} #Test WIN32 like cxbx does if doesn't need compile option set
)

set_target_properties(cxbxr-debugger PROPERTIES
 VS_DOTNET_REFERENCES
 "Microsoft.CSharp;System;System.Core;System.Data;System.Data.DataSetExtensions;System.Deployment;System.Drawing;System.Windows;System.Windows.Forms;System.Xml;System.Xml.Linq;System.Net.Http"
 VS_GLOBAL_ApplicationIcon "${CXBXR_ROOT_DIR}/src/gui/resource/Cxbx-R.ico"

 VS_GLOBAL_ROOTNAMESPACE "CxbxDebugger"

 DOTNET_TARGET_FRAMEWORK_VERSION ${DOTNET_TARGET_FRAMEWORK_VERSION}
)

target_link_libraries(cxbxr-debugger cs_x86)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)
