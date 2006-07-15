// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuAlloc.cpp
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

#include "Emu.h"
#include "EmuAlloc.h"

#ifdef _DEBUG_ALLOC

#include "mutex.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

// ******************************************************************
// * Memory tracking stuff
// ******************************************************************

static const uint32 MEMORY_GUARD = 0xDEADFADE;

typedef enum
{
    CXBX_ALLOC_NORMAL,
    CXBX_ALLOC_RTL
}
CXBX_ALLOC_TYPE;

typedef struct _CXBX_MEMORY_BLOCK
{
    void                      *pMem;
    size_t                     Size;
    char                      *pFile;
    uint32                     Line;
    CXBX_ALLOC_TYPE            Type;
    struct _CXBX_MEMORY_BLOCK *pNext;
}
CXBX_MEMORY_BLOCK;

static CXBX_MEMORY_BLOCK *g_pFirstBlock = NULL;
static CXBX_MEMORY_BLOCK *g_pLastBlock  = NULL;
static Mutex g_MemoryMutex;

// ******************************************************************
// * GetMemStart - Retrieves the actual start of the allocated memory
// *               block (first guard)
// ******************************************************************
static inline void *GetMemStart(CXBX_MEMORY_BLOCK *pBlock)
{
    return (void*)((char*)pBlock->pMem - sizeof(MEMORY_GUARD));
}

// ******************************************************************
// * GetMemEnd - Retrieves the end of the allocated memory block
// *             (second guard)
// ******************************************************************
static inline void *GetMemEnd(CXBX_MEMORY_BLOCK *pBlock)
{
    return (void*)((char*)pBlock->pMem + pBlock->Size);
}

// ******************************************************************
// * CheckIntegrity - Prints if the memory block is overwritten
// ******************************************************************
static bool CheckIntegrity(CXBX_MEMORY_BLOCK *pBlock)
{
    bool Integrity = true;

    if(*(uint32*)GetMemStart(pBlock) != MEMORY_GUARD)
    {
        printf("    Memory block corrupted at start, overwrite: 0x%.04X\n",
               *(uint32*)GetMemStart(pBlock));
        Integrity = false;
    }

    if(*(uint32*)GetMemEnd(pBlock) != MEMORY_GUARD)
    {
        printf("    Memory block corrupted at end, overwrite: 0x.04X\n",
               *(uint32*)GetMemEnd(pBlock));
        Integrity = false;
    }

    return Integrity;
}

// ******************************************************************
// * IsThisMemoryBlock - Simple block matching function
// ******************************************************************
static inline bool IsThisMemoryBlock(void              *pMem,
                                     CXBX_MEMORY_BLOCK *pBlock)
{
    return (pBlock && pMem == pBlock->pMem);
}

// ******************************************************************
// * InThisMemoryBlock - Simple block matching function
// ******************************************************************
static inline bool InThisMemoryBlock(const void        *pMem,
                                     CXBX_MEMORY_BLOCK *pBlock)
{
    return (pBlock && pMem >= pBlock->pMem && pMem <= GetMemEnd(pBlock));
}


// ******************************************************************
// * InsertMemoryBlock - Inserts a new memory block in the tracker
// ******************************************************************
static CXBX_MEMORY_BLOCK *InsertMemoryBlock(void           *pMem,
                                            size_t          Size,
                                            char           *pFile,
                                            int             Line,
                                            CXBX_ALLOC_TYPE Type)
{
    CXBX_MEMORY_BLOCK *pBlock = (CXBX_MEMORY_BLOCK*)
                                malloc(sizeof(CXBX_MEMORY_BLOCK));
    pBlock->pMem = (uint08*)pMem + sizeof(MEMORY_GUARD);
    pBlock->Size = Size;
    size_t Length = strlen(pFile) + 1;
    pBlock->pFile = (char*)malloc(Length);
    memcpy(pBlock->pFile, pFile, Length);
    pBlock->pNext = NULL;
    pBlock->Line = Line;
    pBlock->Type = Type;
    *(uint32*)(GetMemStart(pBlock)) = MEMORY_GUARD;
    *(uint32*)(GetMemEnd(pBlock)) = MEMORY_GUARD;

    if(g_pFirstBlock == NULL)
    {
        g_pFirstBlock = pBlock;
        g_pLastBlock = pBlock;
    }
    else
    {
        g_pLastBlock->pNext = pBlock;
        g_pLastBlock = pBlock;
    }

    return pBlock;
}

