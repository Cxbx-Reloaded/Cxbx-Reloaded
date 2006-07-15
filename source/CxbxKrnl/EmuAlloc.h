// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuAlloc.h
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUALLOC_H
#define EMUALLOC_H

#include <malloc.h>

#ifdef _DEBUG_ALLOC
#define CxbxMalloc(x)                            CxbxMallocDebug(x, __FILE__, __LINE__)
#define CxbxCalloc(x, y)                         CxbxCallocDebug(x, y, __FILE__, __LINE__)
#define CxbxFree(x)                              CxbxFreeDebug(x, __FILE__, __LINE__)
#define CxbxRtlAlloc(Heap, Flags, Bytes)         CxbxRtlAllocDebug(Heap, Flags, Bytes, __FILE__, __LINE__)
#define CxbxRtlFree(Heap, Flags, pMem)           CxbxRtlFreeDebug(Heap, Flags, pMem, __FILE__, __LINE__)
#define CxbxRtlRealloc(Heap, Flags, pMem, Bytes) CxbxRtlReallocDebug(Heap, Flags, pMem, Bytes, __FILE__, __LINE__)
#define CxbxRtlSizeHeap(Heap, Flags, pMem)       CxbxRtlSizeHeapDebug(Heap, Flags, pMem, __FILE__, __LINE__)

// ******************************************************************
// * CxbxMallocDebug - Debug track malloc
// ******************************************************************
void *CxbxMallocDebug(size_t  Size,
                      char   *pFile,
                      int     Line);

// ******************************************************************
// * CxbxCallocDebug - Debug track calloc
// ******************************************************************
void *CxbxCallocDebug(size_t  NbrElements,
                      size_t  ElementSize,
                      char   *pFile,
                      int     Line);

// ******************************************************************
// * CxbxFreeDebug - Debug track free
// ******************************************************************
void  CxbxFreeDebug(void *pMem,
                    char *pFile,
                    int   Line);

// ******************************************************************
// * CxbxRtlAllocDebug - Debug track RTL alloc
// ******************************************************************
void *CxbxRtlAllocDebug(HANDLE  Heap,
                        DWORD   Flags,
                        SIZE_T  Bytes,
                        char   *pFile,
                        int     Line);

// ******************************************************************
// * CxbxRtlFreeDebug - Debug track RTL free
// ******************************************************************
BOOL  CxbxRtlFreeDebug(HANDLE Heap,
                       DWORD  Flags,
                       PVOID  pMem,
                       char  *pFile,
                       int    Line);

// ******************************************************************
// * CxbxRtlReallocDebug - Debug track RTL realloc
// ******************************************************************
void *CxbxRtlReallocDebug(HANDLE Heap,
                          DWORD  Flags,
                          PVOID  pMem,
                          SIZE_T Bytes,
                          char  *pFile,
                          int    Line);

// ******************************************************************
// * CxbxRtlHeapSizeDebug - Debug track RTL heap size
// ******************************************************************
SIZE_T CxbxRtlSizeHeapDebug(HANDLE Heap,
                            DWORD  Flags,
                            PVOID  pMem,
                            char  *pFile,
                            int    Line);

// ******************************************************************
// * CxbxVirtualQueryDebug - Debug virtual query
// ******************************************************************
DWORD CxbxVirtualQueryDebug(LPCVOID                   lpAddress,
                            PMEMORY_BASIC_INFORMATION lpBuffer,
                            DWORD                     dwLength);

// ******************************************************************
// * CxbxAllocDump - Dump the memory allocations
// ******************************************************************
void CxbxAllocDump(bool DumpData);

#else // _DEBUG_ALLOC
#define CxbxMalloc(x)                            malloc(x)
#define CxbxCalloc(x, y)                         calloc(x, y)
#define CxbxFree(x)                              free(x)
#define CxbxRtlAlloc(Heap, Flags, Bytes)         NtDll::RtlAllocateHeap(Heap, Flags, Bytes)
#define CxbxRtlFree(Heap, Flags, pMem)           NtDll::RtlFreeHeap(Heap, Flags, pMem)
#define CxbxRtlRealloc(Heap, Flags, pMem, Bytes) NtDll::RtlReAllocateHeap(Heap, Flags, pMem, Bytes)
#define CxbxRtlSizeHeap(Heap, Flags, pMem)       NtDll::RtlSizeHeap(Heap, Flags, pMem)
#endif

#endif // EMUALLOC_H
