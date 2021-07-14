// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Version.h"
#include "CxbxVersion.h"
#include <string>

/*! version string dependent on trace flag */
#ifndef _DEBUG_TRACE
const char* CxbxVersionStr = _GIT_VERSION " (" __DATE__  ")";
const char *CxbxrHashBuild = _GIT_VERSION;
#else
const char* CxbxVersionStr = _GIT_VERSION "-Trace (" __DATE__  ")";
const char *CxbxrHashBuild = _GIT_VERSION "-Trace";
#endif

static constexpr const char *GitVersionStr = _GIT_VERSION;
static constexpr size_t GitVersionLength = std::char_traits<char>::length(GitVersionStr);
static_assert(GitVersionLength < GitVersionMaxLength);

const char *const GetGitVersionStr() {
    return GitVersionStr;
}

const size_t GetGitVersionLength() {
    return GitVersionLength;
}
