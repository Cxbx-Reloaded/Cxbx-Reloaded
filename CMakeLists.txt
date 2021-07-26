cmake_minimum_required (VERSION 3.8)

project(Cxbx-Reloaded)

# Allow devs to disable regeneration option.
# Suppress extra stuff from generated solution
#set(CMAKE_SUPPRESS_REGENERATION true)

# Output all binary files into one folder
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
 set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${Cxbx-Reloaded_BINARY_DIR}/bin")
endif()

set(SUBHOOK_TESTS OFF)
set(SUBHOOK_INSTALL OFF)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/import/subhook" EXCLUDE_FROM_ALL)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/import/XbSymbolDatabase")

# Not require since only include a header file
#add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/import/simpleini")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/import/SDL2" EXCLUDE_FROM_ALL)

if(${CMAKE_GENERATOR} MATCHES "Visual Studio ([^9]|[9][0-9])")
 add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/import/cs_x86")
endif()

# Cxbx-Reloaded projects
set(CXBXR_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

add_custom_target(misc-batch
  ${CMAKE_COMMAND} -DTargetRunTimeDir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>
  -P ${CXBXR_ROOT_DIR}/projects/misc/batch.cmake
  WORKING_DIRECTORY ${CXBXR_ROOT_DIR}
)

# Custom CMake projects since some import libraries doesn't have it.

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/projects/libtom")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/projects/imgui")

# Split the files into group for which project is likely
# going to be used for both header and source files.
# Then move only specific project files into their
# cmakelist file in finalize stage.
# Any of files being shared between two projects need to
# leave in parent cmakelist (here) for better sync across
# child projects.

# Common (GUI and Emulator)
file (GLOB CXBXR_HEADER_COMMON
 "${CXBXR_ROOT_DIR}/src/common/crypto/EmuDes.h"
 "${CXBXR_ROOT_DIR}/src/common/crypto/EmuRsa.h"
 "${CXBXR_ROOT_DIR}/src/common/crypto/EmuSha.h"
 "${CXBXR_ROOT_DIR}/src/common/crypto/LibRc4.h"
 "${CXBXR_ROOT_DIR}/src/common/CxbxDebugger.h"
 "${CXBXR_ROOT_DIR}/src/common/EmuEEPROM.h"
 "${CXBXR_ROOT_DIR}/src/common/Error.h"
 "${CXBXR_ROOT_DIR}/src/common/input/DInputKeyboardCodes.h"
 "${CXBXR_ROOT_DIR}/src/common/input/DInputKeyboardMouse.h"
 "${CXBXR_ROOT_DIR}/src/common/input/layout_xbox_device.h"
 "${CXBXR_ROOT_DIR}/src/common/input/InputDevice.h"
 "${CXBXR_ROOT_DIR}/src/common/input/InputManager.h"
 "${CXBXR_ROOT_DIR}/src/common/input/SdlJoystick.h"
 "${CXBXR_ROOT_DIR}/src/common/input/XInputPad.h"
 "${CXBXR_ROOT_DIR}/src/common/input/RawDevice.h"
 "${CXBXR_ROOT_DIR}/src/common/IPCHybrid.hpp"
 "${CXBXR_ROOT_DIR}/src/common/Logging.h"
 "${CXBXR_ROOT_DIR}/src/common/ReservedMemory.h"
 "${CXBXR_ROOT_DIR}/src/common/Settings.hpp"
 "${CXBXR_ROOT_DIR}/src/common/Timer.h"
 "${CXBXR_ROOT_DIR}/src/common/util/cliConfig.hpp"
 "${CXBXR_ROOT_DIR}/src/common/util/cliConverter.hpp"
 "${CXBXR_ROOT_DIR}/src/common/util/CPUID.h"
 "${CXBXR_ROOT_DIR}/src/common/util/crc32c.h"
 "${CXBXR_ROOT_DIR}/src/common/util/CxbxUtil.h"
 "${CXBXR_ROOT_DIR}/src/common/util/std_extend.hpp"
 "${CXBXR_ROOT_DIR}/src/common/util/strConverter.hpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/AlignPosfix1.h"
 "${CXBXR_ROOT_DIR}/src/common/win32/AlignPrefix1.h"
 "${CXBXR_ROOT_DIR}/src/common/win32/EmuShared.h"
 "${CXBXR_ROOT_DIR}/src/common/win32/Mutex.h"
 "${CXBXR_ROOT_DIR}/src/common/win32/Threads.h"
 "${CXBXR_ROOT_DIR}/src/common/win32/Util.h"
 "${CXBXR_ROOT_DIR}/src/common/win32/WineEnv.h"
 "${CXBXR_ROOT_DIR}/src/common/xbdm/CxbxXbdm.h"
 "${CXBXR_ROOT_DIR}/src/common/xbe/Xbe.h"
 "${CXBXR_ROOT_DIR}/src/common/xbe/XbePrinter.h"
 "${CXBXR_ROOT_DIR}/src/common/xbox/Types.hpp"
 "${CXBXR_ROOT_DIR}/src/common/xdvdfs-tools/buffered_io.h"
 "${CXBXR_ROOT_DIR}/src/common/xdvdfs-tools/xdvdfs.h"
 "${CXBXR_ROOT_DIR}/src/Cxbx.h"
 "${CXBXR_ROOT_DIR}/src/CxbxVersion.h"
 "${CXBXR_ROOT_DIR}/src/version.h"
)

