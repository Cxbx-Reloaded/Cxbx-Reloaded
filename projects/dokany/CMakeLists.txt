cmake_minimum_required(VERSION 3.12)
project(dokan-import LANGUAGES C)

if(NOT MSVC)
 message(FATAL_ERROR "Dokany import library generation currently requires MSVC.")
endif()

set(DOKANY_ROOT_DIR "${CXBXR_ROOT_DIR}/import/dokany")
set(DOKANY_DEF_FILE "${DOKANY_ROOT_DIR}/dokan/dokan.def")
set(DOKANY_IMPORT_LIB "${CMAKE_CURRENT_BINARY_DIR}/dokan2.lib")

set(DOKANY_LIB_HINTS)
if(CMAKE_LINKER)
 get_filename_component(DOKANY_TOOL_DIR "${CMAKE_LINKER}" DIRECTORY)
 list(APPEND DOKANY_LIB_HINTS "${DOKANY_TOOL_DIR}")
endif()
if(CMAKE_C_COMPILER)
 get_filename_component(DOKANY_TOOL_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
 list(APPEND DOKANY_LIB_HINTS "${DOKANY_TOOL_DIR}")
endif()
if(CMAKE_CXX_COMPILER)
 get_filename_component(DOKANY_TOOL_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
 list(APPEND DOKANY_LIB_HINTS "${DOKANY_TOOL_DIR}")
endif()

find_program(DOKANY_LIB_EXECUTABLE NAMES lib.exe HINTS ${DOKANY_LIB_HINTS} REQUIRED)

set(DOKANY_MACHINE "${CMAKE_GENERATOR_PLATFORM}")
if(NOT DOKANY_MACHINE)
 if(CMAKE_VS_PLATFORM_NAME)
  set(DOKANY_MACHINE "${CMAKE_VS_PLATFORM_NAME}")
 elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(DOKANY_MACHINE "x64")
 else()
  set(DOKANY_MACHINE "Win32")
 endif()
endif()

if(DOKANY_MACHINE STREQUAL "Win32")
 set(DOKANY_MACHINE_FLAG "X86")
elseif(DOKANY_MACHINE STREQUAL "x64")
 set(DOKANY_MACHINE_FLAG "X64")
elseif(DOKANY_MACHINE STREQUAL "ARM")
 set(DOKANY_MACHINE_FLAG "ARM")
elseif(DOKANY_MACHINE STREQUAL "ARM64")
 set(DOKANY_MACHINE_FLAG "ARM64")
else()
 message(FATAL_ERROR "Unsupported Dokany machine platform: ${DOKANY_MACHINE}")
endif()

add_custom_command(
 OUTPUT "${DOKANY_IMPORT_LIB}"
 COMMAND "${DOKANY_LIB_EXECUTABLE}" /nologo /def:${DOKANY_DEF_FILE} /machine:${DOKANY_MACHINE_FLAG} /out:${DOKANY_IMPORT_LIB}
 DEPENDS "${DOKANY_DEF_FILE}"
 VERBATIM
)

add_custom_target(dokan-import-lib DEPENDS "${DOKANY_IMPORT_LIB}" SOURCES
 "${DOKANY_ROOT_DIR}/README.Cxbx.md"
 "${DOKANY_ROOT_DIR}/LICENSE.LGPL.txt"
 "${DOKANY_ROOT_DIR}/LICENSE.MIT.txt"
 "${DOKANY_ROOT_DIR}/dokan/dokan.def"
 "${DOKANY_ROOT_DIR}/dokan/dokan.h"
 "${DOKANY_ROOT_DIR}/dokan/dokanc.h"
 "${DOKANY_ROOT_DIR}/dokan/fileinfo.h"
 "${DOKANY_ROOT_DIR}/sys/public.h"
)

add_library(dokan INTERFACE)
add_library(dokan::dokan ALIAS dokan)
add_dependencies(dokan dokan-import-lib)

target_include_directories(dokan
 INTERFACE
 "${DOKANY_ROOT_DIR}/dokan"
 "${DOKANY_ROOT_DIR}/sys"
)

target_link_libraries(dokan
 INTERFACE
 "${DOKANY_IMPORT_LIB}"
)
