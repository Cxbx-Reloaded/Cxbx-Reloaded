#pragma once

#include "version.h"

/*! version string dependent on trace flag */
#ifndef _DEBUG_TRACE
#define _CXBX_VERSION _GIT_VERSION " (" __DATE__  ")"
#else
#define _CXBX_VERSION _GIT_VERSION "-Trace (" __DATE__  ")"
#endif
