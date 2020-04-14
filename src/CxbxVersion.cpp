// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Version.h"
#include "CxbxVersion.h"

/*! version string dependent on trace flag */
#ifndef _DEBUG_TRACE
const char* CxbxVersionStr = _GIT_VERSION " (" __DATE__  ")";
#else
const char* CxbxVersionStr = _GIT_VERSION "-Trace (" __DATE__  ")";
#endif
