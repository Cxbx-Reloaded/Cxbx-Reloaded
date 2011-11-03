// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnl.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include <cstdio>
#include <cctype>
#include <clocale>
#include <process.h>

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuFile.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"
#include "ResourceTracker.h"

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)

// PsCreateSystemThread proxy parameters
typedef struct _PCSTProxyParam
{
    IN PVOID  StartContext1;
    IN PVOID  StartContext2;
    IN PVOID  StartRoutine;
    IN BOOL   StartSuspended;
    IN HANDLE hStartedEvent;
}
PCSTProxyParam;

// Global Variable(s)
extern PVOID g_pfnThreadNotification[16] = { NULL };
extern int g_iThreadNotificationCount = 0;
PVOID g_pPersistedData = NULL;

// PsCreateSystemThread proxy procedure
#pragma warning(push)
#pragma warning(disable: 4731)  // disable ebp modification warning
static unsigned int WINAPI PCSTProxy
(
    IN PVOID Parameter
)
{
    PCSTProxyParam *iPCSTProxyParam = (PCSTProxyParam*)Parameter;

    uint32 StartContext1 = (uint32)iPCSTProxyParam->StartContext1;
    uint32 StartContext2 = (uint32)iPCSTProxyParam->StartContext2;
    uint32 StartRoutine  = (uint32)iPCSTProxyParam->StartRoutine;
    BOOL   StartSuspended = (BOOL)iPCSTProxyParam->StartSuspended;

    delete iPCSTProxyParam;

    DbgPrintf("EmuKrnl (0x%X): PCSTProxy\n"
           "(\n"
           "   StartContext1       : 0x%.08X\n"
           "   StartContext2       : 0x%.08X\n"
           "   StartRoutine        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), StartContext1, StartContext2, StartRoutine);

    if(StartSuspended == TRUE)
        SuspendThread(GetCurrentThread());

    EmuGenerateFS(CxbxKrnl_TLS, CxbxKrnl_TLSData);

    // call thread notification routine(s)
    if(g_iThreadNotificationCount != 0)
    {
		for(int i = 0; i < 16; i++ )
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if(pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmKrnl (0x%X): Calling pfnNotificationRoutine[%d] (0x%.08X)\n", g_iThreadNotificationCount, GetCurrentThreadId(), pfnNotificationRoutine);

			EmuSwapFS();   // Xbox FS

			pfnNotificationRoutine(TRUE);

			EmuSwapFS();   // Win2k/XP FS
		}
    }

    // use the special calling convention
    __try
    {
        SetEvent(iPCSTProxyParam->hStartedEvent);

        EmuSwapFS();   // Xbox FS

        __asm
        {
            mov         esi, StartRoutine
            push        StartContext2
            push        StartContext1
            push        offset callComplete
            lea         ebp, [esp-4]
            jmp near    esi
        }
    }
    __except(EmuException(GetExceptionInformation()))
    {
        EmuWarning("Problem with ExceptionFilter!");
    }

callComplete:

    EmuSwapFS();    // Win2k/XP FS

    // call thread notification routine(s)
    if(g_iThreadNotificationCount != 0)
    {
		for(int i = 0; i < 16; i++ )
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if(pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmKrnl (0x%X): Calling pfnNotificationRoutine[%d] (0x%.08X)\n", g_iThreadNotificationCount, GetCurrentThreadId(), pfnNotificationRoutine);

			EmuSwapFS();   // Xbox FS

			pfnNotificationRoutine(FALSE);

			EmuSwapFS();   // Win2k/XP FS
		}
    }

    CxbxKrnlTerminateThread();

    return 0;
}
#pragma warning(pop)

using namespace xboxkrnl;

// ******************************************************************
// * 0x0001 AvGetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(1) xboxkrnl::PVOID NTAPI xboxkrnl::AvGetSavedDataAddress()
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuKrnl (0x%X): AvGetSavedDataAddress();\n", GetCurrentThreadId() );

//	__asm int 3;

	// Allocate a buffer the size of the screen buffer and return that.
	// TODO: Fill this buffer with the contents of the front buffer.
	// TODO: This isn't always the size we need...

	if( g_pPersistedData )
	{
		CxbxFree( g_pPersistedData );
		g_pPersistedData = NULL;
	}

	g_pPersistedData = CxbxMalloc( 640*480*4 );

#if 0
	// Get a copy of the front buffer
	IDirect3DSurface8* pFrontBuffer = NULL;

	if( SUCCEEDED(g_pD3DDevice8->GetFrontBuffer(pFrontBuffer)))
	{
		D3DLOCKED_RECT LockedRect;
		pFrontBuffer->LockRect( 0, NULL, &LockedRect );

		CopyMemory( g_pPersistedData, LockRect.pBits, 640*480*4 );

		pFrontBuffer->UnlockRect();
	}
#endif

	// TODO: We might want to return something sometime...
	/*if( !g_pPersistedData )
	{
		FILE* fp = fopen( "PersistedSurface.bin", "rb" );
		fseek( fp, 0, SEEK_END );
		long size = ftell( fp );
		g_pPersistedData = malloc( size );
		fread( g_pPersistedData, size, 1, fp );
		fclose(fp);
	}*/

	EmuSwapFS();	// Xbox FS

	return NULL;//g_pPersistedData;
}

// ******************************************************************
// * 0x0008 DbgPrint
// ******************************************************************
XBSYSAPI EXPORTNUM(8) xboxkrnl::ULONG _cdecl xboxkrnl::DbgPrint
(
	PCHAR  Format, ...
)
{
	EmuSwapFS();   // Win2k/XP FS

	DbgPrintf( "EmuKrnl (0x%X): DbgPrint\n"
		"(\n"
		"   Format              : 0x%.08X\n"
		"   ...\n"
		");\n",
		GetCurrentThreadId(), Format);

	if(Format != NULL) 
	{
		char szBuffer[MAX_PATH];

		va_list argp;
		va_start(argp, Format);

		vsprintf(szBuffer, Format, argp);
		va_end(argp);

		//LogPrintf("[EmuKrnl] DbgPrint: %s", szBuffer);

		EmuWarning(szBuffer);
		//DbgPrintf(szBuffer);
	}

	EmuSwapFS();   // Xbox FS

	return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0009 HalReadSMCTrayState
// ******************************************************************
XBSYSAPI EXPORTNUM(9) VOID NTAPI xboxkrnl::HalReadSMCTrayState
(
	DWORD*	State,
	DWORD*	Count
)
{
	EmuSwapFS();	// Win2k/XP FS
	
	DbgPrintf("EmuKrnl (0x%X): HalReadSMCTrayState\n"
           "(\n"
           "   State              : 0x%.08X\n"
		   "   Count              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), State, Count);

#define TRAY_CLOSED_MEDIA_PRESENT 96
#define TRAY_CLOSED_NO_MEDIA 64
#define TRAY_OPEN 16

	// TODO: Make this configurable?
	// TODO: What is the count parameter for??

	*State = TRAY_CLOSED_NO_MEDIA;
//	*Count = 1;

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * 0x000E ExAllocatePool
// ******************************************************************
XBSYSAPI EXPORTNUM(14) xboxkrnl::PVOID NTAPI xboxkrnl::ExAllocatePool
(
    IN ULONG NumberOfBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): ExAllocatePool\n"
           "(\n"
           "   NumberOfBytes       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NumberOfBytes);

    PVOID pRet = CxbxMalloc(NumberOfBytes);

    EmuSwapFS();   // Xbox FS

    return pRet;
}

// ******************************************************************
// * 0x000F ExAllocatePoolWithTag
// ******************************************************************
// * Differences from NT: There is no PoolType field, as the XBOX
// * only has 1 pool, the non-paged pool.
// ******************************************************************
XBSYSAPI EXPORTNUM(15) xboxkrnl::PVOID NTAPI xboxkrnl::ExAllocatePoolWithTag
(
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): ExAllocatePoolWithTag\n"
           "(\n"
           "   NumberOfBytes       : 0x%.08X\n"
           "   Tag                 : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NumberOfBytes, Tag);

    // TODO: Actually implement this
    PVOID pRet = CxbxMalloc(NumberOfBytes);

    EmuSwapFS();   // Xbox FS

    return pRet;
}

// ******************************************************************
// * 0x0018 ExQueryNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(24) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExQueryNonVolatileSetting
(
    IN  DWORD               ValueIndex,
    OUT DWORD              *Type,
    OUT PUCHAR              Value,
    IN  SIZE_T              ValueLength,
    OUT PSIZE_T             ResultLength OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): ExQueryNonVolatileSetting\n"
           "(\n"
           "   ValueIndex          : 0x%.08X\n"
           "   Type                : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           "   ValueLength         : 0x%.08X\n"
           "   ResultLength        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ValueIndex, Type, Value, ValueLength, ResultLength);

    // handle eeprom read
    switch(ValueIndex)
    {
        // Factory Game Region
        case 0x104:
        {
            // TODO: configurable region or autodetect of some sort
            if(Type != 0)
                *Type = 0x04;

            if(Value != 0)
                *Value = 0x01;  // North America

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }
        break;

        // Factory AC Region
        case 0x103:
        {
            // TODO: configurable region or autodetect of some sort
            if(Type != 0)
                *Type = 0x04;

            if(Value != 0)
                *Value = 0x01; // NTSC_M

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }
        break;

        // Language
        case 0x007:
        {
            // TODO: configurable language or autodetect of some sort
            if(Type != 0)
                *Type = 0x04;

            if(Value != 0)
                *Value = 0x01;  // English

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }
        break;

        // Video Flags
        case 0x008:
        {
            // TODO: configurable video flags or autodetect of some sort
            if(Type != 0)
                *Type = 0x04;

            if(Value != 0)
                *Value = 0x10;  // Letterbox

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }
        break;

        // Audio Flags
        case 0x009:
        {
            if(Type != 0)
                *Type = 0x04;

            if(Value != 0)
                *Value = 0;

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }

        case EEPROM_MISC:
        {
            if(Type != 0)
                *Type  = 0x04;

            if(Value != 0)
                *Value = 0;

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }
        break;

        /* Timezone info
        case 0x0FF:
        {
            _asm int 3;
        }
        break;
        //*/

        default:
            EmuWarning("ExQueryNonVolatileSetting unknown ValueIndex (%d)", ValueIndex);
            break;
    }

    EmuSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0019 - ExReadWriteRefurbInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(25) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExReadWriteRefurbInfo
(
	PVOID	Unknown1,
	DWORD	Unknown2,
	DWORD	Unknown3
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): ExReadWriteRefurbInfo\n"
           "(\n"
           "   Unknown1            : 0x%.08X\n"
           "   Unknown2            : 0x%.08X\n"
           "   Unknown3            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Unknown1, Unknown2, Unknown3);

	// TODO: What does this do?
	EmuWarning( "ExReadWriteRefurbInfo ignored!" );

	EmuSwapFS();	// Xbox FS

	return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x001D - ExSaveNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(29) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExSaveNonVolatileSetting
(
    IN  DWORD               ValueIndex,
    OUT DWORD              *Type,
    IN  PUCHAR              Value,
    IN  SIZE_T              ValueLength
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): ExQueryNonVolatileSetting\n"
           "(\n"
           "   ValueIndex          : 0x%.08X\n"
           "   Type                : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           "   ValueLength         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ValueIndex, Type, Value, ValueLength);

	// TODO: Later.

	EmuSwapFS();	// Xbox FS

	return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0025 - FscSetCacheSize
