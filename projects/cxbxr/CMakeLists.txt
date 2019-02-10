cmake_minimum_required (VERSION 3.12)
project(cxbxr)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# Suppress extra stuff from generated solution
set(CMAKE_SUPPRESS_REGENERATION true)

include_directories(
 "${CXBXR_ROOT_DIR}/src"
 "${CXBXR_ROOT_DIR}/src/common"
 "${CXBXR_ROOT_DIR}/src/common/Win32"
 "${CXBXR_ROOT_DIR}/import/OpenXDK/include"
 "${CXBXR_ROOT_DIR}/import/XbSymbolDatabase"
 "${CXBXR_ROOT_DIR}/import/simpleini"
 "${CXBXR_ROOT_DIR}/import/DirectX9/include"
 "${CXBXR_ROOT_DIR}/import/libtommath"
 "${CXBXR_ROOT_DIR}/import/libtomcrypt/src/headers"
)

link_directories(
 "${CXBXR_ROOT_DIR}/import/DirectX9/lib"
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 add_compile_definitions(
 _CRT_SECURE_NO_WARNINGS
  # Windows 7 minimum requirement
  _WIN32_WINNT=0x0601

  LTM_DESC
  USE_LTM
  LTC_NO_TEST
  LTC_NO_CIPHERS
  LTC_NO_HASHES
  LTC_NO_MACS
  LTC_NO_PRNGS
  LTC_NO_MISC
  LTC_NO_PROTOTYPES
 )
 add_compile_options(
  /EHs
  /MP
  /GF
  /arch:SSE2
 )
endif()

add_compile_definitions(NOMINMAX
)

file (GLOB RESOURCES
 
 "${CXBXR_ROOT_DIR}/CONTRIBUTORS"
 "${CXBXR_ROOT_DIR}/COPYING"
 "${CXBXR_ROOT_DIR}/README.md"
 "${CXBXR_ROOT_DIR}/src/gui/resource/.editorconfig"
 "${CXBXR_ROOT_DIR}/src/gui/resource/cxbxr.rc"
 "${CXBXR_ROOT_DIR}/src/gui/resource/Cxbx-R.ico"
 "${CXBXR_ROOT_DIR}/src/gui/resource/resource.h"
 "${CXBXR_ROOT_DIR}/src/gui/resource/stdafx.h"
 "${CXBXR_ROOT_DIR}/src/gui/resource/targetver.h"
 "${CXBXR_ROOT_DIR}/src/gui/resource/Logo.bmp"
 "${CXBXR_ROOT_DIR}/src/gui/resource/Logo-License-CC4.bmp"
 "${CXBXR_ROOT_DIR}/src/.editorconfig"
)

source_group(TREE ${CXBXR_ROOT_DIR}/src PREFIX header FILES
 ${CXBXR_HEADER_GUIv1}
 ${CXBXR_HEADER_COMMON}
 "${CXBXR_ROOT_DIR}/src/gui/cxbxr.h"
)

source_group(TREE ${CXBXR_ROOT_DIR}/src PREFIX source FILES 
 ${CXBXR_SOURCE_GUIv1}
 ${CXBXR_SOURCE_COMMON}
 "${CXBXR_ROOT_DIR}/src/gui/cxbxr.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/stdafx.cpp"
)

source_group(TREE ${CXBXR_ROOT_DIR} FILES ${RESOURCES})

add_executable(cxbxr WIN32 ${RESOURCES}
# ${CXBXR_HEADER_GUIv1}
# ${CXBXR_HEADER_COMMON}
# ${CXBXR_SOURCE_GUIv1}
# ${CXBXR_SOURCE_COMMON}
 ${CXBXR_GIT_VERSION_H}
 "${CXBXR_ROOT_DIR}/src/gui/cxbxr.h"
 "${CXBXR_ROOT_DIR}/src/gui/cxbxr.cpp"
 "${CXBXR_ROOT_DIR}/src/gui/stdafx.cpp"
)

# Link and compile flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

 set_target_properties(cxbxr PROPERTIES
  LINK_FLAGS "
  /INCREMENTAL:NO
  /LARGEADDRESSAWARE
  /SAFESEH:NO
  /NODEFAULTLIB:libcmt
  "
  LINK_FLAGS_RELEASE "
  /LTCG
  "
 )

 # Set optimization options for release build
 set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}
  /Zi
  /Ob2
  /Oi
  /Ot
  /GL

  /GS-
  /Gy
  /Qpar
  "
 )
endif()

# Windows libraries
set(WINS_LIB 
 legacy_stdio_definitions
 d3d9
 dinput8
 dxguid
 odbc32
 odbccp32
 Shlwapi
 dxerr9
 ws2_32
 dsound
 winmm
 ddraw
 d3dx9
 dbghelp
 comctl32
 XINPUT9_1_0
)

target_link_libraries(cxbxr
 PUBLIC XbSymbolDatabase
 libtomcrypt

 ${WINS_LIB}
)

add_dependencies(cxbxr cxbxr-emu cxbxr-debugger)
