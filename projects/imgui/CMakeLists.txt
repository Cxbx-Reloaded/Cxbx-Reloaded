cmake_minimum_required (VERSION 3.12)
project(imgui LANGUAGES CXX)
# Since imgui doesn't have CMake, we'll make an interface project here.

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 add_compile_definitions(
 _CRT_SECURE_NO_WARNINGS
 _CRT_NONSTDC_NO_DEPRECATE
 )
endif()

# Add any defines from imconfig.h file in here without need to edit import file directly.
add_compile_definitions(
 IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
 IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
)

file (GLOB HEADERS
 "${CXBXR_ROOT_DIR}/import/imgui/imconfig.h"
 "${CXBXR_ROOT_DIR}/import/imgui/imgui.h"
 "${CXBXR_ROOT_DIR}/import/imgui/imgui_internal.h"
 "${CXBXR_ROOT_DIR}/import/imgui/imstb_rectpack.h"
 "${CXBXR_ROOT_DIR}/import/imgui/imstb_textedit.h"
 "${CXBXR_ROOT_DIR}/import/imgui/imstb_truetype.h"
)

file (GLOB SOURCES
 "${CXBXR_ROOT_DIR}/import/imgui/imgui.cpp"
 "${CXBXR_ROOT_DIR}/import/imgui/imgui_draw.cpp"
 "${CXBXR_ROOT_DIR}/import/imgui/imgui_tables.cpp"
 "${CXBXR_ROOT_DIR}/import/imgui/imgui_widgets.cpp"
)

source_group(TREE ${CXBXR_ROOT_DIR}/import/imgui PREFIX header FILES ${HEADERS})

source_group(TREE ${CXBXR_ROOT_DIR}/import/imgui PREFIX source FILES ${SOURCES})

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
 PUBLIC "${CXBXR_ROOT_DIR}/import/imgui"
)
