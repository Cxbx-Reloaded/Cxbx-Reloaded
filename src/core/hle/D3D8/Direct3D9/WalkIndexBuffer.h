#ifndef WALKINDEXBUFFER_H
#define WALKINDEXBUFFER_H

#include "core\kernel\support\Emu.h"

extern void(*WalkIndexBuffer)
(
	INDEX16 &LowIndex,
	INDEX16 &HighIndex,
	INDEX16 *pIndexData,
	DWORD dwIndexCount
);

#endif