// ******************************************************************
// * RemoveMemoryBlock - Removes a memory block from the tracker
// ******************************************************************
static CXBX_MEMORY_BLOCK *RemoveMemoryBlock(void *pMem)
{
    CXBX_MEMORY_BLOCK *pFree = NULL;
    if(IsThisMemoryBlock(pMem, g_pFirstBlock))
    {
        pFree = g_pFirstBlock;
        g_pFirstBlock = g_pFirstBlock->pNext;
        if(pFree == g_pLastBlock)
        {
            g_pLastBlock = NULL;
        }
    }
    else
    {
        CXBX_MEMORY_BLOCK *pCur;
        CXBX_MEMORY_BLOCK *pPrev = NULL;
        for(pCur = g_pFirstBlock; pCur; pCur = pCur->pNext)
        {
            if(IsThisMemoryBlock(pMem, pCur))
            {
                if(pCur == g_pLastBlock)
                {
                    g_pLastBlock = pPrev;
                }
                pFree = pCur;
                pPrev->pNext = pCur->pNext;
                break;
            }
            pPrev = pCur;
        }
    }
    return pFree;
}

// ******************************************************************
// * FindMemoryBlock - Finds a memory block in the tracker
// ******************************************************************
static CXBX_MEMORY_BLOCK *FindMemoryBlock(void *pMem)
{
    CXBX_MEMORY_BLOCK *pCur;

    for(pCur = g_pFirstBlock; pCur; pCur = pCur->pNext)
    {
        if(IsThisMemoryBlock(pMem, pCur))
        {
            return pCur;
        }
    }

    return NULL;
}

// ******************************************************************
// * FindMemoryBlockIn - Finds a memory block in the tracker
// ******************************************************************
static CXBX_MEMORY_BLOCK *FindMemoryBlockIn(const void *pMem)
{
    CXBX_MEMORY_BLOCK *pCur;

    for(pCur = g_pFirstBlock; pCur; pCur = pCur->pNext)
    {
        if(InThisMemoryBlock(pMem, pCur))
        {
            return pCur;
        }
    }

    return NULL;
}

// ******************************************************************
// * CxbxAllocDump - Dump the memory allocations
// ******************************************************************
void CxbxAllocDump(bool DumpData)
{
    g_MemoryMutex.Lock();

    CXBX_MEMORY_BLOCK *pCur;
    printf("******************************************************\n"
           "* Dumping memory allocations                         *\n"
           "******************************************************\n");
    for(pCur = g_pFirstBlock; pCur; pCur = pCur->pNext)
    {
        printf("\n"
               "    Block: 0x%.08X\n"
               "    Size : %d\n"
               "    File : %s\n"
               "    Line : %d\n"
               "    Type : %s\n",
               pCur->pMem, pCur->Size, pCur->pFile, pCur->Line,
               pCur->Type == CXBX_ALLOC_NORMAL ? "NORMAL" : "RTL");
        CheckIntegrity(pCur);
    }

    g_MemoryMutex.Unlock();
}

// ******************************************************************
// * CxbxMallocDebug - Debug track malloc
// ******************************************************************
void *CxbxMallocDebug(size_t  Size,
                      char   *pFile,
                      int     Line)
{
    void *pRetMem = NULL;
    g_MemoryMutex.Lock();

    void *pMem = malloc(Size + 2 * sizeof(MEMORY_GUARD));
    if(!pMem)
    {
        printf("CxbxMallocDebug: Allocation failed\n"
               "    Size: %d\n"
               "    File: %s\n"
               "    Line: %d\n",
               Size, pFile, Line);
    }
    else
    {
        CXBX_MEMORY_BLOCK *pBlock = InsertMemoryBlock(pMem,
                                                      Size,
                                                      pFile,
                                                      Line,
                                                      CXBX_ALLOC_NORMAL);
        pRetMem = pBlock->pMem;
    }

    g_MemoryMutex.Unlock();

    return pRetMem;
}

// ******************************************************************
// * CxbxCallocDebug - Debug track calloc
// ******************************************************************
void *CxbxCallocDebug(size_t  NbrElements,
                      size_t  ElementSize,
                      char   *pFile,
                      int     Line)
{
    void *pRetMem = NULL;
    g_MemoryMutex.Lock();

    void *pMem = calloc(NbrElements * ElementSize + 2 * sizeof(MEMORY_GUARD), 1);
    if(!pMem)
    {
        printf("CxbxCallocDebug: Allocation failed\n"
               "    NbrElements: %d\n"
               "    ElementSize: %d\n"
               "    File       : %s\n"
               "    Line       : %d\n",
               NbrElements, ElementSize, pFile, Line);
    }
    else
    {
        CXBX_MEMORY_BLOCK *pBlock = InsertMemoryBlock(pMem,
                                                      NbrElements * ElementSize,
                                                      pFile,
                                                      Line,
                                                      CXBX_ALLOC_NORMAL);
        pRetMem = pBlock->pMem;
    }

    g_MemoryMutex.Unlock();

    return pRetMem;
}

