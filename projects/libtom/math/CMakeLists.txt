cmake_minimum_required (VERSION 3.12)
project(libtommath)

# Suppress extra stuff from generated solution
set(CMAKE_SUPPRESS_REGENERATION true)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
 add_compile_definitions(
 _CRT_SECURE_NO_WARNINGS
 _CRT_NONSTDC_NO_DEPRECATE
 )
 # Suppress warnings in external code
 add_compile_options(
 /wd4146 # unary minus on unsigned type
 )
endif()

file (GLOB HEADERS
 "${CXBXR_ROOT_DIR}/import/libtommath/*.h"
)

file (GLOB SOURCES
 "${CXBXR_ROOT_DIR}/import/libtommath/*.c"
)

source_group(TREE ${CXBXR_ROOT_DIR}/import/libtommath PREFIX header FILES ${HEADERS})

source_group(TREE ${CXBXR_ROOT_DIR}/import/libtommath PREFIX source FILES ${SOURCES})

add_library(libtommath ${HEADERS} ${SOURCES})

target_include_directories(libtommath
 PUBLIC "${CXBXR_ROOT_DIR}/import/libtommath"
)