// ******************************************************************
XBSYSAPI EXPORTNUM(37) xboxkrnl::LONG NTAPI xboxkrnl::FscSetCacheSize(ULONG uCachePages)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): FscSetCacheSize\n"
           "(\n"
           "   uCachePages         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), uCachePages);

    EmuWarning("FscSetCacheSize is being ignored");

    EmuSwapFS();   // Xbox FS

    return 0;
}

// ******************************************************************
// * 0x0031 - HalReturnToFirmware
// ******************************************************************
XBSYSAPI EXPORTNUM(49) VOID DECLSPEC_NORETURN xboxkrnl::HalReturnToFirmware
(
    RETURN_FIRMWARE Routine
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): HalReturnToFirmware\n"
           "(\n"
           "   Routine             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Routine);

	if( ReturnFirmwareFatal )
		EmuWarning("Stupid hack, lol" );
	else
		CxbxKrnlCleanup("Xbe has rebooted : HalReturnToFirmware(%d)", Routine);

	EmuSwapFS(); // Xbox FS
}

// ******************************************************************
// * 0x0042 - IoCreateFile
// ******************************************************************
XBSYSAPI EXPORTNUM(66) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoCreateFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               Disposition,
    IN  ULONG               CreateOptions,
    IN  ULONG               Options
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): IoCreateFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   DesiredAccess       : 0x%.08X\n"
           "   ObjectAttributes    : 0x%.08X (%s)\n"
           "   IoStatusBlock       : 0x%.08X\n"
           "   AllocationSize      : 0x%.08X\n"
           "   FileAttributes      : 0x%.08X\n"
           "   ShareAccess         : 0x%.08X\n"
           "   Disposition         : 0x%.08X\n"
           "   CreateOptions       : 0x%.08X\n"
           "   Options             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, DesiredAccess, ObjectAttributes, ObjectAttributes->ObjectName->Buffer,
           IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, Disposition, CreateOptions, Options);

    NTSTATUS ret = STATUS_SUCCESS;

    // TODO: Try redirecting to NtCreateFile if this function ever is run into
    CxbxKrnlCleanup("IoCreateFile not implemented");

    EmuSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0043 IoCreateSymbolicLink
// ******************************************************************
XBSYSAPI EXPORTNUM(67) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoCreateSymbolicLink
(
    IN PSTRING SymbolicLinkName,
    IN PSTRING DeviceName
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): IoCreateSymbolicLink\n"
           "(\n"
           "   SymbolicLinkName    : 0x%.08X (%s)\n"
           "   DeviceName          : 0x%.08X (%s)\n"
           ");\n",
           GetCurrentThreadId(), SymbolicLinkName, SymbolicLinkName->Buffer,
           DeviceName, DeviceName->Buffer);

    // TODO: Actually um...implement this function
    NTSTATUS ret = STATUS_OBJECT_NAME_COLLISION;

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x0045 - IoDeleteSymbolicLink
// ******************************************************************
XBSYSAPI EXPORTNUM(69) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoDeleteSymbolicLink
(
    IN PSTRING SymbolicLinkName
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): IoDeleteSymbolicLink\n"
           "(\n"
           "   SymbolicLinkName    : 0x%.08X (%s)\n"
           ");\n",
           GetCurrentThreadId(), SymbolicLinkName, SymbolicLinkName->Buffer);

    // TODO: Actually um...implement this function
    NTSTATUS ret = STATUS_OBJECT_NAME_NOT_FOUND;

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * IoDismountVolumeByName
// ******************************************************************
XBSYSAPI EXPORTNUM(91) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoDismountVolumeByName
(
	IN PSTRING VolumeName
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuKrnl (0x%X): IoDismountVolumeByName\n"
			"(\n"
			"   VolumeName        : 0x%.08X (%s)\n"
			");\n",
			GetCurrentThreadId(), VolumeName, VolumeName);

	// TODO: Anything?
	NTSTATUS ret = STATUS_SUCCESS;

	EmuSwapFS();	// Xbox FS

	return ret;
}

// ******************************************************************
// * KeBugCheck
// ******************************************************************
XBSYSAPI EXPORTNUM(95) VOID NTAPI xboxkrnl::KeBugCheck
(
	IN ULONG BugCheckMode
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuKrnl (0x%X): KeBugCheck\n"
			"(\n"
			"   BugCheckMode      : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), BugCheckMode);

	// TODO: Investigate XapiFiberStartup maybe?

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * 0x0063 - KeDelayExecutionThread
// ******************************************************************
XBSYSAPI EXPORTNUM(99) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeDelayExecutionThread
(
    IN KPROCESSOR_MODE  WaitMode,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeDelayExecutionThread\n"
           "(\n"
           "   WaitMode            : 0x%.08X\n"
           "   Alertable           : 0x%.08X\n"
           "   Interval            : 0x%.08X (%I64d)\n"
           ");\n",
           GetCurrentThreadId(), WaitMode, Alertable, Interval, Interval == 0 ? 0 : Interval->QuadPart);

    NTSTATUS ret = NtDll::NtDelayExecution(Alertable, (NtDll::LARGE_INTEGER*)Interval);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x006B - KeInitializeDpc
// ******************************************************************
XBSYSAPI EXPORTNUM(107) VOID NTAPI xboxkrnl::KeInitializeDpc
(
    KDPC                *Dpc,
    PKDEFERRED_ROUTINE   DeferredRoutine,
    PVOID                DeferredContext
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeInitializeDpc\n"
           "(\n"
           "   Dpc                 : 0x%.08X\n"
           "   DeferredRoutine     : 0x%.08X\n"
           "   DeferredContext     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Dpc, DeferredRoutine, DeferredContext);

    // inialize Dpc field values
    Dpc->Number = 0;
    Dpc->DeferredRoutine = DeferredRoutine;
    Dpc->Type = DpcObject;
    Dpc->DeferredContext = DeferredContext;

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0071 - KeInitializeTimerEx
// ******************************************************************
XBSYSAPI EXPORTNUM(113) VOID NTAPI xboxkrnl::KeInitializeTimerEx
(
    IN PKTIMER      Timer,
    IN TIMER_TYPE   Type
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeInitializeTimerEx\n"
           "(\n"
           "   Timer               : 0x%.08X\n"
           "   Type                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Timer, Type);

    Timer->Header.Type               = Type + 8;
    Timer->Header.Inserted           = 0;
    Timer->Header.Size               = sizeof(*Timer) / sizeof(ULONG);
    Timer->Header.SignalState        = 0;
    Timer->TimerListEntry.Blink      = NULL;
    Timer->TimerListEntry.Flink      = NULL;
    Timer->Header.WaitListHead.Flink = &Timer->Header.WaitListHead;
    Timer->Header.WaitListHead.Blink = &Timer->Header.WaitListHead;
    Timer->DueTime.QuadPart          = 0;
    Timer->Period                    = 0;

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * KeQueryPerformanceCounter
// ******************************************************************
XBSYSAPI EXPORTNUM(126) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceCounter()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeQueryPerformanceCounter();\n", GetCurrentThreadId());

    ::LARGE_INTEGER Counter;

    QueryPerformanceCounter(&Counter);

    EmuSwapFS();   // Xbox FS

    return Counter.QuadPart;
}

// ******************************************************************
// * KeQueryPerformanceFrequency
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceFrequency()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeQueryPerformanceFrequency();\n", GetCurrentThreadId());

    // Xbox Performance Counter Frequency := 337F98h
    ::LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);

    EmuSwapFS();   // Xbox FS

    return Frequency.QuadPart;
}

// ******************************************************************
// * 0x0080 - KeQuerySystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(128) VOID NTAPI xboxkrnl::KeQuerySystemTime
(
    PLARGE_INTEGER CurrentTime
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeQuerySystemTime\n"
           "(\n"
           "   CurrentTime         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), CurrentTime);

    // TODO: optimize for WinXP if speed ever becomes important here

    SYSTEMTIME SystemTime;

    GetSystemTime(&SystemTime);

    SystemTimeToFileTime(&SystemTime, (FILETIME*)CurrentTime);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * KeRaiseIrqlToDpcLevel
// ******************************************************************
XBSYSAPI EXPORTNUM(129) xboxkrnl::UCHAR NTAPI xboxkrnl::KeRaiseIrqlToDpcLevel()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeRaiseIrqlToDpcLevel()\n", GetCurrentThreadId());

	// I really tried to avoid adding this...
//	__asm int 3;
//	CxbxKrnlCleanup("KeRaiseIrqlToDpcLevel not implemented! (Tell blueshogun -_-)");

	EmuSwapFS();

	return 0;
}

// ******************************************************************
// * 0x0095 - KeSetTimer
// ******************************************************************
XBSYSAPI EXPORTNUM(149) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimer
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN PKDPC          Dpc OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeSetTimer\n"
           "(\n"
           "   Timer               : 0x%.08X\n"
           "   DueTime             : 0x%I64X\n"
           "   Dpc                 : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Timer, DueTime, Dpc);

    // Call KeSetTimerEx
//    KeSetTimerEx(Timer, DueTime, 0, Dpc);
	__asm int 3;
	CxbxKrnlCleanup("KeSetTimer is not implemented!");

    EmuSwapFS();   // Xbox FS

    return TRUE;
}

// ******************************************************************
// * 0x0096 - KeSetTimerEx
// ******************************************************************
XBSYSAPI EXPORTNUM(150) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimerEx
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN LONG           Period OPTIONAL,
    IN PKDPC          Dpc OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KeSetTimerEx\n"
           "(\n"
           "   Timer               : 0x%.08X\n"
           "   DueTime             : 0x%I64X\n"
           "   Period              : 0x%.08X\n"
           "   Dpc                 : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Timer, DueTime, Period, Dpc);

    CxbxKrnlCleanup("KeSetTimerEx is not implemented");

    EmuSwapFS();   // Xbox FS

    return TRUE;
}

// ******************************************************************
// * 0x009C - KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) volatile xboxkrnl::DWORD xboxkrnl::KeTickCount = 0;

// ******************************************************************
// * xLaunchDataPage (pointed to by LaunchDataPage)
// ******************************************************************
LAUNCH_DATA_PAGE xLaunchDataPage =
{
    {   // header
        2,  // 2: dashboard, 0: title
        0,
        "D:\\default.xbe",
        0
    }
};

