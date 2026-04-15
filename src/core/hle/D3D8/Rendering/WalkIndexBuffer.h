#ifndef WALKINDEXBUFFER_H
#define WALKINDEXBUFFER_H

#include "core/hle/D3D8/XbD3D8Types.h"

extern void(*WalkIndexBuffer)
(
	INDEX16 &LowIndex,
	INDEX16 &HighIndex,
	INDEX16 *pIndexData,
	DWORD dwIndexCount
);

#endif