// ******************************************************************
// * CxbxFreeDebug - Debug track free
// ******************************************************************
void  CxbxFreeDebug(void *pMem,
                    char *pFile,
                    int   Line)
{
    if (pMem == NULL)
    {
        return;
    }
    g_MemoryMutex.Lock();

    CXBX_MEMORY_BLOCK *pFree = RemoveMemoryBlock(pMem);
    if(pFree == NULL)
    {
        printf("CxbxFreeDebug: free on non-existent block: 0x%.08X! "
               "Possibly a multiple free.\n"
               "    File: %s\n"
               "    Line: %d\n",
               pMem, pFile, Line);
    }
    else
    {
        if(!CheckIntegrity(pFree))
        {
            printf("CxbxFreeDebug: Free on damaged block\n"
                   "    Block   : 0x.%08X\n"
                   "    Allocation\n"
                   "        File: %s\n"
                   "        Line: %d\n"
                   "    Free\n"
                   "        File: %s\n"
                   "        Line: %d\n",
                   pFree->pMem, pFree->pFile, pFree->Line, pFile, Line);
        }
        free(GetMemStart(pFree));
        free(pFree->pFile);
        free(pFree);
    }

    g_MemoryMutex.Unlock();
}

// ******************************************************************
// * CxbxRtlAllocDebug - Debug track RTL alloc
// ******************************************************************
void *CxbxRtlAllocDebug(HANDLE  Heap,
                        DWORD   Flags,
                        SIZE_T  Bytes,
                        char   *pFile,
                        int     Line)
{
    void *pRetMem = NULL;
    g_MemoryMutex.Lock();

    void *pMem = NtDll::RtlAllocateHeap(Heap, Flags, Bytes + 2 * sizeof(MEMORY_GUARD));
    if(!pMem)
    {
        printf("CxbxRtlAllocDebug: Allocation failed\n"
               "    Heap  : 0x%.08X\n"
               "    Flags : 0x%.08X\n"
               "    Bytes : %d\n"
               "    File  : %s\n"
               "    Line  : %d\n",
               Heap, Flags, Bytes, pFile, Line);
    }
    else
    {
        CXBX_MEMORY_BLOCK *pBlock = InsertMemoryBlock(pMem,
                                                      Bytes,
                                                      pFile,
                                                      Line,
                                                      CXBX_ALLOC_RTL);
        pRetMem = pBlock->pMem;
    }

    g_MemoryMutex.Unlock();

    return pRetMem;
}

// ******************************************************************
// * CxbxRtlFreeDebug - Debug track RTL free
// ******************************************************************
BOOL  CxbxRtlFreeDebug(HANDLE Heap,
                       DWORD  Flags,
                       PVOID  pMem,
                       char  *pFile,
                       int    Line)
{
    BOOL Ret = FALSE;
    if (pMem == NULL)
    {
        return TRUE;
    }
    g_MemoryMutex.Lock();

    CXBX_MEMORY_BLOCK *pFree = RemoveMemoryBlock(pMem);
    if(pFree == NULL)
    {
        printf("CxbxRtlFreeDebug: free on non-existent block: 0x%.08X! "
               "Possibly a multiple free.\n"
               "    File: %s\n"
               "    Line: %d\n",
               pMem, pFile, Line);
    }
    else
    {
        if(!CheckIntegrity(pFree))
        {
            printf("CxbxRtlFreeDebug: Free on damaged block\n"
                   "    Block   : 0x.%08X\n"
                   "    Allocation\n"
                   "        File: %s\n"
                   "        Line: %d\n"
                   "    Free\n"
                   "        File: %s\n"
                   "        Line: %d\n",
                   pFree->pMem, pFree->pFile, pFree->Line, pFile, Line);
        }
        Ret = NtDll::RtlFreeHeap(Heap, Flags, GetMemStart(pFree));
        free(pFree->pFile);
        free(pFree);
    }

    g_MemoryMutex.Unlock();
    return Ret;
}