// ******************************************************************
// * 0x00A0 - KfRaiseIrql
// ******************************************************************
XBSYSAPI EXPORTNUM(160) xboxkrnl::UCHAR NTAPI xboxkrnl::KfRaiseIrql
(
    IN UCHAR NewIrql
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KfRaiseIrql\n"
           "(\n"
           "   NewIrql            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NewIrql);

    EmuSwapFS();   // Xbox FS

    return 0;
}

// ******************************************************************
// * 0x00A1 - KfLowerIrql
// ******************************************************************
XBSYSAPI EXPORTNUM(161) xboxkrnl::UCHAR NTAPI xboxkrnl::KfLowerIrql
(
    IN UCHAR NewIrql
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): KfLowerIrql\n"
           "(\n"
           "   NewIrql            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NewIrql);

    EmuSwapFS();   // Xbox FS

    return 0;
}

// ******************************************************************
// * 0x00A4 - LaunchDataPage (actually a pointer)
// ******************************************************************
XBSYSAPI EXPORTNUM(164) xboxkrnl::PLAUNCH_DATA_PAGE xboxkrnl::LaunchDataPage = &xLaunchDataPage;

// ******************************************************************
// * 0x00A5 - MmAllocateContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(165) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateContiguousMemory
(
    IN ULONG NumberOfBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmAllocateContiguousMemory\n"
           "(\n"
           "   NumberOfBytes            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NumberOfBytes);

    //
    // NOTE: Kludgey (but necessary) solution:
    //
    // Since this memory must be aligned on a page boundary, we must allocate an extra page
    // so that we can return a valid page aligned pointer
    //

    PVOID pRet = CxbxMalloc(NumberOfBytes + 0x1000);

    // align to page boundary
    {
        DWORD dwRet = (DWORD)pRet;

        dwRet += 0x1000 - dwRet%0x1000;

        g_AlignCache.insert(dwRet, pRet);

        pRet = (PVOID)dwRet;
    }

    DbgPrintf("EmuKrnl (0x%X): MmAllocateContiguous returned 0x%.08X\n", GetCurrentThreadId(), pRet);

    EmuSwapFS();   // Xbox FS

    return pRet;
}

// ******************************************************************
// * 0x00A6 - MmAllocateContiguousMemoryEx
// ******************************************************************
XBSYSAPI EXPORTNUM(166) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateContiguousMemoryEx
(
    IN ULONG            NumberOfBytes,
    IN PHYSICAL_ADDRESS LowestAcceptableAddress,
    IN PHYSICAL_ADDRESS HighestAcceptableAddress,
    IN ULONG            Alignment OPTIONAL,
    IN ULONG            ProtectionType
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmAllocateContiguousMemoryEx\n"
           "(\n"
           "   NumberOfBytes            : 0x%.08X\n"
           "   LowestAcceptableAddress  : 0x%.08X\n"
           "   HighestAcceptableAddress : 0x%.08X\n"
           "   Alignment                : 0x%.08X\n"
           "   ProtectionType           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NumberOfBytes, LowestAcceptableAddress, HighestAcceptableAddress,
           Alignment, ProtectionType);

    //
    // NOTE: Kludgey (but necessary) solution:
    //
    // Since this memory must be aligned on a page boundary, we must allocate an extra page
    // so that we can return a valid page aligned pointer
    //

    PVOID pRet = CxbxMalloc(NumberOfBytes + 0x1000);

    // align to page boundary
    {
        DWORD dwRet = (DWORD)pRet;

        dwRet += 0x1000 - dwRet%0x1000;

        g_AlignCache.insert(dwRet, pRet);

        pRet = (PVOID)dwRet;
    }

    static int count = 0;

    if(count++ < 4)
        g_HaloHack[count-1] = (uint32)pRet;

    DbgPrintf("EmuKrnl (0x%X): MmAllocateContiguousEx returned 0x%.08X\n", GetCurrentThreadId(), pRet);

    EmuSwapFS();   // Xbox FS

    return pRet;
}

// ******************************************************************
// * 0x00A7 - MmAllocateSystemMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(167) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateSystemMemory
(
    ULONG NumberOfBytes,
    ULONG Protect
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmAllocateContiguousMemoryEx\n"
           "(\n"
           "   NumberOfBytes            : 0x%.08X\n"
           "   Protect                  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NumberOfBytes, Protect);

    // TODO: should this be aligned?
    PVOID pRet = CxbxMalloc(NumberOfBytes);

    EmuSwapFS();   // Xbox FS

    return pRet;
}

// ******************************************************************
// * 0x00A9 - MmCreateKernelStack
// ******************************************************************
// * Differences from NT: Custom stack size.
// ******************************************************************
XBSYSAPI EXPORTNUM(169) xboxkrnl::PVOID NTAPI xboxkrnl::MmCreateKernelStack
(
    ULONG NumberOfBytes,
    ULONG Unknown
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmCreateKernelStack\n"
           "(\n"
           "   NumberOfBytes            : 0x%.08X\n"
           "   Unknown                  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), NumberOfBytes, Unknown);

    if(Unknown)
        EmuWarning("MmCreateKernelStack unknown parameter ignored\n");

    NtDll::PVOID pRet = NULL;
    if(FAILED(NtDll::NtAllocateVirtualMemory(GetCurrentProcess(), &pRet, 0, &NumberOfBytes, MEM_COMMIT, PAGE_READWRITE)))
        EmuWarning("MmCreateKernelStack failed!\n");
    else
        pRet = (PVOID)((ULONG)pRet + NumberOfBytes);

    EmuSwapFS();   // Xbox FS

    return pRet;
}

