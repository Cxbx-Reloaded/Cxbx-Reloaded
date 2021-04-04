#pragma once

#include "version.h"
#include <string>


extern const char* CxbxVersionStr;

// Note: GitVersionMaxLength should be large enough to accomodate the longest git version string we can practically expect to have. This is necessary
// to avoid possible mismatches in the string length which can happen if the user mixes different cxbxr versions
inline constexpr const char *CxbxGitVersion = _GIT_VERSION;
inline constexpr size_t GitVersionLength = std::char_traits<char>::length(CxbxGitVersion);
inline constexpr size_t GitVersionMaxLength = 80;
static_assert(GitVersionLength < GitVersionMaxLength);