# GUI v1
file (GLOB CXBXR_HEADER_GUIv1
 "${CXBXR_ROOT_DIR}/src/common/input/Button.h"
 "${CXBXR_ROOT_DIR}/src/common/input/EmuDevice.h"
 "${CXBXR_ROOT_DIR}/src/common/input/InputWindow.h"
 "${CXBXR_ROOT_DIR}/src/gui/controllers/DlgDukeControllerConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/controllers/DlgSBControllerConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgAbout.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgAudioConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgInputConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgEepromConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgLoggingConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgNetworkConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/DlgVideoConfig.h"
 "${CXBXR_ROOT_DIR}/src/gui/Wnd.h"
 "${CXBXR_ROOT_DIR}/src/gui/WndMain.h"
)

# Emulator (module)
file (GLOB CXBXR_HEADER_EMU_IMPORT
 "${CXBXR_ROOT_DIR}/import/imgui/backends/imgui_impl_dx9.h"
 "${CXBXR_ROOT_DIR}/import/imgui/backends/imgui_impl_opengl3.h"
 "${CXBXR_ROOT_DIR}/import/imgui/backends/imgui_impl_win32.h"
)
file (GLOB CXBXR_HEADER_EMU
 "${CXBXR_ROOT_DIR}/src/common/AddressRanges.h"
 "${CXBXR_ROOT_DIR}/src/common/audio/converter.hpp"
 "${CXBXR_ROOT_DIR}/src/common/util/gloffscreen/glextensions.h"
 "${CXBXR_ROOT_DIR}/src/common/util/gloffscreen/gloffscreen.h"
 "${CXBXR_ROOT_DIR}/src/common/audio/XADPCM.h"
 "${CXBXR_ROOT_DIR}/src/common/xbox/Logging.hpp"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/audio.hpp"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/ui.hpp"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/settings.h"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/video.hpp"
 "${CXBXR_ROOT_DIR}/src/core/common/video/RenderBase.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/CxbxPixelShaderTemplate.hlsl"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/CxbxVertexShaderTemplate.hlsl"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/Direct3D9.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionPixelShader.hlsl"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionPixelShader.hlsli"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionVertexShader.hlsl"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionVertexShaderState.hlsli"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/PixelShader.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/Shader.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/VertexShader.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/VertexShaderSource.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/WalkIndexBuffer.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/FixedFunctionState.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/ResourceTracker.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbConvert.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbD3D8Logging.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbD3D8Types.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbPixelShader.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbPushBuffer.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbState.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbVertexBuffer.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbVertexShader.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/common/windows/WFXformat.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSound.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundGlobal.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundInline.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundLogging.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DSStream_PacketManager.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/XbDSoundFuncs.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/XbDSoundLogging.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/XbDSoundTypes.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/common/XbInternalStruct.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/Intercept.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/Patches.hpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/XACTENG/XactEng.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/XAPI/Xapi.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/XGRAPHIC/XGraphic.h"
 "${CXBXR_ROOT_DIR}/src/core/hle/XONLINE/XOnline.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/common/strings.hpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlAvModes.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlKe.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlKi.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlLogging.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/init/CxbxKrnl.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/memory-manager/PhysicalMemory.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/memory-manager/PoolManager.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/memory-manager/VMManager.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/Emu.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/EmuFile.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/EmuFS.h"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/EmuNtDll.h"
 "${CXBXR_ROOT_DIR}/src/devices/ADM1032Device.h"
 "${CXBXR_ROOT_DIR}/src/devices/EEPROMDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/network/NVNetDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/LED.h"
 "${CXBXR_ROOT_DIR}/src/devices/MCPXDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/PCIBus.h"
 "${CXBXR_ROOT_DIR}/src/devices/PCIDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/SMBus.h"
 "${CXBXR_ROOT_DIR}/src/devices/SMCDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/SMDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/usb/Hub.h"
 "${CXBXR_ROOT_DIR}/src/devices/usb/OHCI.h"
 "${CXBXR_ROOT_DIR}/src/devices/usb/UsbCommon.h"
 "${CXBXR_ROOT_DIR}/src/devices/usb/USBDevice.h"
 "${CXBXR_ROOT_DIR}/src/devices/usb/XidGamepad.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_debug.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_int.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_psh.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_regs.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_shaders.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_shaders_common.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_vsh.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/qemu-thread.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/queue.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/swizzle.h"
 "${CXBXR_ROOT_DIR}/src/devices/video/vga.h"
 "${CXBXR_ROOT_DIR}/src/devices/x86/EmuX86.h"
 "${CXBXR_ROOT_DIR}/src/devices/Xbox.h"
)

