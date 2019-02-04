cmake_minimum_required (VERSION 3.12)
project(CxbxLoader)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# Suppress extra stuff from generated solution
set(CMAKE_SUPPRESS_REGENERATION true)

# Force exclude default libraries being included
set(CMAKE_CXX_STANDARD_LIBRARIES "")

# Force set BasicRunTimeChecks to default
STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 add_compile_definitions(
 _CRT_SECURE_NO_WARNINGS
 )
 add_compile_options(
  /sdl-
 )
endif()

file (GLOB HEADERS
 "${CXBXR_ROOT_DIR}/src/common/AddressRanges.h"
 "${CXBXR_ROOT_DIR}/src/common/ReserveAddressRanges.h"
)

file (GLOB SOURCES
 "${CXBXR_ROOT_DIR}/src/common/LoaderTooling.cpp"
 "${CXBXR_ROOT_DIR}/src/common/ReserveAddressRanges.cpp"
 "${CXBXR_ROOT_DIR}/src/CxbxLoader/loader.cpp"
)

source_group(TREE ${CXBXR_ROOT_DIR}/src PREFIX header FILES ${HEADERS})

source_group(TREE ${CXBXR_ROOT_DIR}/src PREFIX source FILES ${SOURCES})

add_executable(CxbxLoader ${HEADERS} ${SOURCES})

# Link and compile flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

 set_target_properties(CxbxLoader PROPERTIES
  LINK_FLAGS "
  /LARGEADDRESSAWARE
  /FIXED
  /DYNAMICBASE:NO
  /BASE:0x10000
  /STACK:65536,65536
  /NODEFAULTLIB
  /ENTRY:rawMain
  "
 )

 # Set optimization options for release build
 set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}
  /Oi
  "
 )
endif()

target_link_libraries(CxbxLoader
 PUBLIC kernel32.lib
)