// ******************************************************************
// * CxbxRtlReallocDebug - Debug track RTL realloc
// ******************************************************************
void *CxbxRtlReallocDebug(HANDLE Heap,
                          DWORD  Flags,
                          PVOID  pMem,
                          SIZE_T Bytes,
                          char  *pFile,
                          int    Line)
{
    void *pRetMem = NULL;
    g_MemoryMutex.Lock();

    CXBX_MEMORY_BLOCK *pRealloc = FindMemoryBlock(pMem);
    if(pRealloc == NULL)
    {
        printf("CxbxRtlRealloc: realloc on non-existent block: 0x%.08X! "
               "    File: %s\n"
               "    Line: %d\n",
               pMem, pFile, Line);
    }
    else
    {
        if(!CheckIntegrity(pRealloc))
        {
            printf("CxbxRtlReallocDebug: Realloc on damaged block\n"
                   "    Block   : 0x.%08X\n"
                   "    Allocation\n"
                   "        Size: %d\n"
                   "        File: %s\n"
                   "        Line: %d\n"
                   "    Reallocation\n"
                   "        Size: %d\n"
                   "        File: %s\n"
                   "        Line: %d\n",
                   pRealloc->pMem,
                   pRealloc->pFile, pRealloc->Size, pRealloc->Line,
                   Bytes, pFile, Line);
        }
        void *pNewMem = NtDll::RtlReAllocateHeap(Heap, Flags, GetMemStart(pRealloc), Bytes + 2 * sizeof(MEMORY_GUARD));
        free(pRealloc->pFile);
        free(pRealloc);
        if(!pNewMem)
        {
            printf("CxbxRtlReallocDebug: Reallocation failed\n"
                   "    Heap  : 0x%.08X\n"
                   "    Flags : 0x%.08X\n"
                   "    pMem  : 0x%.08X\n"
                   "    Bytes : %d\n"
                   "    File  : %s\n"
                   "    Line  : %d\n",
                   Heap, Flags, pMem, Bytes, pFile, Line);
        }
        else
        {
            CXBX_MEMORY_BLOCK *pBlock = InsertMemoryBlock(pNewMem,
                                                          Bytes,
                                                          pFile,
                                                          Line,
                                                          CXBX_ALLOC_RTL);
            pRetMem = pBlock->pMem;
        }
    }

    g_MemoryMutex.Unlock();
    return pRetMem;
}

// ******************************************************************
// * CxbxRtlSizeHeapDebug - Debug track RTL heap size
// ******************************************************************
SIZE_T CxbxRtlSizeHeapDebug(HANDLE Heap,
                            DWORD  Flags,
                            PVOID  pMem,
                            char  *pFile,
                            int    Line)
{
    SIZE_T Size = 0;
    g_MemoryMutex.Lock();

    CXBX_MEMORY_BLOCK *pBlock = FindMemoryBlock(pMem);
    if(pBlock == NULL)
    {
        printf("CxbxRtlSizeHeap: size heap on non-existent block: 0x%.08X! "
               "    File: %s\n"
               "    Line: %d\n",
               pMem, pFile, Line);
    }
    else
    {
        SIZE_T ActualSize = NtDll::RtlSizeHeap(Heap, Flags, GetMemStart(pBlock))
                            - 2 * sizeof(MEMORY_GUARD);
        if(ActualSize != pBlock->Size)
        {
            printf("CxbxRtlSizeHeap: heap size mismatch, RtlSizeHeap: %d Tracker: %d\n"
                   "    File  : %s\n"
                   "    Line  : %d\n",
                   ActualSize,
                   pBlock->Size,
                   pFile,
                   Line);
        }
        Size = ActualSize;
    }

    g_MemoryMutex.Unlock();
    return Size;
}

// ******************************************************************
// * CxbxVirtualQueryDebug - Debug virtual query
// ******************************************************************
DWORD CxbxVirtualQueryDebug(LPCVOID                   lpAddress,
                            PMEMORY_BASIC_INFORMATION lpBuffer,
                            DWORD                     dwLength)
{
    DWORD Size = 0;
    g_MemoryMutex.Lock();

    lpBuffer->State = MEM_COMMIT;

    CXBX_MEMORY_BLOCK *pBlock = FindMemoryBlockIn(lpAddress);

    if (pBlock)
    {
        Size = dwLength;
        lpBuffer->RegionSize = pBlock->Size;
        lpBuffer->BaseAddress = pBlock->pMem;
    }
    g_MemoryMutex.Unlock();
    return Size;
}

#endif // _DEBUG_ALLOC