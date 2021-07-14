#pragma once

extern const char* CxbxVersionStr;
extern const char *CxbxrHashBuild;

// Note: GitVersionMaxLength should be large enough to accomodate the longest git version string we can practically expect to have. This is necessary
// to avoid possible mismatches in the string length which can happen if the user mixes different cxbxr versions
inline constexpr size_t GitVersionMaxLength = 80;
const char *const GetGitVersionStr();
const size_t GetGitVersionLength();
