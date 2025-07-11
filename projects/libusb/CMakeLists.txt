cmake_minimum_required (VERSION 3.12)
project(libusb LANGUAGES CXX)
# Since libusb doesn't have CMake, we'll make an interface project here.

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 add_compile_definitions(
 _CRT_SECURE_NO_WARNINGS
 _CRT_NONSTDC_NO_DEPRECATE
 )
endif()

include_directories(
 "${CXBXR_ROOT_DIR}/import/libusb/msvc"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb")

file (GLOB HEADERS
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/libusb.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/libusbi.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/version.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/version_nano.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/msvc/config.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/events_windows.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/threads_windows.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/windows_common.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/windows_usbdk.h"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/windows_winusb.h"
)

file (GLOB SOURCES
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/core.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/descriptor.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/hotplug.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/io.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/strerror.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/sync.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/events_windows.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/threads_windows.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/windows_common.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/windows_usbdk.c"
 "${CXBXR_ROOT_DIR}/import/libusb/libusb/os/windows_winusb.c"
)

source_group(TREE ${CXBXR_ROOT_DIR}/import/libusb/libusb PREFIX header FILES ${HEADERS})

source_group(TREE ${CXBXR_ROOT_DIR}/import/libusb/libusb PREFIX source FILES ${SOURCES})

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
 PUBLIC "${CXBXR_ROOT_DIR}/import/libusb"
)