# filter hlsl files into its own list
# excluding hlsli file(s)
set(CXBXR_HEADER_HLSL ${CXBXR_HEADER_EMU})
list(FILTER CXBXR_HEADER_HLSL INCLUDE REGEX ".*\\.hlsl$")

# Common (GUI and Emulator)
file (GLOB CXBXR_SOURCE_COMMON
 "${CXBXR_ROOT_DIR}/src/common/crypto/EmuDes.cpp"
 "${CXBXR_ROOT_DIR}/src/common/crypto/EmuRsa.cpp"
 "${CXBXR_ROOT_DIR}/src/common/crypto/EmuSha.cpp"
 "${CXBXR_ROOT_DIR}/src/common/crypto/LibRc4.cpp"
 "${CXBXR_ROOT_DIR}/src/common/CxbxDebugger.cpp"
 "${CXBXR_ROOT_DIR}/src/common/EmuEEPROM.cpp"
 "${CXBXR_ROOT_DIR}/src/common/Error.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/DInputKeyboardMouse.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/InputDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/InputManager.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/SdlJoystick.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/XInputPad.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/RawDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/common/Logging.cpp"
 "${CXBXR_ROOT_DIR}/src/common/Settings.cpp"
 "${CXBXR_ROOT_DIR}/src/common/Timer.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/cliConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/cliConverter.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/crc32c.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/CxbxUtil.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/hasher.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/EmuShared.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/InlineFunc.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/IPCWindows.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/Mutex.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/Threads.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/Util.cpp"
 "${CXBXR_ROOT_DIR}/src/common/win32/WineEnv.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xbdm/CxbxXbdm.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xbe/Xbe.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xbe/XbePrinter.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xbox/Types.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xdvdfs-tools/buffered_io.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xdvdfs-tools/xdvdfs.cpp"
 "${CXBXR_ROOT_DIR}/src/CxbxVersion.cpp"
 "${CXBXR_ROOT_DIR}/src/HighPerformanceGraphicsEnabler.c"
)

