// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFile.h
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUFILE_H
#define EMUFILE_H

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include <stdio.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "Emu.h"

// ******************************************************************
// * Maximum number of open handles in the system
// ******************************************************************
#define EMU_MAX_HANDLES 1024

// ******************************************************************
// * Wrapper of a handle object
// ******************************************************************
class EmuHandle
{
    public:
        // Type of handle
        volatile enum _EmuHandleType m_Type;

        // To keep the size 8 bytes, these 2 items are in a union
	    union
	    {
		    // Pointer to actual object (when handle is valid)
		    volatile class EmuNtObject *m_Object;

            // Pointer to next free handle
		    volatile EmuHandle *m_NextFree;
	    };

        // Close this handle
	    NtDll::NTSTATUS Close(void);

	    // Initialize the EmuHandle system
        static bool Initialize();

        // Close all open handles
	    static void CloseAll(void);

        // Allocate an empty handle
	    static volatile EmuHandle *Allocate(void);

    private:
	    // Array of EmuHandles in the system
	    static EmuHandle Handles[EMU_MAX_HANDLES];

        // Pointer to first free handle in array, or NULL if none
	    volatile static EmuHandle *FirstFree;

        // Pointer to last free handle in array, or NULL if none
	    volatile static EmuHandle *LastFree;

        // Lock on the handle system
	    static CRITICAL_SECTION HandleLock;

	    // Quick functions to lock/unlock
	    inline static void Lock(void);
	    inline static void Unlock(void);
};

// ******************************************************************
// * Various "special" handle types
// ******************************************************************
typedef enum _EmuHandleType
{
	// Unallocated handle
	EMUHANDLE_TYPE_EMPTY = 0,

    // Allocated but so far unused handle
	EMUHANDLE_TYPE_ALLOCATED,

    // File handle with no really special features
	EMUHANDLE_TYPE_FILE,

    // Fake file/directory/directory object/partition handle
	EMUHANDLE_TYPE_OBJECT
}
EmuHandleType;

// ******************************************************************
// * An NT fake object
// ******************************************************************
class EmuNtObject
{
    public:
	    // Decrements the reference count of this object (never override)
	    void NtClose(void);

	    // These functions mimic the Nt* calls

	    // Increments the reference count of this object
	    // For file handles, a whole new EmuFile structure is returned.
	    // For other objects (the default implementation), "this" is returned.
	    virtual EmuNtObject *NtDuplicateObject(void);

    protected:
	    // Object name (Unicode, because we handle after-conversion strings)
	    const WCHAR *Name;
	    ULONG NameLength;
	    // Permanent status
	    bool PermanentFlag;

	    // Called by close() when the reference count reaches zero
	    virtual void Free(void) = 0;
	    // Constructor
	    EmuNtObject(void);
	    // Destructor
	    virtual ~EmuNtObject() = 0;

    private:
	    // Reference count
	    ULONG RefCount;
};

// ******************************************************************
// * Emulated file handle
// ******************************************************************
class EmuNtFile : public EmuNtObject
{
    public:
	    // We need to override NtDuplicateObject in this case

    private:
	    // The Windows file handle
	    HANDLE File;
	    // Pointer to the volume from which this came
	    //EmuNtVolume *Volume;
};

// ******************************************************************
// * is hFile a 'special' emulated handle?
// ******************************************************************
static inline bool IsEmuHandle(xboxkrnl::HANDLE hFile)
{
    bool ret = ((uint32)hFile > 0x80000000) && ((uint32)hFile != -1);

    return ret;
}

// ******************************************************************
// * convert from 'special' emulated handle to a pointer
// ******************************************************************
static inline EmuHandle *EmuHandleToPtr(xboxkrnl::HANDLE hFile)
{
    return (EmuHandle*)((uint32)hFile - 0x80000000);
}

// ******************************************************************
// * convert from 'special' emulated handle to a pointer
// ******************************************************************
static inline HANDLE PtrToEmuHandle(EmuHandle *pEmuHandle)
{
    return (HANDLE)((uint32)pEmuHandle + 0x80000000);
}

#endif