// ******************************************************************
// * 0x00AA - MmDeleteKernelStack
// ******************************************************************
XBSYSAPI EXPORTNUM(170) VOID NTAPI xboxkrnl::MmDeleteKernelStack
(
    PVOID EndAddress,
    PVOID BaseAddress
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmDeleteKernelStack\n"
           "(\n"
           "   EndAddress               : 0x%.08X\n"
           "   BaseAddress              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), EndAddress, BaseAddress);

    ULONG RegionSize = 0;
    if (FAILED(NtDll::NtFreeVirtualMemory(GetCurrentProcess(), &BaseAddress, &RegionSize, MEM_RELEASE)))
        EmuWarning("MmDeleteKernelStack failed!\n");

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * 0x00AB - MmFreeContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(171) VOID NTAPI xboxkrnl::MmFreeContiguousMemory
(
    IN PVOID BaseAddress
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmFreeContiguousMemory\n"
           "(\n"
           "   BaseAddress              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress);

    PVOID OrigBaseAddress = BaseAddress;

    if(g_AlignCache.exists(BaseAddress))
    {
        OrigBaseAddress = g_AlignCache.get(BaseAddress);

        g_AlignCache.remove(BaseAddress);
    }

    if(OrigBaseAddress != &xLaunchDataPage)
    {
        CxbxFree(OrigBaseAddress);
    }
    else
    {
        DbgPrintf("Ignored MmFreeContiguousMemory(&xLaunchDataPage)\n");
    }

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x00AC - MmFreeSystemMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(172) xboxkrnl::NTSTATUS NTAPI xboxkrnl::MmFreeSystemMemory
(
    PVOID BaseAddress,
    ULONG NumberOfBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmFreeSystemMemory\n"
           "(\n"
           "   BaseAddress              : 0x%.08X\n"
           "   NumberOfBytes            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress, NumberOfBytes);

    CxbxFree(BaseAddress);

    EmuSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00B2 - MmPersistContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(178) VOID NTAPI xboxkrnl::MmPersistContiguousMemory
(
    IN PVOID   BaseAddress,
    IN ULONG   NumberOfBytes,
    IN BOOLEAN Persist
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmPersistContiguousMemory\n"
           "(\n"
           "   BaseAddress              : 0x%.08X\n"
           "   NumberOfBytes            : 0x%.08X\n"
           "   Persist                  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress, NumberOfBytes, Persist);

    // TODO: Actually set this up to be remember across a "reboot"
    EmuWarning("MmPersistContiguousMemory is being ignored\n");

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * 0x00B4 - MmQueryAllocationSize
// ******************************************************************
XBSYSAPI EXPORTNUM(180) XTL::ULONG NTAPI xboxkrnl::MmQueryAllocationSize
(
    IN PVOID   BaseAddress
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmQueryAllocationSize\n"
           "(\n"
           "   BaseAddress              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress);

    ULONG uiSize = EmuCheckAllocationSize(BaseAddress, false);

    EmuSwapFS();   // Xbox FS

    return uiSize;
}

// ******************************************************************
// * 0x00B5 - MmQueryStatistics
// ******************************************************************
XBSYSAPI EXPORTNUM(181) xboxkrnl::NTSTATUS NTAPI xboxkrnl::MmQueryStatistics
(
    OUT PMM_STATISTICS MemoryStatistics
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmQueryStatistics\n"
           "(\n"
           "   MemoryStatistics         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), MemoryStatistics);

    MEMORYSTATUS MemoryStatus;

    GlobalMemoryStatus(&MemoryStatus);

    ZeroMemory(MemoryStatistics, sizeof(MM_STATISTICS));

    MemoryStatistics->Length = sizeof(MM_STATISTICS);
    MemoryStatistics->TotalPhysicalPages = MemoryStatus.dwTotalVirtual / 4096;
    MemoryStatistics->AvailablePages = MemoryStatus.dwAvailVirtual / 4096;

    // HACK (does this matter?)
    MemoryStatistics->VirtualMemoryBytesReserved = MemoryStatus.dwTotalPhys - MemoryStatus.dwAvailPhys;

    // the rest arent really used from what i've seen

    EmuSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00B6 - MmSetAddressProtect
// ******************************************************************
XBSYSAPI EXPORTNUM(182) VOID NTAPI xboxkrnl::MmSetAddressProtect
(
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes,
    IN ULONG NewProtect
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): MmSetAddressProtect\n"
           "(\n"
           "   BaseAddress              : 0x%.08X\n"
           "   NumberOfBytes            : 0x%.08X\n"
           "   NewProtect               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress, NumberOfBytes, NewProtect);

    // Halo Hack
    if(BaseAddress == (PVOID)0x80366000)
    {
        BaseAddress = (PVOID)(g_HaloHack[0] + (0x80366000 - 0x80061000));

        DbgPrintf("EmuKrnl (0x%X): Halo Access Adjust 3 was applied! (0x%.08X)\n", GetCurrentThreadId(), BaseAddress);
    }

    DWORD dwOldProtect;

    if(!VirtualProtect(BaseAddress, NumberOfBytes, NewProtect & (~PAGE_WRITECOMBINE), &dwOldProtect))
        EmuWarning("VirtualProtect Failed!");

    DbgPrintf("EmuKrnl (0x%X): VirtualProtect was 0x%.08X -> 0x%.08X\n", GetCurrentThreadId(), dwOldProtect, NewProtect & (~PAGE_WRITECOMBINE));

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x00B8 - NtAllocateVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(184) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtAllocateVirtualMemory
(
    IN OUT PVOID    *BaseAddress,
    IN ULONG         ZeroBits,
    IN OUT PULONG    AllocationSize,
    IN DWORD         AllocationType,
    IN DWORD         Protect
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtAllocateVirtualMemory\n"
           "(\n"
           "   BaseAddress         : 0x%.08X (0x%.08X)\n"
           "   ZeroBits            : 0x%.08X\n"
           "   AllocationSize      : 0x%.08X (0x%.08X)\n"
           "   AllocationType      : 0x%.08X\n"
           "   Protect             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress, *BaseAddress, ZeroBits, AllocationSize, *AllocationSize, AllocationType, Protect);
	
	// TODO: The flag known as MEM_NOZERO (which appears to be exclusive to Xbox) has the exact
	// same value as MEM_ROTATE which causes problems for Windows XP, but not Vista.  Removing
	// this flag fixes Azurik for XP.
	DWORD MEM_NOZERO = 0x800000;

	if( AllocationType & MEM_NOZERO )
	{
		EmuWarning( "MEM_NOZERO flag is not supported!" );
		AllocationType &= ~MEM_NOZERO;
	}

    NTSTATUS ret = NtDll::NtAllocateVirtualMemory(GetCurrentProcess(), BaseAddress, ZeroBits, AllocationSize, AllocationType, Protect);
	if( ret == 0xC00000F3 )
		EmuWarning( "Invalid Param!" );

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00BA - NtClearEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(186) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtClearEvent
(
    IN HANDLE EventHandle
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtClearEvent\n"
           "(\n"
           "   EventHandle         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), EventHandle);

    NTSTATUS ret = NtDll::NtClearEvent(EventHandle);

    if(FAILED(ret))
        EmuWarning("NtClearEvent Failed!");

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00BB - NtClose
// ******************************************************************
XBSYSAPI EXPORTNUM(187) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtClose
(
    IN HANDLE Handle
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtClose\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Handle);

    NTSTATUS ret = STATUS_SUCCESS;

    // delete 'special' handles
    if(IsEmuHandle(Handle))
    {
        EmuHandle *iEmuHandle = EmuHandleToPtr(Handle);

        delete iEmuHandle;

        ret = STATUS_SUCCESS;
    }
    // close normal handles
    else
    {
        ret = NtDll::NtClose(Handle);
    }

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00BD - NtCreateEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(189) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateEvent
(
    OUT PHANDLE             EventHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  EVENT_TYPE          EventType,
    IN  BOOLEAN             InitialState
)
{
    EmuSwapFS();   // Win2k/XP FS

    char *szBuffer = (ObjectAttributes != 0) ? ObjectAttributes->ObjectName->Buffer : 0;

    DbgPrintf("EmuKrnl (0x%X): NtCreateEvent\n"
           "(\n"
           "   EventHandle         : 0x%.08X\n"
           "   ObjectAttributes    : 0x%.08X (\"%s\")\n"
           "   EventType           : 0x%.08X\n"
           "   InitialState        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), EventHandle, ObjectAttributes, szBuffer,
           EventType, InitialState);

    wchar_t wszObjectName[160];

    NtDll::UNICODE_STRING    NtUnicodeString;
    NtDll::OBJECT_ATTRIBUTES NtObjAttr;

    // initialize object attributes
    if(szBuffer != 0)
    {
        mbstowcs(wszObjectName, "\\??\\", 4);
        mbstowcs(wszObjectName+4, szBuffer, 160);

        NtDll::RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

        InitializeObjectAttributes(&NtObjAttr, &NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);
    }

    NtObjAttr.RootDirectory = 0;

    // redirect to NtCreateEvent
    NTSTATUS ret = NtDll::NtCreateEvent(EventHandle, EVENT_ALL_ACCESS, (szBuffer != 0) ? &NtObjAttr : 0, (NtDll::EVENT_TYPE)EventType, InitialState);

    if(FAILED(ret))
        EmuWarning("NtCreateEvent Failed!");

    DbgPrintf("EmuKrnl (0x%X): NtCreateEvent EventHandle = 0x%.08X\n", GetCurrentThreadId(), *EventHandle);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00BE - NtCreateFile
// ******************************************************************
XBSYSAPI EXPORTNUM(190) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               CreateDisposition,
    IN  ULONG               CreateOptions
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtCreateFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   DesiredAccess       : 0x%.08X\n"
           "   ObjectAttributes    : 0x%.08X (\"%s\")\n"
           "   IoStatusBlock       : 0x%.08X\n"
           "   AllocationSize      : 0x%.08X\n"
           "   FileAttributes      : 0x%.08X\n"
           "   ShareAccess         : 0x%.08X\n"
           "   CreateDisposition   : 0x%.08X\n"
           "   CreateOptions       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, DesiredAccess, ObjectAttributes, ObjectAttributes->ObjectName->Buffer,
           IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions);

    char ReplaceChar  = '\0';
    int  ReplaceIndex = -1;

    char *szBuffer = ObjectAttributes->ObjectName->Buffer;

	if(szBuffer == (char*) 0xFFFFFFFF)
		szBuffer = NULL;

    if(szBuffer != NULL)
    {
        //printf("Orig : %s\n", szBuffer);

        // trim this off
        if(szBuffer[0] == '\\' && szBuffer[1] == '?' && szBuffer[2] == '?' && szBuffer[3] == '\\')
        {
            szBuffer += 4;
        }

        // D:\ should map to current directory
        if( (szBuffer[0] == 'D' || szBuffer[0] == 'd') && szBuffer[1] == ':' && szBuffer[2] == '\\')
        {
            szBuffer += 3;

            ObjectAttributes->RootDirectory = g_hCurDir;

            DbgPrintf("EmuKrnl (0x%X): NtCreateFile Corrected path...\n", GetCurrentThreadId());
            DbgPrintf("  Org:\"%s\"\n", ObjectAttributes->ObjectName->Buffer);
            DbgPrintf("  New:\"$XbePath\\%s\"\n", szBuffer);
        }
		// Going to map Y:\ to current directory as well (dashboard test, 3944)
        else if( (szBuffer[0] == 'Y' || szBuffer[0] == 'y') && szBuffer[1] == ':' && szBuffer[2] == '\\')
        {
            szBuffer += 3;

            ObjectAttributes->RootDirectory = g_hCurDir;

            DbgPrintf("EmuKrnl (0x%X): NtCreateFile Corrected path...\n", GetCurrentThreadId());
            DbgPrintf("  Org:\"%s\"\n", ObjectAttributes->ObjectName->Buffer);
            DbgPrintf("  New:\"$XbePath\\%s\"\n", szBuffer);
        }
        else if( (szBuffer[0] == 'T' || szBuffer[0] == 't') && szBuffer[1] == ':' && szBuffer[2] == '\\')
        {
            szBuffer += 3;

            ObjectAttributes->RootDirectory = g_hTDrive;

            DbgPrintf("EmuKrnl (0x%X): NtCreateFile Corrected path...\n", GetCurrentThreadId());
            DbgPrintf("  Org:\"%s\"\n", ObjectAttributes->ObjectName->Buffer);
            DbgPrintf("  New:\"$CxbxPath\\EmuDisk\\T\\%s\"\n", szBuffer);
        }
        else if( (szBuffer[0] == 'U' || szBuffer[0] == 'u') && szBuffer[1] == ':' && szBuffer[2] == '\\')
        {
            szBuffer += 3;

            ObjectAttributes->RootDirectory = g_hUDrive;

            DbgPrintf("EmuKrnl (0x%X): NtCreateFile Corrected path...\n", GetCurrentThreadId());
            DbgPrintf("  Org:\"%s\"\n", ObjectAttributes->ObjectName->Buffer);
            DbgPrintf("  New:\"$CxbxPath\\EmuDisk\\U\\%s\"\n", szBuffer);
        }
        else if( (szBuffer[0] == 'Z' || szBuffer[0] == 'z') && szBuffer[1] == ':' && szBuffer[2] == '\\')
        {
            szBuffer += 3;

            ObjectAttributes->RootDirectory = g_hZDrive;

            DbgPrintf("EmuKrnl (0x%X): NtCreateFile Corrected path...\n", GetCurrentThreadId());
            DbgPrintf("  Org:\"%s\"\n", ObjectAttributes->ObjectName->Buffer);
            DbgPrintf("  New:\"$CxbxPath\\EmuDisk\\Z\\%s\"\n", szBuffer);
        }

        // Ignore wildcards. Xapi FindFirstFile uses the same path buffer for
        // NtOpenFile and NtQueryDirectoryFile. Wildcards are only parsed by
        // the latter.
        {
            for(int v=0;szBuffer[v] != '\0';v++)
            {
                // FIXME: Fallback to parent directory if wildcard is found.
                if(szBuffer[v] == '*')
                {
                    ReplaceIndex = v;
                    break;
                }
            }
        }

        // Note: Hack: Not thread safe (if problems occur, create a temp buffer)
        if(ReplaceIndex != -1)
		{
            ReplaceChar = szBuffer[ReplaceIndex];
            szBuffer[ReplaceIndex] = '\0';
        }

        //printf("Aftr : %s\n", szBuffer);
    }

    wchar_t wszObjectName[160];

    NtDll::UNICODE_STRING    NtUnicodeString;
    NtDll::OBJECT_ATTRIBUTES NtObjAttr;

    // initialize object attributes
    if(szBuffer != NULL)
    {
        mbstowcs(wszObjectName, szBuffer, 160);
    }
    else
    {
        wszObjectName[0] = L'\0';
    }

    NtDll::RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

    InitializeObjectAttributes(&NtObjAttr, &NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);

    // redirect to NtCreateFile
    NTSTATUS ret = NtDll::NtCreateFile
    (
        FileHandle, DesiredAccess, &NtObjAttr, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock,
        (NtDll::LARGE_INTEGER*)AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, NULL, NULL
    );

    // If we're trying to open a regular file as a directory, fallback to
    // parent directory. This behavior is required by Xapi FindFirstFile.
    if(ret == STATUS_NOT_A_DIRECTORY)
    {
        DbgPrintf("EmuKrnl (0x%X): NtCreateFile fallback to parent directory\n", GetCurrentThreadId());

        // Restore original buffer.
        if(ReplaceIndex != -1)
        {
            szBuffer[ReplaceIndex] = ReplaceChar;
        }

        // Strip filename from path.
        int CurIndex = strlen(szBuffer);
        while(CurIndex--)
        {
            if(szBuffer[CurIndex] == '\\')
            {
                ReplaceIndex = CurIndex;
                break;
            }
        }
        if(CurIndex == -1)
        {
            ReplaceIndex = 0;
        }

        // Modify buffer again.
        ReplaceChar = szBuffer[ReplaceIndex];
        szBuffer[ReplaceIndex] = '\0';
        DbgPrintf("  New:\"$CurRoot\\%s\"\n", szBuffer);

        mbstowcs(wszObjectName, szBuffer, 160);
        NtDll::RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

        ret = NtDll::NtCreateFile
        (
            FileHandle, DesiredAccess, &NtObjAttr, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock,
            (NtDll::LARGE_INTEGER*)AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, NULL, NULL
        );
    }

    if(FAILED(ret))
    {
        DbgPrintf("EmuKrnl (0x%X): NtCreateFile Failed! (0x%.08X)\n", GetCurrentThreadId(), ret);
    }
    else
    {
        DbgPrintf("EmuKrnl (0x%X): NtCreateFile = 0x%.08X\n", GetCurrentThreadId(), *FileHandle);
    }

    // restore original buffer
    if(ReplaceIndex != -1)
    {
        szBuffer[ReplaceIndex] = ReplaceChar;
    }

    // NOTE: We can map this to IoCreateFile once implemented (if ever necessary)
    //       xboxkrnl::IoCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, 0);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00C0 - NtCreateMutant
// ******************************************************************
XBSYSAPI EXPORTNUM(192) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateMutant
(
    OUT PHANDLE             MutantHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  BOOLEAN             InitialOwner
)
{
    EmuSwapFS();   // Win2k/XP FS

    char *szBuffer = (ObjectAttributes != 0) ? ObjectAttributes->ObjectName->Buffer : 0;

    DbgPrintf("EmuKrnl (0x%X): NtCreateMutant\n"
           "(\n"
           "   MutantHandle        : 0x%.08X\n"
           "   ObjectAttributes    : 0x%.08X (\"%s\")\n"
           "   InitialOwner        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), MutantHandle, ObjectAttributes, szBuffer, InitialOwner);

    wchar_t wszObjectName[160];

    NtDll::UNICODE_STRING    NtUnicodeString;
    NtDll::OBJECT_ATTRIBUTES NtObjAttr;

    // initialize object attributes
    if(szBuffer != 0)
    {
        mbstowcs(wszObjectName, "\\??\\", 4);
        mbstowcs(wszObjectName+4, szBuffer, 160);

        NtDll::RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

        InitializeObjectAttributes(&NtObjAttr, &NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);
    }

    NtObjAttr.RootDirectory = 0;

    // redirect to NtCreateMutant
    NTSTATUS ret = NtDll::NtCreateMutant(MutantHandle, MUTANT_ALL_ACCESS, (szBuffer != 0) ? &NtObjAttr : 0, InitialOwner);

    if(FAILED(ret))
        EmuWarning("NtCreateMutant Failed!");

    DbgPrintf("EmuKrnl (0x%X): NtCreateMutant MutantHandle = 0x%.08X\n", GetCurrentThreadId(), *MutantHandle);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00C1 - NtCreateSemaphore
// ******************************************************************
XBSYSAPI EXPORTNUM(193) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateSemaphore
(
    OUT PHANDLE             SemaphoreHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               InitialCount,
    IN  ULONG               MaximumCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtCreateSemaphore\n"
           "(\n"
           "   SemaphoreHandle     : 0x%.08X\n"
           "   ObjectAttributes    : 0x%.08X\n"
           "   InitialCount        : 0x%.08X\n"
           "   MaximumCount        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), SemaphoreHandle, ObjectAttributes,
           InitialCount, MaximumCount);

    // redirect to Win2k/XP
    NTSTATUS ret = NtDll::NtCreateSemaphore
    (
        SemaphoreHandle,
        SEMAPHORE_ALL_ACCESS,
        (NtDll::POBJECT_ATTRIBUTES)ObjectAttributes,
        InitialCount,
        MaximumCount
    );

    if(FAILED(ret))
        EmuWarning("NtCreateSemaphore failed!");

    DbgPrintf("EmuKrnl (0x%X): NtCreateSemaphore SemaphoreHandle = 0x%.08X\n", GetCurrentThreadId(), *SemaphoreHandle);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00C5 - NtDuplicateObject
// ******************************************************************
XBSYSAPI EXPORTNUM(197) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtDuplicateObject
(
    HANDLE  SourceHandle,
    HANDLE *TargetHandle,
    DWORD   Options
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtDuplicateObject\n"
           "(\n"
           "   SourceHandle        : 0x%.08X\n"
           "   TargetHandle        : 0x%.08X\n"
           "   Options             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), SourceHandle, TargetHandle, Options);

    // redirect to Win2k/XP
    NTSTATUS ret = NtDll::NtDuplicateObject
    (
        GetCurrentProcess(),
        SourceHandle,
        GetCurrentProcess(),
        TargetHandle,
        0, 0, Options
    );

    if(ret != STATUS_SUCCESS)
        EmuWarning("Object was not duplicated!");

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * NtFlushBuffersFile
// ******************************************************************
XBSYSAPI EXPORTNUM(198) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtFlushBuffersFile
(
    PVOID                FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtFlushBuffersFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   IoStatusBlock       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, IoStatusBlock);

    NTSTATUS ret = NtDll::NtFlushBuffersFile(FileHandle, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00C7 - NtFreeVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(199) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtFreeVirtualMemory
(
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG FreeSize,
    IN ULONG      FreeType
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtFreeVirtualMemory\n"
           "(\n"
           "   BaseAddress         : 0x%.08X\n"
           "   FreeSize            : 0x%.08X\n"
           "   FreeType            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress, FreeSize, FreeType);

    NTSTATUS ret = NtDll::NtFreeVirtualMemory(GetCurrentProcess(), BaseAddress, FreeSize, FreeType);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00CA - NtOpenFile
// ******************************************************************
XBSYSAPI EXPORTNUM(202) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtOpenFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               ShareAccess,
    IN  ULONG               OpenOptions
)
{
    // debug trace
    /* Redundant
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuKrnl (0x%X): NtOpenFile\n"
               "(\n"
               "   FileHandle          : 0x%.08X\n"
               "   DesiredAccess       : 0x%.08X\n"
               "   ObjectAttributes    : 0x%.08X (\"%s\")\n"
               "   IoStatusBlock       : 0x%.08X\n"
               "   ShareAccess         : 0x%.08X\n"
               "   CreateOptions       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), FileHandle, DesiredAccess, ObjectAttributes, ObjectAttributes->ObjectName->Buffer,
               IoStatusBlock, ShareAccess, OpenOptions);
        EmuSwapFS();   // Xbox FS
    }
    #endif
    //*/

    return NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, NULL, 0, ShareAccess, FILE_OPEN, OpenOptions);
}

// ******************************************************************
// * 0x00CE - NtQueueApcThread
// ******************************************************************
XBSYSAPI EXPORTNUM(206) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueueApcThread
(
	IN HANDLE               ThreadHandle,
	IN PIO_APC_ROUTINE      ApcRoutine,
	IN PVOID                ApcRoutineContext OPTIONAL,
	IN PIO_STATUS_BLOCK     ApcStatusBlock OPTIONAL,
	IN ULONG                ApcReserved OPTIONAL 
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuKrnl (0x%X): NtQueueApcThread\n"
           "(\n"
           "   ThreadHandle         : 0x%.08X\n"
           "   ApcRoutine           : 0x%.08X\n"
           "   ApcRoutineContext    : 0x%.08X\n"
           "   ApcStatusBlock       : 0x%.08X\n"
           "   ApcReserved          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ThreadHandle, ApcRoutine, ApcRoutineContext,
		   ApcStatusBlock, ApcReserved);

	NTSTATUS ret;

	// TODO: Not too sure how this one works.  If there's any special *magic* that needs to be
	//		 done, let me know!
	ret = NtDll::NtQueueApcThread( (NtDll::HANDLE) ThreadHandle, (NtDll::PIO_APC_ROUTINE) ApcRoutine, ApcRoutineContext, 
									(NtDll::PIO_STATUS_BLOCK) ApcStatusBlock, ApcReserved );
	if( FAILED( ret ) )
		EmuWarning( "NtQueueApcThread failed!" );

	EmuSwapFS();	// Xbox FS

	return ret;
}

// ******************************************************************
// * 0x00CF - NtQueryDirectoryFile
// ******************************************************************
XBSYSAPI EXPORTNUM(207) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryDirectoryFile
(
    IN  HANDLE                      FileHandle,
    IN  HANDLE                      Event OPTIONAL,
    IN  PVOID                       ApcRoutine, // Todo: define this routine's prototype
    IN  PVOID                       ApcContext,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT FILE_DIRECTORY_INFORMATION *FileInformation,
    IN  ULONG                       Length,
    IN  FILE_INFORMATION_CLASS      FileInformationClass,
    IN  PSTRING                     FileMask,
    IN  BOOLEAN                     RestartScan
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtQueryDirectoryFile\n"
           "(\n"
           "   FileHandle           : 0x%.08X\n"
           "   Event                : 0x%.08X\n"
           "   ApcRoutine           : 0x%.08X\n"
           "   ApcContext           : 0x%.08X\n"
           "   IoStatusBlock        : 0x%.08X\n"
           "   FileInformation      : 0x%.08X\n"
           "   Length               : 0x%.08X\n"
           "   FileInformationClass : 0x%.08X\n"
           "   FileMask             : 0x%.08X (%s)\n"
           "   RestartScan          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
           FileInformation, Length, FileInformationClass, FileMask,
           (FileMask != 0) ? FileMask->Buffer : "", RestartScan);

    NTSTATUS ret;

    if(FileInformationClass != 1)   // Due to unicode->string conversion
        CxbxKrnlCleanup("Unsupported FileInformationClass");

    NtDll::UNICODE_STRING NtFileMask;

    wchar_t wszObjectName[160];

    // initialize FileMask
    {
        if(FileMask != 0)
            mbstowcs(wszObjectName, FileMask->Buffer, 160);
        else
            mbstowcs(wszObjectName, "", 160);

        NtDll::RtlInitUnicodeString(&NtFileMask, wszObjectName);
    }

    NtDll::FILE_DIRECTORY_INFORMATION *FileDirInfo = (NtDll::FILE_DIRECTORY_INFORMATION*)CxbxMalloc(0x40 + 160*2);

    char    *mbstr = FileInformation->FileName;
    wchar_t *wcstr = FileDirInfo->FileName;

    do
    {
        ZeroMemory(wcstr, 160*2);

        ret = NtDll::NtQueryDirectoryFile
        (
            FileHandle, Event, (NtDll::PIO_APC_ROUTINE)ApcRoutine, ApcContext, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock, FileDirInfo,
            0x40+160*2, (NtDll::FILE_INFORMATION_CLASS)FileInformationClass, TRUE, &NtFileMask, RestartScan
        );

        // convert from PC to Xbox
        {
            memcpy(FileInformation, FileDirInfo, 0x40);

            wcstombs(mbstr, wcstr, 160);

            FileInformation->FileNameLength /= 2;
        }

        RestartScan = FALSE;
    }
    // Xbox does not return . and ..
    while(strcmp(mbstr, ".") == 0 || strcmp(mbstr, "..") == 0);

    // TODO: Cache the last search result for quicker access with CreateFile (xbox does this internally!)
    CxbxFree(FileDirInfo);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00D2 - NtQueryFullAttributesFile
// ******************************************************************
XBSYSAPI EXPORTNUM(210) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryFullAttributesFile
(
    IN  POBJECT_ATTRIBUTES          ObjectAttributes,
    OUT PVOID                       Attributes
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtQueryFullAttributesFile\n"
           "(\n"
           "   ObjectAttributes    : 0x%.08X (%s)\n"
           "   Attributes          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ObjectAttributes, ObjectAttributes->ObjectName->Buffer, Attributes);

//	__asm int 3;

    char *szBuffer = ObjectAttributes->ObjectName->Buffer;

    wchar_t wszObjectName[160];

    NtDll::UNICODE_STRING    NtUnicodeString;
    NtDll::OBJECT_ATTRIBUTES NtObjAttr;

	// Ensure that we are in the current directory for D:\\.
	// TODO: Other directories when needed.
	if((szBuffer[0] == 'D' || szBuffer[0] == 'd') && szBuffer[1] == ':' || szBuffer[2] == '\\')
	{
		szBuffer += 3;

		 DbgPrintf("EmuXapi (0x%X): NtQueryFullAttributesFile Corrected path...\n", GetCurrentThreadId());
         DbgPrintf("  Org:\"%s\"\n", ObjectAttributes->ObjectName->Buffer);
         DbgPrintf("  New:\"$XbePath\\%s\"\n", szBuffer);
    }

    // initialize object attributes
    {
        mbstowcs(wszObjectName, szBuffer, 160);

        NtDll::RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

        InitializeObjectAttributes(&NtObjAttr, &NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);
    }

    NTSTATUS ret = NtDll::NtQueryFullAttributesFile(&NtObjAttr, Attributes);

	if(FAILED(ret))
		EmuWarning("NtQueryFullAttributesFile failed! (0x%.08X)\n", ret);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00D3 - NtQueryInformationFile
// ******************************************************************
XBSYSAPI EXPORTNUM(211) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryInformationFile
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PVOID                       FileInformation,
    IN  ULONG                       Length,
    IN  FILE_INFORMATION_CLASS      FileInfo
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtQueryInformationFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   IoStatusBlock       : 0x%.08X\n"
           "   FileInformation     : 0x%.08X\n"
           "   Length              : 0x%.08X\n"
           "   FileInformationClass: 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, IoStatusBlock, FileInformation,
           Length, FileInfo);

	// TODO: IIRC, this function is depreciated.  Maybe we should just use
	// ZwQueryInformationFile instead?

//    if(FileInfo != FilePositionInformation && FileInfo != FileNetworkOpenInformation)
//        CxbxKrnlCleanup("Unknown FILE_INFORMATION_CLASS 0x%.08X", FileInfo);

    NTSTATUS ret = NtDll::NtQueryInformationFile
    (
        FileHandle,
        (NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
        (NtDll::PFILE_FS_SIZE_INFORMATION)FileInformation,
        Length,
        (NtDll::FILE_INFORMATION_CLASS)FileInfo
    );

    //
    // DEBUGGING!
    //
    {
        /*
        _asm int 3;
        NtDll::FILE_NETWORK_OPEN_INFORMATION *pInfo = (NtDll::FILE_NETWORK_OPEN_INFORMATION*)FileInformation;

        if(FileInfo == FileNetworkOpenInformation && (pInfo->AllocationSize.LowPart == 57344))
        {
            DbgPrintf("pInfo->AllocationSize : %d\n", pInfo->AllocationSize.LowPart);
            DbgPrintf("pInfo->EndOfFile      : %d\n", pInfo->EndOfFile.LowPart);

            pInfo->EndOfFile.LowPart = 0x1000;
            pInfo->AllocationSize.LowPart = 0x1000;

            fflush(stdout);
        }
        */
    }

    if(FAILED(ret))
        EmuWarning("NtQueryInformationFile failed! (0x%.08X)", ret);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00D9 - NtQueryVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(217) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryVirtualMemory
(
    IN  PVOID                       BaseAddress,
    OUT PMEMORY_BASIC_INFORMATION   Buffer
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtQueryVirtualMemory\n"
           "(\n"
           "   BaseAddress         : 0x%.08X\n"
           "   Buffer              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BaseAddress, Buffer);

    NTSTATUS ret = NtDll::NtQueryVirtualMemory
    (
        GetCurrentProcess(),
        BaseAddress,
        (NtDll::MEMORY_INFORMATION_CLASS)NtDll::MemoryBasicInformation,
        (NtDll::PMEMORY_BASIC_INFORMATION)Buffer,
        sizeof(MEMORY_BASIC_INFORMATION),
        0
    );

    if(FAILED(ret))
        EmuWarning("NtQueryVirtualMemory failed!");

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00DA - NtQueryVolumeInformationFile
// ******************************************************************
XBSYSAPI EXPORTNUM(218) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryVolumeInformationFile
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
    IN  ULONG                       Length,
    IN  FS_INFORMATION_CLASS        FileInformationClass
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtQueryVolumeInformationFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   IoStatusBlock       : 0x%.08X\n"
           "   FileInformation     : 0x%.08X\n"
           "   Length              : 0x%.08X\n"
           "   FileInformationClass: 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, IoStatusBlock, FileInformation,
           Length, FileInformationClass);

    // Safety/Sanity Check
    if((FileInformationClass != FileFsSizeInformation) && (FileInformationClass != FileDirectoryInformation))
        CxbxKrnlCleanup("NtQueryVolumeInformationFile: Unsupported FileInformationClass");

    NTSTATUS ret = NtDll::NtQueryVolumeInformationFile
    (
        FileHandle,
        (NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
        (NtDll::PFILE_FS_SIZE_INFORMATION)FileInformation, Length,
        (NtDll::FS_INFORMATION_CLASS)FileInformationClass
    );

    // NOTE: TODO: Dynamically fill in, or allow configuration?
    if(FileInformationClass == FileFsSizeInformation)
    {
        FILE_FS_SIZE_INFORMATION *SizeInfo = (FILE_FS_SIZE_INFORMATION*)FileInformation;

        SizeInfo->TotalAllocationUnits.QuadPart     = 0x4C468;
        SizeInfo->AvailableAllocationUnits.QuadPart = 0x2F125;
        SizeInfo->SectorsPerAllocationUnit          = 32;
        SizeInfo->BytesPerSector                    = 512;
    }

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00DA - NtReadFile
// ******************************************************************
XBSYSAPI EXPORTNUM(219) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtReadFile
(
    IN  HANDLE          FileHandle,            // TODO: correct paramters
    IN  HANDLE          Event OPTIONAL,
    IN  PVOID           ApcRoutine OPTIONAL,
    IN  PVOID           ApcContext,
    OUT PVOID           IoStatusBlock,
    OUT PVOID           Buffer,
    IN  ULONG           Length,
    IN  PLARGE_INTEGER  ByteOffset OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtReadFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   Event               : 0x%.08X\n"
           "   ApcRoutine          : 0x%.08X\n"
           "   ApcContext          : 0x%.08X\n"
           "   IoStatusBlock       : 0x%.08X\n"
           "   Buffer              : 0x%.08X\n"
           "   Length              : 0x%.08X\n"
           "   ByteOffset          : 0x%.08X (0x%.08X)\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, Event, ApcRoutine,
           ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, ByteOffset == 0 ? 0 : ByteOffset->QuadPart);

// Halo...
//    if(ByteOffset != 0 && ByteOffset->QuadPart == 0x00120800)
//        _asm int 3

    NTSTATUS ret = NtDll::NtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, (NtDll::LARGE_INTEGER*)ByteOffset, 0);

    if(FAILED(ret))
        EmuWarning("NtReadFile Failed!");

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00DD - NtReleaseMutant
// ******************************************************************
XBSYSAPI EXPORTNUM(221) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtReleaseMutant
(
    IN  HANDLE  MutantHandle,
    OUT PLONG   PreviousCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtReleaseMutant\n"
           "(\n"
           "   MutantHandle         : 0x%.08X\n"
           "   PreviousCount        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), MutantHandle, PreviousCount);

    // redirect to NtCreateMutant
    NTSTATUS ret = NtDll::NtReleaseMutant(MutantHandle, PreviousCount);

    if(FAILED(ret))
        EmuWarning("NtReleaseMutant Failed!");

    EmuSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00DE - NtReleaseSemaphore
// ******************************************************************
XBSYSAPI EXPORTNUM(222) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtReleaseSemaphore
(
    IN  HANDLE              SemaphoreHandle,
    IN  ULONG               ReleaseCount,
    OUT PULONG              PreviousCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtReleaseSemaphore\n"
           "(\n"
           "   SemaphoreHandle      : 0x%.08X\n"
           "   ReleaseCount         : 0x%.08X\n"
           "   PreviousCount        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), SemaphoreHandle, ReleaseCount, PreviousCount);

    NTSTATUS ret = NtDll::NtReleaseSemaphore(SemaphoreHandle, ReleaseCount, PreviousCount);

    if(FAILED(ret))
        EmuWarning("NtReleaseSemaphore failed!");

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00E0 - NtResumeThread
// ******************************************************************
XBSYSAPI EXPORTNUM(224) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtResumeThread
(
    IN  HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtResumeThread\n"
           "(\n"
           "   ThreadHandle         : 0x%.08X\n"
           "   PreviousSuspendCount : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ThreadHandle, PreviousSuspendCount);

    NTSTATUS ret = NtDll::NtResumeThread(ThreadHandle, PreviousSuspendCount);

    Sleep(10);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00E1 - NtSetEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(225) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetEvent
(
    IN  HANDLE EventHandle,
    OUT PLONG  PreviousState
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtSetEvent\n"
           "(\n"
           "   EventHandle          : 0x%.08X\n"
           "   PreviousState        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), EventHandle, PreviousState);

    NTSTATUS ret = NtDll::NtSetEvent(EventHandle, PreviousState);

    if(FAILED(ret))
        EmuWarning("NtSetEvent Failed!");

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00E2 - NtSetInformationFile
// ******************************************************************
XBSYSAPI EXPORTNUM(226) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetInformationFile
(
    IN  HANDLE  FileHandle,            // TODO: correct paramters
    OUT PVOID   IoStatusBlock,
    IN  PVOID   FileInformation,
    IN  ULONG   Length,
    IN  ULONG   FileInformationClass
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtSetInformationFile\n"
           "(\n"
           "   FileHandle           : 0x%.08X\n"
           "   IoStatusBlock        : 0x%.08X\n"
           "   FileInformation      : 0x%.08X\n"
           "   Length               : 0x%.08X\n"
           "   FileInformationClass : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, IoStatusBlock, FileInformation,
           Length, FileInformationClass);

    NTSTATUS ret = NtDll::NtSetInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00E4 - NtSetSystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(228) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetSystemTime
(
	IN  PLARGE_INTEGER			SystemTime,
	OUT PLARGE_INTEGER			PreviousTime OPTIONAL 
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtSetInformationFile\n"
           "(\n"
           "   SystemTime           : 0x%.08X\n"
           "   PreviousTime         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), SystemTime, PreviousTime);

	// Maybe it's not such a good idea to allow Cxbx to change your time 
	// clock.  Might need admin privileges to do this.... dunno.

    NTSTATUS ret = STATUS_SUCCESS;

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00E7 - NtSuspendThread
// ******************************************************************
XBSYSAPI EXPORTNUM(231) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSuspendThread
(
    IN  HANDLE  ThreadHandle,
    OUT PULONG  PreviousSuspendCount OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtSuspendThread\n"
           "(\n"
           "   ThreadHandle         : 0x%.08X\n"
           "   PreviousSuspendCount : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ThreadHandle, PreviousSuspendCount);

    NTSTATUS ret = NtDll::NtSuspendThread(ThreadHandle, PreviousSuspendCount);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00E8 - NtUserIoApcDispatcher
// ******************************************************************
XBSYSAPI EXPORTNUM(232) VOID NTAPI xboxkrnl::NtUserIoApcDispatcher
(
    PVOID            ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG            Reserved
)
{
    // Note: This function is called within Win2k/XP context, so no EmuSwapFS here

    DbgPrintf("EmuKrnl (0x%X): NtUserIoApcDispatcher\n"
           "(\n"
           "   ApcContext           : 0x%.08X\n"
           "   IoStatusBlock        : 0x%.08X\n"
           "   Reserved             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ApcContext, IoStatusBlock, Reserved);

    DbgPrintf("IoStatusBlock->Pointer     : 0x%.08X\n"
              "IoStatusBlock->Information : 0x%.08X\n", IoStatusBlock->u1.Pointer, IoStatusBlock->Information);

    EmuSwapFS();   // Xbox FS

    uint32 dwEsi, dwEax, dwEcx;

    dwEsi = (uint32)IoStatusBlock;

    if((IoStatusBlock->u1.Status & 0xC0000000) == 0xC0000000)
    {
        dwEcx = 0;
        dwEax = NtDll::RtlNtStatusToDosError(IoStatusBlock->u1.Status);
    }
    else
    {
        dwEcx = (DWORD)IoStatusBlock->Information;
        dwEax = 0;
    }

    /*
    // ~XDK 3911??
    if(true)
    {
        dwEsi = dw2;
        dwEcx = dw1;
        dwEax = dw3;

    }
    else
    {
        dwEsi = dw1;
        dwEcx = dw2;
        dwEax = dw3;
    }//*/

    __asm
    {
        pushad
        /*
        mov esi, IoStatusBlock
        mov ecx, dwEcx
        mov eax, dwEax
        */
        // TODO: Figure out if/why this works!? Matches prototype, but not xboxkrnl disassembly
        // Seems to be XDK/version dependand??
        mov esi, dwEsi
        mov ecx, dwEcx
        mov eax, dwEax

        push esi
        push ecx
        push eax

        call ApcContext

        popad
    }

    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtUserIoApcDispatcher Completed\n", GetCurrentThreadId());

    return;
}

// ******************************************************************
// * 0x00EA - NtWaitForSingleObjectEx
// ******************************************************************
XBSYSAPI EXPORTNUM(234) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWaitForSingleObjectEx
(
    IN  HANDLE          Handle,
    IN  CHAR            WaitMode,
    IN  BOOLEAN         Alertable,
    IN  PLARGE_INTEGER  Timeout
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtWaitForSingleObjectEx\n"
           "(\n"
           "   Handle               : 0x%.08X\n"
           "   WaitMode             : 0x%.08X\n"
           "   Alertable            : 0x%.08X\n"
           "   Timeout              : 0x%.08X (%d)\n"
           ");\n",
           GetCurrentThreadId(), Handle, WaitMode, Alertable, Timeout, Timeout == 0 ? 0 : Timeout->QuadPart);

    NTSTATUS ret = NtDll::NtWaitForSingleObject(Handle, Alertable, (NtDll::PLARGE_INTEGER)Timeout);

    DbgPrintf("Finished waiting for 0x%.08X\n", Handle);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * NtWaitForMultipleObjectsEx
// ******************************************************************
XBSYSAPI EXPORTNUM(235) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWaitForMultipleObjectsEx
(
    IN  ULONG           Count,
    IN  HANDLE         *Handles,
    IN  WAIT_TYPE       WaitType,
    IN  CHAR            WaitMode,
    IN  BOOLEAN         Alertable,
    IN  PLARGE_INTEGER  Timeout
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtWaitForMultipleObjectsEx\n"
           "(\n"
           "   Count                : 0x%.08X\n"
           "   Handles              : 0x%.08X\n"
           "   WaitType             : 0x%.08X\n"
           "   WaitMode             : 0x%.08X\n"
           "   Alertable            : 0x%.08X\n"
           "   Timeout              : 0x%.08X (%d)\n"
           ");\n",
           GetCurrentThreadId(), Count, Handles, WaitType, WaitMode, Alertable,
           Timeout, Timeout == 0 ? 0 : Timeout->QuadPart);

    NTSTATUS ret = NtDll::NtWaitForMultipleObjects(Count, Handles, (NtDll::OBJECT_WAIT_TYPE)WaitType, Alertable, (NtDll::PLARGE_INTEGER)Timeout);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00EC - NtWriteFile
// ******************************************************************
XBSYSAPI EXPORTNUM(236) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWriteFile
(
    IN  HANDLE          FileHandle,            // TODO: correct paramters
    IN  PVOID           Event,
    IN  PVOID           ApcRoutine,
    IN  PVOID           ApcContext,
    OUT PVOID           IoStatusBlock,
    IN  PVOID           Buffer,
    IN  ULONG           Length,
    IN  PLARGE_INTEGER  ByteOffset
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): NtWriteFile\n"
           "(\n"
           "   FileHandle          : 0x%.08X\n"
           "   Event               : 0x%.08X\n"
           "   ApcRoutine          : 0x%.08X\n"
           "   ApcContext          : 0x%.08X\n"
           "   IoStatusBlock       : 0x%.08X\n"
           "   Buffer              : 0x%.08X\n"
           "   Length              : 0x%.08X\n"
           "   ByteOffset          : 0x%.08X (0x%.08X)\n"
           ");\n",
           GetCurrentThreadId(), FileHandle, Event, ApcRoutine,
           ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, ByteOffset == 0 ? 0 : ByteOffset->QuadPart);

// Halo..
//    if(ByteOffset != 0 && ByteOffset->QuadPart == 0x01C00800)
//        _asm int 3

    NTSTATUS ret = NtDll::NtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, (NtDll::LARGE_INTEGER*)ByteOffset, 0);

    if(FAILED(ret))
        EmuWarning("NtWriteFile Failed! (0x%.08X)", ret);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00EE - NtYieldExecution
// ******************************************************************
XBSYSAPI EXPORTNUM(238) VOID NTAPI xboxkrnl::NtYieldExecution()
{
    EmuSwapFS();   // Win2k/XP FS

    // NOTE: this eats up the debug log far too quickly
    //DbgPrintf("EmuKrnl (0x%X): NtYieldExecution();\n", GetCurrentThreadId());

    NtDll::NtYieldExecution();

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x00FF - PsCreateSystemThreadEx
// ******************************************************************
XBSYSAPI EXPORTNUM(255) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PsCreateSystemThreadEx
(
    OUT PHANDLE         ThreadHandle,
    IN  ULONG           ThreadExtraSize,
    IN  ULONG           KernelStackSize,
    IN  ULONG           TlsDataSize,
    OUT PULONG          ThreadId OPTIONAL,
    IN  PVOID           StartContext1,
    IN  PVOID           StartContext2,
    IN  BOOLEAN         CreateSuspended,
    IN  BOOLEAN         DebugStack,
    IN  PKSTART_ROUTINE StartRoutine
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): PsCreateSystemThreadEx\n"
           "(\n"
           "   ThreadHandle        : 0x%.08X\n"
           "   ThreadExtraSize     : 0x%.08X\n"
           "   KernelStackSize     : 0x%.08X\n"
           "   TlsDataSize         : 0x%.08X\n"
           "   ThreadId            : 0x%.08X\n"
           "   StartContext1       : 0x%.08X\n"
           "   StartContext2       : 0x%.08X\n"
           "   CreateSuspended     : 0x%.08X\n"
           "   DebugStack          : 0x%.08X\n"
           "   StartRoutine        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ThreadHandle, ThreadExtraSize, KernelStackSize, TlsDataSize, ThreadId,
           StartContext1, StartContext2, CreateSuspended, DebugStack, StartRoutine);

	static bool bFirstTime = false;

    // create thread, using our special proxy technique
    {
        DWORD dwThreadId;

        // PCSTProxy is responsible for cleaning up this pointer
        ::PCSTProxyParam *iPCSTProxyParam = new ::PCSTProxyParam();

        iPCSTProxyParam->StartContext1 = StartContext1;
        iPCSTProxyParam->StartContext2 = StartContext2;
        iPCSTProxyParam->StartRoutine  = StartRoutine;
        iPCSTProxyParam->StartSuspended = CreateSuspended;
        iPCSTProxyParam->hStartedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        *ThreadHandle = (HANDLE)_beginthreadex(NULL, NULL, PCSTProxy, iPCSTProxyParam, NULL, (uint*)&dwThreadId);

        WaitForSingleObject(iPCSTProxyParam->hStartedEvent, 1000);

//        *ThreadHandle = CreateThread(NULL, NULL, PCSTProxy, iPCSTProxyParam, NULL, &dwThreadId);

        DbgPrintf("EmuKrnl (0x%X): ThreadHandle : 0x%X, ThreadId : 0x%.08X\n", GetCurrentThreadId(), *ThreadHandle, dwThreadId);

        // we must duplicate this handle in order to retain Suspend/Resume thread rights from a remote thread
        {
            HANDLE hDupHandle = NULL;

            DuplicateHandle(GetCurrentProcess(), *ThreadHandle, GetCurrentProcess(), &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

            CxbxKrnlRegisterThread(hDupHandle);
        }

        if(ThreadId != NULL)
            *ThreadId = dwThreadId;
    }

    EmuSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0102 - PsTerminateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(258) VOID NTAPI xboxkrnl::PsTerminateSystemThread(IN NTSTATUS ExitStatus)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): PsTerminateSystemThread\n"
           "(\n"
           "   ExitStatus          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ExitStatus);

    // call thread notification routine(s)
    if(g_iThreadNotificationCount != 0)
    {
		for(int i = 0; i < 16; i++ )
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if(pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmKrnl (0x%X): Calling pfnNotificationRoutine[%d] (0x%.08X)\n", g_iThreadNotificationCount, GetCurrentThreadId(), pfnNotificationRoutine);

			EmuSwapFS();   // Xbox FS

			pfnNotificationRoutine(FALSE);

			EmuSwapFS();   // Win2k/XP FS
		}
    }

//    CxbxKrnlTerminateThread();

    EmuCleanupFS();

    _endthreadex(ExitStatus);
    //ExitThread(ExitStatus);

    return;
}

// ******************************************************************
// * 0x0104 - RtlAnsiStringToUnicodeString
// ******************************************************************
XBSYSAPI EXPORTNUM(260) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlAnsiStringToUnicodeString
(
    PUNICODE_STRING DestinationString,
    PSTRING         SourceString,
    UCHAR           AllocateDestinationString
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlAnsiStringToUnicodeString\n"
           "(\n"
           "   DestinationString         : 0x%.08X\n"
           "   SourceString              : 0x%.08X\n"
           "   AllocateDestinationString : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), DestinationString, SourceString, AllocateDestinationString);

    NTSTATUS ret = NtDll::RtlAnsiStringToUnicodeString((NtDll::UNICODE_STRING*)DestinationString, (NtDll::STRING*)SourceString, AllocateDestinationString);

    EmuSwapFS();   // Xbox FS

    return ret;
}
// ******************************************************************
// * 0x0108 - RtlAssert - Debug API?
// ******************************************************************
XBSYSAPI EXPORTNUM(264) VOID NTAPI xboxkrnl::RtlAssert
(
    PVOID   FailedAssertion,
    PVOID   FileName,
    ULONG   LineNumber,
    PCHAR   Message
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlAssert\n"
           "(\n"
           "   FailedAssertion           : 0x%.08X\n"
           "   FileName                  : 0x%.08X\n"
           "   LineNumber                : 0x%.08X\n"
           "   Message                   : 0x%.08X (\"%s\")\n"
           ");\n",
           GetCurrentThreadId(), FailedAssertion, FileName, Message, Message);

    //TODO: Actually implement this.
    //NTSTATUS ret = NtDll::RtlAssert((NtDll::UNICODE_STRING*)DestinationString, (NtDll::STRING*)SourceString, AllocateDestinationString);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0115 RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(277) VOID NTAPI xboxkrnl::RtlEnterCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuSwapFS();   // Win2k/XP FS

    /** sorta pointless
    DbgPrintf("EmuKrnl (0x%X): RtlEnterCriticalSection\n"
           "(\n"
           "   CriticalSection     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), CriticalSection);
    //*/

    //printf("CriticalSection->LockCount : %d\n", CriticalSection->LockCount);

    // This seems redundant, but xbox software doesn't always do it
    if(CriticalSection->LockCount == -1)
        NtDll::RtlInitializeCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);

    NtDll::RtlEnterCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0117 - RtlEqualString
// ******************************************************************
XBSYSAPI EXPORTNUM(279) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlEqualString
(
  IN PSTRING String1,
  IN PSTRING String2,
  IN BOOLEAN CaseSensitive
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuKrnl (0x%X): RtlEqualString\n"
			"(\n"
			"	String2            : 0x%.08X (\"%s\")\n"
			"	String2            : 0x%.08X (\"%s\")\n"
			"	CaseSensitive      : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), String1, String1, String2, String2, CaseSensitive );

	BOOLEAN bRet = NtDll::RtlEqualString( (NtDll::PSTRING)String1, (NtDll::PSTRING)String2, (NtDll::BOOLEAN)CaseSensitive );

	EmuSwapFS();	// Xbox FS

	return bRet;
}

// ******************************************************************
// * 0x0121 - RtlInitAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(289) VOID NTAPI xboxkrnl::RtlInitAnsiString
(
  IN OUT PANSI_STRING DestinationString,
  IN     PCSZ         SourceString
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlInitAnsiString\n"
           "(\n"
           "   DestinationString   : 0x%.08X\n"
           "   SourceString        : 0x%.08X (\"%s\")\n"
           ");\n",
           GetCurrentThreadId(), DestinationString, SourceString, SourceString);

    NtDll::RtlInitAnsiString((NtDll::PANSI_STRING)DestinationString, (NtDll::PCSZ)SourceString);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0123 - RtlInitializeCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(291) VOID NTAPI xboxkrnl::RtlInitializeCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuSwapFS();   // Win2k/XP FS

    /*
    DbgPrintf("EmuKrnl (0x%X): RtlInitializeCriticalSection\n"
           "(\n"
           "   CriticalSection     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), CriticalSection);
    //*/

    NtDll::RtlInitializeCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0126 RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(294) VOID NTAPI xboxkrnl::RtlLeaveCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuSwapFS();   // Win2k/XP FS

    // Note: We need to execute this before debug output to avoid trouble
    NtDll::RtlLeaveCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);

    /* sorta pointless
    DbgPrintf("EmuKrnl (0x%X): RtlLeaveCriticalSection\n"
           "(\n"
           "   CriticalSection     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), CriticalSection);
    //*/

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * RtlLowerChar
// ******************************************************************
XBSYSAPI EXPORTNUM(296) xboxkrnl::CHAR NTAPI xboxkrnl::RtlLowerChar(CHAR Character)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlLowerChar(%c)\n", GetCurrentThreadId(), Character);

    CHAR ret = tolower(Character);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x012D - RtlNtStatusToDosError
// ******************************************************************
XBSYSAPI EXPORTNUM(301) xboxkrnl::ULONG NTAPI xboxkrnl::RtlNtStatusToDosError
(
    IN NTSTATUS Status
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlNtStatusToDosError\n"
           "(\n"
           "   Status              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Status);

    ULONG ret = NtDll::RtlNtStatusToDosError(Status);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x0130 - RtlTimeFieldsToTime
// ******************************************************************
XBSYSAPI EXPORTNUM(304) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlTimeFieldsToTime
(
    IN  PTIME_FIELDS    TimeFields,
    OUT PLARGE_INTEGER  Time
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlTimeFieldsToTime\n"
           "(\n"
           "   TimeFields          : 0x%.08X\n"
           "   Time                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), TimeFields, Time);

    BOOLEAN bRet = NtDll::RtlTimeFieldsToTime((NtDll::TIME_FIELDS*)TimeFields, (NtDll::LARGE_INTEGER*)Time);

    EmuSwapFS();   // Xbox FS

    return bRet;
}

// ******************************************************************
// * 0x0131 - RtlTimeToTimeFields
// ******************************************************************
XBSYSAPI EXPORTNUM(305) VOID NTAPI xboxkrnl::RtlTimeToTimeFields
(
    IN  PLARGE_INTEGER  Time,
    OUT PTIME_FIELDS    TimeFields
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlTimeToTimeFields\n"
           "(\n"
           "   Time                : 0x%.08X\n"
           "   TimeFields          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Time, TimeFields);

    NtDll::RtlTimeToTimeFields((NtDll::LARGE_INTEGER*)Time, (NtDll::TIME_FIELDS*)TimeFields);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * RtlTryEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(306) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlTryEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlTryEnterCriticalSection\n"
           "(\n"
           "   CriticalSection     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), CriticalSection);

    BOOL bRet = NtDll::RtlTryEnterCriticalSection((NtDll::PRTL_CRITICAL_SECTION)CriticalSection);

    EmuSwapFS();   // Xbox FS

    return bRet;
}

// ******************************************************************
// * 0x0134 - RtlUnicodeStringToAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(308) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUnicodeStringToAnsiString
(
    IN OUT PSTRING         DestinationString,
    IN     PUNICODE_STRING SourceString,
    IN     BOOLEAN         AllocateDestinationString
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): RtlUnicodeStringToAnsiString\n"
           "(\n"
           "   DestinationString         : 0x%.08X\n"
           "   SourceString              : 0x%.08X\n"
           "   AllocateDestinationString : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), DestinationString, SourceString, AllocateDestinationString);

    NTSTATUS ret = NtDll::RtlUnicodeStringToAnsiString((NtDll::STRING*)DestinationString, (NtDll::UNICODE_STRING*)SourceString, AllocateDestinationString);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x0142 - XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) XBOX_HARDWARE_INFO xboxkrnl::XboxHardwareInfo =
{
    0xC0000035,
    0,0,0,0
};

// ******************************************************************
// * 0x0143 - XboxHDKey
// ******************************************************************
XBSYSAPI EXPORTNUM(323) xboxkrnl::UCHAR xboxkrnl::XboxHDKey[16] =
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

// ******************************************************************
// * XboxSignatureKey
// ******************************************************************
XBSYSAPI EXPORTNUM(325) xboxkrnl::BYTE xboxkrnl::XboxSignatureKey[16] =
{
    // cxbx default saved game key
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ******************************************************************
// * XcSHAInit
// ******************************************************************
XBSYSAPI EXPORTNUM(335) VOID NTAPI xboxkrnl::XcSHAInit(UCHAR *pbSHAContext)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): XcSHAInit\n"
           "(\n"
           "   pbSHAContext        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pbSHAContext);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * XcSHAUpdate
// ******************************************************************
XBSYSAPI EXPORTNUM(336) VOID NTAPI xboxkrnl::XcSHAUpdate(UCHAR *pbSHAContext, UCHAR *pbInput, ULONG dwInputLength)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): XcSHAUpdate\n"
           "(\n"
           "   pbSHAContext        : 0x%.08X\n"
           "   pbInput             : 0x%.08X\n"
           "   dwInputLength       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pbSHAContext, pbInput, dwInputLength);

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * XcSHAFinal
// ******************************************************************
XBSYSAPI EXPORTNUM(337) VOID NTAPI xboxkrnl::XcSHAFinal(UCHAR *pbSHAContext, UCHAR *pbDigest)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuKrnl (0x%X): XcSHAFinal\n"
           "(\n"
           "   pbSHAContext        : 0x%.08X\n"
           "   pbDigest            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pbSHAContext, pbDigest);

    // for now, we dont care about the digest
    for(int v=0;v<20;v++)
    {
        pbDigest[v] = 0;
    }

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * HalBootSMCVideoMode
// ******************************************************************
// TODO: Verify this!
XBSYSAPI EXPORTNUM(356) xboxkrnl::DWORD xboxkrnl::HalBootSMCVideoMode = 1;