# GUI v1
file (GLOB CXBXR_SOURCE_GUIv1
 "${CXBXR_ROOT_DIR}/src/common/input/Button.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/EmuDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/common/input/InputWindow.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/controllers/DlgDukeControllerConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/controllers/DlgSBControllerConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgAbout.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgAudioConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgInputConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgEepromConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgLoggingConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgNetworkConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/DlgVideoConfig.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/WinMain.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/Wnd.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/WndMain.cpp"
)

# Emulator (module)
# TODO: Merge CXBXR_KRNL_CPP into CXBXR_SOURCE_EMU once cxbx project is removed.
file (GLOB CXBXR_KRNL_CPP
 "${CXBXR_ROOT_DIR}/src/core/kernel/init/CxbxKrnl.cpp"
)
file (GLOB CXBXR_SOURCE_EMU_IMPORT
 "${CXBXR_ROOT_DIR}/import/imgui/backends/imgui_impl_dx9.cpp"
 "${CXBXR_ROOT_DIR}/import/imgui/backends/imgui_impl_opengl3.cpp"
 "${CXBXR_ROOT_DIR}/import/imgui/backends/imgui_impl_win32.cpp"
)
file (GLOB CXBXR_SOURCE_EMU
 "${CXBXR_KRNL_CPP}"
 "${CXBXR_ROOT_DIR}/src/common/AddressRanges.cpp"
 "${CXBXR_ROOT_DIR}/src/common/VerifyAddressRanges.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/gloffscreen/glextensions.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/gloffscreen/gloffscreen_common.cpp"
 "${CXBXR_ROOT_DIR}/src/common/util/gloffscreen/gloffscreen_wgl.cpp"
 "${CXBXR_ROOT_DIR}/src/common/xbox/Logging.cpp"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/audio.cpp"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/ui.cpp"
 "${CXBXR_ROOT_DIR}/src/core/common/imgui/video.cpp"
 "${CXBXR_ROOT_DIR}/src/core/common/video/RenderBase.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/Direct3D9.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/PixelShader.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/RenderStates.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/Shader.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/TextureStates.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/VertexShader.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/VertexShaderSource.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/Direct3D9/WalkIndexBuffer.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/FixedFunctionState.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/ResourceTracker.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbConvert.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbD3D8Logging.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbPixelShader.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbPushBuffer.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbVertexBuffer.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/D3D8/XbVertexShader.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSound.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSound3DCalculator.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundBuffer.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundGlobal.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundLogging.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DirectSoundStream.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/DSStream_PacketManager.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/DirectSound/XFileMediaObject.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/XbDSoundLogging.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/common/XbInternalDSVoice.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/DSOUND/common/XbInternalStruct.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/Intercept.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/Patches.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/XACTENG/XactEng.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/XAPI/Xapi.cpp"
 "${CXBXR_ROOT_DIR}/src/core/hle/XGRAPHIC/XGraphic.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnl.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlAv.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlDbg.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlEx.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlFs.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlHal.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlIo.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlKd.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlKe.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlKi.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlLogging.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlMm.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlNt.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlOb.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlPhy.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlPs.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlRtl.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlXbox.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlXc.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/EmuKrnlXe.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/exports/KernelThunk.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/memory-manager/PhysicalMemory.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/memory-manager/PoolManager.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/memory-manager/VMManager.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/Emu.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/EmuFile.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/EmuFS.cpp"
 "${CXBXR_ROOT_DIR}/src/core/kernel/support/EmuNtDll.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/ADM1032Device.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/EEPROMDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/network/NVNetDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/MCPXDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/PCIBus.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/PCIDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/SMBus.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/SMCDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/SMDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/usb/Hub.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/usb/OHCI.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/usb/USBDevice.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/usb/XidGamepad.cpp"

 #NOTE: These files are direct includes inside nv2a.cpp file. If we comment them out, we will have compile errors.
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_DEBUG.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PBUS.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PCOUNTER.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PCRTC.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PFB.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PFIFO.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PGRAPH.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PMC.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRAMDAC.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRAMIN.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRMA.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRMCIO.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRMDIO.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRMFB.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PRMVIO.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PSTRAPS.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PTIMER.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PTV.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PVIDEO.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_PVPE.cpp"
 #"${CXBXR_ROOT_DIR}/src/devices/video/EmuNV2A_USER.cpp"

 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_debug.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_psh.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_shaders.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/video/nv2a_vsh.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/video/qemu-thread-win32.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/video/swizzle.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/x86/EmuX86.cpp"
 "${CXBXR_ROOT_DIR}/src/devices/Xbox.cpp"
 # Temporary usage for need ReserveAddressRanges func with cxbx.exe's emulation.
 "${CXBXR_ROOT_DIR}/src/common/ReserveAddressRanges.cpp"
)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/projects/cxbx")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/projects/cxbxr-ldr")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/projects/cxbxr-emu")

