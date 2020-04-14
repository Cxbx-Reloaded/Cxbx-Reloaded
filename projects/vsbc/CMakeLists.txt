cmake_minimum_required (VERSION 3.8)
project(vsbc)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# Suppress extra stuff from generated solution
set(CMAKE_SUPPRESS_REGENERATION true)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 add_compile_definitions(_CRT_SECURE_NO_WARNINGS
 )
endif()

add_compile_definitions(CXBXVSBC_EXPORTS
)

file (GLOB HEADERS
 "${CXBXR_ROOT_DIR}/src/vsbc/CxbxVSBC.h"
 "${CXBXR_ROOT_DIR}/src/vsbc/DlgVirtualSBCFeedback.h"
 "${CXBXR_ROOT_DIR}/src/vsbc/stdafx.h"
 "${CXBXR_ROOT_DIR}/src/vsbc/targetver.h"
)

file (GLOB SOURCES
 "${CXBXR_ROOT_DIR}/src/vsbc/CxbxVSBC.cpp"
 "${CXBXR_ROOT_DIR}/src/vsbc/DlgVirtualSBCFeedback.cpp"
 "${CXBXR_ROOT_DIR}/src/vsbc/dllmain.cpp"
 "${CXBXR_ROOT_DIR}/src/vsbc/stdafx.cpp"
)

source_group(TREE ${CXBXR_ROOT_DIR}/src/vsbc PREFIX header FILES ${HEADERS})

source_group(TREE ${CXBXR_ROOT_DIR}/src/vsbc PREFIX source FILES ${SOURCES})

add_library(vsbc SHARED ${HEADERS} ${SOURCES})

set_target_properties(vsbc PROPERTIES
 OUTPUT_NAME CxbxVSBC
)