set(cxbxr_INSTALL_files "COPYING" "README.md")

# Cxbx-Reloaded project with third-party libraries
set_target_properties(cxbx cxbxr-ldr cxbxr-emu misc-batch SDL2 subhook libXbSymbolDatabase libtommath libtomcrypt imgui
 PROPERTIES FOLDER Cxbx-Reloaded
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 # Configure startup project
 set_property(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" PROPERTY VS_STARTUP_PROJECT cxbx)
endif()

# Check if generator is Visual Studio then enable Cxbxr-Debugger project.
# Since C# is currently supported with Visual Studio for now.
if(${CMAKE_GENERATOR} MATCHES "Visual Studio ([^9]|[9][0-9])")
 # Issues with compile (the same with develop branch) and
 # for some reason did not put the files into virtual folder?
 # Might need to put the list in the source folder for workaround fix.
 add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/CxbxDebugger")

 set_target_properties(Tests_cs_x86
  PROPERTIES EXCLUDE_FROM_ALL TRUE
 )

 # Cxbx-Debugger project with third-party libraries
 set_target_properties(cxbxr-debugger cs_x86 Tests_cs_x86 capstone-shared
  PROPERTIES FOLDER Cxbx-Reloaded/debugger
 )
endif()

install(FILES ${cxbxr_INSTALL_files}
  DESTINATION bin
)

install(FILES
 "${CMAKE_SOURCE_DIR}/src/core/hle/D3D8/Direct3D9/CxbxPixelShaderTemplate.hlsl"
 "${CMAKE_SOURCE_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionPixelShader.hlsl"
 "${CMAKE_SOURCE_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionPixelShader.hlsli"
 "${CMAKE_SOURCE_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionVertexShaderState.hlsli"
 "${CMAKE_SOURCE_DIR}/src/core/hle/D3D8/Direct3D9/FixedFunctionVertexShader.hlsl"
 DESTINATION bin/hlsl
)

set(cxbxr_GLEW_DLL "${CMAKE_SOURCE_DIR}/import/glew-2.0.0/bin/Release/Win32/glew32.dll")

install(PROGRAMS ${cxbxr_GLEW_DLL}
  DESTINATION bin
)

install(PROGRAMS $<TARGET_FILE_DIR:cxbx>/${CMAKE_SHARED_LIBRARY_PREFIX}subhook${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION bin
)

install(PROGRAMS $<TARGET_FILE_DIR:cxbx>/${CMAKE_SHARED_LIBRARY_PREFIX}SDL2${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION bin
)
