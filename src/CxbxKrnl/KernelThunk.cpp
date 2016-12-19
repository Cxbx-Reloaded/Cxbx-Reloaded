// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->KernelThunkTable.cpp
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

#include "Cxbx.h" // For CXBXKRNL_API, CxbxKrnl_KernelThunkTable
#include "CxbxKrnl.h" // For UINT

//
// Enable "#define PANIC(numb) numb" if you wish to find out what
// kernel export the application is attempting to call. The app
// will crash at the thunk number (i.e. PsQueryStatistics:0x0100)
//
// For general use, you should probably just enable the other
// option "#define PANIC(numb) cxbx_panic"
//
//#define PANIC(numb) CxbxKrnlPanic
#define PANIC(numb) numb

#define FUNC(f) f
#define VARIABLE(v) v

#define DEVKIT // developer kit only functions
#define PROFILING // private kernel profiling functions
// A.k.a. _XBOX_ENABLE_PROFILING

// kernel thunk table
// Note : Names that collide with other symbols, use the KRNL() macro.
extern "C" CXBXKRNL_API uint32 CxbxKrnl_KernelThunkTable[379] =
{
	(uint32)PANIC(0x0000),                                    // 0x0000 (0)   NULL
	(uint32)FUNC(&xboxkrnl::AvGetSavedDataAddress),           // 0x0001 (1)
	(uint32)FUNC(&xboxkrnl::AvSendTVEncoderOption),           // 0x0002 (2)
	(uint32)FUNC(&xboxkrnl::AvSetDisplayMode),                // 0x0003 (3)
	(uint32)FUNC(&xboxkrnl::AvSetSavedDataAddress),           // 0x0004 (4)
	(uint32)FUNC(&xboxkrnl::DbgBreakPoint),                   // 0x0005 (5)
	(uint32)FUNC(&xboxkrnl::DbgBreakPointWithStatus),         // 0x0006 (6)
	(uint32)FUNC(&xboxkrnl::DbgLoadImageSymbols),             // 0x0007 (7) DEVKIT
	(uint32)FUNC(&xboxkrnl::DbgPrint),                        // 0x0008 (8)
	(uint32)FUNC(&xboxkrnl::HalReadSMCTrayState),             // 0x0009 (9)
	(uint32)FUNC(&xboxkrnl::DbgPrompt),                       // 0x000A (10)
	(uint32)FUNC(&xboxkrnl::DbgUnLoadImageSymbols),           // 0x000B (11) DEVKIT
	(uint32)FUNC(&xboxkrnl::ExAcquireReadWriteLockExclusive), // 0x000C (12)
	(uint32)FUNC(&xboxkrnl::ExAcquireReadWriteLockShared),    // 0x000D (13)
	(uint32)FUNC(&xboxkrnl::ExAllocatePool),                  // 0x000E (14)
	(uint32)FUNC(&xboxkrnl::ExAllocatePoolWithTag),           // 0x000F (15)
	(uint32)VARIABLE(&xboxkrnl::ExEventObjectType),           // 0x0010 (16)
	(uint32)FUNC(&xboxkrnl::ExFreePool),                      // 0x0011 (17)
	(uint32)FUNC(&xboxkrnl::ExInitializeReadWriteLock),       // 0x0012 (18)
	(uint32)FUNC(&xboxkrnl::ExInterlockedAddLargeInteger),    // 0x0013 (19)
	(uint32)FUNC(&xboxkrnl::ExInterlockedAddLargeStatistic),  // 0x0014 (20)
	(uint32)FUNC(&xboxkrnl::ExInterlockedCompareExchange64),  // 0x0015 (21)
	(uint32)VARIABLE(&xboxkrnl::ExMutantObjectType),          // 0x0016 (22)
	(uint32)FUNC(&xboxkrnl::ExQueryPoolBlockSize),            // 0x0017 (23)
	(uint32)FUNC(&xboxkrnl::ExQueryNonVolatileSetting),       // 0x0018 (24)
	(uint32)FUNC(&xboxkrnl::ExReadWriteRefurbInfo),           // 0x0019 (25)
	(uint32)FUNC(&xboxkrnl::ExRaiseException),                // 0x001A (26)
	(uint32)FUNC(&xboxkrnl::ExRaiseStatus),                   // 0x001B (27)
	(uint32)FUNC(&xboxkrnl::ExReleaseReadWriteLock),          // 0x001C (28)
	(uint32)FUNC(&xboxkrnl::ExSaveNonVolatileSetting),        // 0x001D (29)
	(uint32)VARIABLE(&xboxkrnl::ExSemaphoreObjectType),       // 0x001E (30)
	(uint32)VARIABLE(&xboxkrnl::ExTimerObjectType),           // 0x001F (31)
	(uint32)FUNC(&xboxkrnl::ExfInterlockedInsertHeadList),    // 0x0020 (32)
	(uint32)FUNC(&xboxkrnl::ExfInterlockedInsertTailList),    // 0x0021 (33)
	(uint32)FUNC(&xboxkrnl::ExfInterlockedRemoveHeadList),    // 0x0022 (34)
	(uint32)FUNC(&xboxkrnl::FscGetCacheSize),                 // 0x0023 (35)
	(uint32)FUNC(&xboxkrnl::FscInvalidateIdleBlocks),         // 0x0024 (36)
	(uint32)FUNC(&xboxkrnl::FscSetCacheSize),                 // 0x0025 (37)
	(uint32)FUNC(&xboxkrnl::HalClearSoftwareInterrupt),       // 0x0026 (38)
	(uint32)FUNC(&xboxkrnl::HalDisableSystemInterrupt),       // 0x0027 (39)
	(uint32)VARIABLE(&xboxkrnl::HalDiskCachePartitionCount),  // 0x0028 (40)  A.k.a. "IdexDiskPartitionPrefixBuffer"
	(uint32)VARIABLE(&xboxkrnl::HalDiskModelNumber),          // 0x0029 (41)
	(uint32)VARIABLE(&xboxkrnl::HalDiskSerialNumber),         // 0x002A (42)
	(uint32)FUNC(&xboxkrnl::HalEnableSystemInterrupt),        // 0x002B (43)
	(uint32)FUNC(&xboxkrnl::HalGetInterruptVector),           // 0x002C (44)
	(uint32)FUNC(&xboxkrnl::HalReadSMBusValue),               // 0x002D (45)
	(uint32)FUNC(&xboxkrnl::HalReadWritePCISpace),            // 0x002E (46)
	(uint32)FUNC(&xboxkrnl::HalRegisterShutdownNotification), // 0x002F (47)
	(uint32)FUNC(&xboxkrnl::HalRequestSoftwareInterrupt),     // 0x0030 (48)
	(uint32)FUNC(&xboxkrnl::HalReturnToFirmware),             // 0x0031 (49)
	(uint32)FUNC(&xboxkrnl::HalWriteSMBusValue),              // 0x0032 (50)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedCompareExchange)),// 0x0033 (51)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedDecrement)),      // 0x0034 (52)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedIncrement)),      // 0x0035 (53)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedExchange)),       // 0x0036 (54)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedExchangeAdd)),    // 0x0037 (55)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedFlushSList)),     // 0x0038 (56)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedPopEntrySList)),  // 0x0039 (57)
	(uint32)FUNC(&xboxkrnl::KRNL(InterlockedPushEntrySList)), // 0x003A (58)
	(uint32)PANIC(0x003B),                                    // 0x003B (59)  IoAllocateIrp
	(uint32)PANIC(0x003C),                                    // 0x003C (60)  IoBuildAsynchronousFsdRequest
	(uint32)PANIC(0x003D),                                    // 0x003D (61)  IoBuildDeviceIoControlRequest
	(uint32)PANIC(0x003E),                                    // 0x003E (62)  IoBuildSynchronousFsdRequest
	(uint32)PANIC(0x003F),                                    // 0x003F (63)  IoCheckShareAccess
	(uint32)VARIABLE(&xboxkrnl::IoCompletionObjectType),      // 0x0040 (64)
	(uint32)PANIC(0x0041),                                    // 0x0041 (65)  IoCreateDevice
	(uint32)FUNC(&xboxkrnl::IoCreateFile),                    // 0x0042 (66)
	(uint32)FUNC(&xboxkrnl::IoCreateSymbolicLink),            // 0x0043 (67)
	(uint32)PANIC(0x0044),                                    // 0x0044 (68)  IoDeleteDevice
	(uint32)FUNC(&xboxkrnl::IoDeleteSymbolicLink),            // 0x0045 (69)
	(uint32)VARIABLE(&xboxkrnl::IoDeviceObjectType),          // 0x0046 (70)
	(uint32)VARIABLE(&xboxkrnl::IoFileObjectType),            // 0x0047 (71)
	(uint32)PANIC(0x0048),                                    // 0x0048 (72)  IoFreeIrp
	(uint32)PANIC(0x0049),                                    // 0x0049 (73)  IoInitializeIrp
	(uint32)PANIC(0x004A),                                    // 0x004A (74)  IoInvalidDeviceRequest
	(uint32)PANIC(0x004B),                                    // 0x004B (75)  IoQueryFileInformation
	(uint32)PANIC(0x004C),                                    // 0x004C (76)  IoQueryVolumeInformation
	(uint32)PANIC(0x004D),                                    // 0x004D (77)  IoQueueThreadIrp
	(uint32)PANIC(0x004E),                                    // 0x004E (78)  IoRemoveShareAccess
	(uint32)PANIC(0x004F),                                    // 0x004F (79)  IoSetIoCompletion
	(uint32)PANIC(0x0050),                                    // 0x0050 (80)  IoSetShareAccess
	(uint32)PANIC(0x0051),                                    // 0x0051 (81)  IoStartNextPacket
	(uint32)PANIC(0x0052),                                    // 0x0052 (82)  IoStartNextPacketByKey
	(uint32)PANIC(0x0053),                                    // 0x0053 (83)  IoStartPacket
	(uint32)PANIC(0x0054),                                    // 0x0054 (84)  IoSynchronousDeviceIoControlRequest
	(uint32)PANIC(0x0055),                                    // 0x0055 (85)  IoSynchronousFsdRequest
	(uint32)PANIC(0x0056),                                    // 0x0056 (86)  IofCallDriver
	(uint32)PANIC(0x0057),                                    // 0x0057 (87)  IofCompleteRequest
	(uint32)VARIABLE(&xboxkrnl::KdDebuggerEnabled),           // 0x0058 (88)
	(uint32)VARIABLE(&xboxkrnl::KdDebuggerNotPresent),        // 0x0059 (89)
	(uint32)FUNC(&xboxkrnl::IoDismountVolume),                // 0x005A (90)
	(uint32)FUNC(&xboxkrnl::IoDismountVolumeByName),          // 0x005B (91)
	(uint32)FUNC(&xboxkrnl::KeAlertResumeThread),             // 0x005C (92)
	(uint32)FUNC(&xboxkrnl::KeAlertThread),                   // 0x005D (93)
	(uint32)FUNC(&xboxkrnl::KeBoostPriorityThread),           // 0x005E (94)
	(uint32)FUNC(&xboxkrnl::KeBugCheck),                      // 0x005F (95)
	(uint32)FUNC(&xboxkrnl::KeBugCheckEx),                    // 0x0060 (96)
	(uint32)FUNC(&xboxkrnl::KeCancelTimer),                   // 0x0061 (97)
	(uint32)FUNC(&xboxkrnl::KeConnectInterrupt),              // 0x0062 (98)
	(uint32)FUNC(&xboxkrnl::KeDelayExecutionThread),          // 0x0063 (99)
	(uint32)FUNC(&xboxkrnl::KeDisconnectInterrupt),           // 0x0064 (100
	(uint32)PANIC(0x0065),                                    // 0x0065 (101) KeEnterCriticalRegion
	(uint32)VARIABLE(&xboxkrnl::MmGlobalData),                // 0x0066 (102)
	(uint32)PANIC(0x0067),                                    // 0x0067 (103) KeGetCurrentIrql
	(uint32)FUNC(&xboxkrnl::KeGetCurrentThread),              // 0x0068 (104)
	(uint32)PANIC(0x0069),                                    // 0x0069 (105) KeInitializeApc
	(uint32)PANIC(0x006A),                                    // 0x006A (106) KeInitializeDeviceQueue
	(uint32)FUNC(&xboxkrnl::KeInitializeDpc),                 // 0x006B (107)
	(uint32)PANIC(0x006C),                                    // 0x006C (108) KeInitializeEvent
	(uint32)FUNC(&xboxkrnl::KeInitializeInterrupt),           // 0x006D (109)
	(uint32)PANIC(0x006E),                                    // 0x006E (110) KeInitializeMutant
	(uint32)PANIC(0x006F),                                    // 0x006F (111) KeInitializeQueue	
	(uint32)PANIC(0x0070),                                    // 0x0070 (112) KeInitializeSemaphore
	(uint32)FUNC(&xboxkrnl::KeInitializeTimerEx),             // 0x0071 (113)
	(uint32)PANIC(0x0072),                                    // 0x0072 (114) KeInsertByKeyDeviceQueue
	(uint32)PANIC(0x0073),                                    // 0x0073 (115) KeInsertDeviceQueue
	(uint32)PANIC(0x0074),                                    // 0x0074 (116) KeInsertHeadQueue
	(uint32)PANIC(0x0075),                                    // 0x0075 (117) KeInsertQueue
	(uint32)PANIC(0x0076),                                    // 0x0076 (118) KeInsertQueueApc
	(uint32)FUNC(&xboxkrnl::KeInsertQueueDpc),                // 0x0077 (119)
	(uint32)VARIABLE(0x0078),                                 // 0x0078 (120) KeInterruptTime (Set by ConnectWindowsTimersToThunkTable)
	(uint32)PANIC(0x0079),                                    // 0x0079 (121) KeIsExecutingDpc
	(uint32)PANIC(0x007A),                                    // 0x007A (122) KeLeaveCriticalRegion
	(uint32)PANIC(0x007B),                                    // 0x007B (123) KePulseEvent
	(uint32)PANIC(0x007C),                                    // 0x007C (124) KeQueryBasePriorityThread
	(uint32)FUNC(&xboxkrnl::KeQueryInterruptTime),            // 0x007D (125)
	(uint32)FUNC(&xboxkrnl::KeQueryPerformanceCounter),       // 0x007E (126)
	(uint32)FUNC(&xboxkrnl::KeQueryPerformanceFrequency),     // 0x007F (127)
	(uint32)FUNC(&xboxkrnl::KeQuerySystemTime),               // 0x0080 (128)
	(uint32)FUNC(&xboxkrnl::KeRaiseIrqlToDpcLevel),           // 0x0081 (129)
	(uint32)PANIC(0x0082),                                    // 0x0082 (130) KeRaiseIrqlToSynchLevel
	(uint32)PANIC(0x0083),                                    // 0x0083 (131) KeRaiseIrqlToSynchLevel
	(uint32)PANIC(0x0084),                                    // 0x0084 (132) KeReleaseSemaphore
	(uint32)PANIC(0x0085),                                    // 0x0085 (133) KeRemoveByKeyDeviceQueue
	(uint32)PANIC(0x0086),                                    // 0x0086 (134) KeRemoveDeviceQueue
	(uint32)PANIC(0x0087),                                    // 0x0087 (135) KeRemoveEntryDeviceQueue
	(uint32)PANIC(0x0088),                                    // 0x0088 (136) KeRemoveQueue
	(uint32)PANIC(0x0089),                                    // 0x0089 (137) KeRemoveQueueDpc
	(uint32)PANIC(0x008A),                                    // 0x008A (138) KeResetEvent
	(uint32)PANIC(0x008B),                                    // 0x008B (139) KeRestoreFloatingPointState
	(uint32)PANIC(0x008C),                                    // 0x008C (140) KeResumeThread
	(uint32)PANIC(0x008D),                                    // 0x008D (141) KeRundownQueue
	(uint32)PANIC(0x008E),                                    // 0x008E (142) KeSaveFloatingPointState
	(uint32)FUNC(&xboxkrnl::KeSetBasePriorityThread),         // 0x008F (143)
	(uint32)PANIC(0x0090),                                    // 0x0090 (144) KeSetDisableBoostThread
	(uint32)PANIC(0x0091),                                    // 0x0091 (145) KeSetEvent
	(uint32)PANIC(0x0092),                                    // 0x0092 (146) KeSetEventBoostPriority
	(uint32)PANIC(0x0093),                                    // 0x0093 (147) KeSetPriorityProcess
	(uint32)FUNC(&xboxkrnl::KeSetPriorityThread),             // 0x0094 (148)
	(uint32)FUNC(&xboxkrnl::KeSetTimer),                      // 0x0095 (149)
	(uint32)FUNC(&xboxkrnl::KeSetTimerEx),                    // 0x0096 (150)
	(uint32)FUNC(&xboxkrnl::KeStallExecutionProcessor),       // 0x0097 (151)
	(uint32)PANIC(0x0098),                                    // 0x0098 (152) KeSuspendThread
	(uint32)PANIC(0x0099),                                    // 0x0099 (153) KeSynchronizeExecution
	(uint32)VARIABLE(0x009A),                                 // 0x009A (154) KeSystemTime (Set by ConnectWindowsTimersToThunkTable)
	(uint32)PANIC(0x009B),                                    // 0x009B (155) KeTestAlertThread
	(uint32)VARIABLE(&xboxkrnl::KeTickCount),                 // 0x009C (156)
	(uint32)VARIABLE(&xboxkrnl::KeTimeIncrement),             // 0x009D (157)
	(uint32)FUNC(&xboxkrnl::KeWaitForMultipleObjects),        // 0x009E (158)
	(uint32)FUNC(&xboxkrnl::KeWaitForSingleObject),           // 0x009F (159)
	(uint32)FUNC(&xboxkrnl::KfRaiseIrql),                     // 0x00A0 (160)
	(uint32)FUNC(&xboxkrnl::KfLowerIrql),                     // 0x00A1 (161)
	(uint32)VARIABLE(&xboxkrnl::KiBugCheckData),              // 0x00A2 (162)
	(uint32)FUNC(&xboxkrnl::KiUnlockDispatcherDatabase),      // 0x00A3 (163)
	(uint32)VARIABLE(&xboxkrnl::LaunchDataPage),              // 0x00A4 (164)
	(uint32)FUNC(&xboxkrnl::MmAllocateContiguousMemory),      // 0x00A5 (165)
	(uint32)FUNC(&xboxkrnl::MmAllocateContiguousMemoryEx),    // 0x00A6 (166)
	(uint32)FUNC(&xboxkrnl::MmAllocateSystemMemory),          // 0x00A7 (167)
	(uint32)FUNC(&xboxkrnl::MmClaimGpuInstanceMemory),        // 0x00A8 (168)
	(uint32)FUNC(&xboxkrnl::MmCreateKernelStack),             // 0x00A9 (169)
	(uint32)FUNC(&xboxkrnl::MmDeleteKernelStack),             // 0x00AA (170)
	(uint32)FUNC(&xboxkrnl::MmFreeContiguousMemory),          // 0x00AB (171)
	(uint32)FUNC(&xboxkrnl::MmFreeSystemMemory),              // 0x00AC (172)
	(uint32)FUNC(&xboxkrnl::MmGetPhysicalAddress),            // 0x00AD (173)
	(uint32)FUNC(&xboxkrnl::MmIsAddressValid),                // 0x00AE (174)
	(uint32)FUNC(&xboxkrnl::MmLockUnlockBufferPages),         // 0x00AF (175)
	(uint32)FUNC(&xboxkrnl::MmLockUnlockPhysicalPage),        // 0x00B0 (176)
	(uint32)FUNC(&xboxkrnl::MmMapIoSpace),                    // 0x00B1 (177)
	(uint32)FUNC(&xboxkrnl::MmPersistContiguousMemory),       // 0x00B2 (178)
	(uint32)FUNC(&xboxkrnl::MmQueryAddressProtect),           // 0x00B3 (179)
	(uint32)FUNC(&xboxkrnl::MmQueryAllocationSize),           // 0x00B4 (180)
	(uint32)FUNC(&xboxkrnl::MmQueryStatistics),               // 0x00B5 (181)
	(uint32)FUNC(&xboxkrnl::MmSetAddressProtect),             // 0x00B6 (182)
	(uint32)FUNC(&xboxkrnl::MmUnmapIoSpace),                  // 0x00B7 (183)
	(uint32)FUNC(&xboxkrnl::NtAllocateVirtualMemory),         // 0x00B8 (184)
	(uint32)PANIC(0x00B9),                                    // 0x00B9 (185) NtCancelTimer
	(uint32)FUNC(&xboxkrnl::NtClearEvent),                    // 0x00BA (186)
	(uint32)FUNC(&xboxkrnl::NtClose),                         // 0x00BB (187)
	(uint32)FUNC(&xboxkrnl::NtCreateDirectoryObject),         // 0x00BC (188)
	(uint32)FUNC(&xboxkrnl::NtCreateEvent),                   // 0x00BD (189)
	(uint32)FUNC(&xboxkrnl::NtCreateFile),                    // 0x00BE (190)
	(uint32)PANIC(0x00BF),                                    // 0x00BF (191) NtCreateIoCompletion
	(uint32)FUNC(&xboxkrnl::NtCreateMutant),                  // 0x00C0 (192)
	(uint32)FUNC(&xboxkrnl::NtCreateSemaphore),               // 0x00C1 (193)
	(uint32)PANIC(0x00C2),                                    // 0x00C2 (194) NtCreateTimer
	(uint32)PANIC(0x00C3),                                    // 0x00C3 (195) NtDeleteFile
	(uint32)FUNC(&xboxkrnl::NtDeviceIoControlFile),           // 0x00C4 (196)
	(uint32)FUNC(&xboxkrnl::NtDuplicateObject),               // 0x00C5 (197)
	(uint32)FUNC(&xboxkrnl::NtFlushBuffersFile),              // 0x00C6 (198)
	(uint32)FUNC(&xboxkrnl::NtFreeVirtualMemory),             // 0x00C7 (199)
	(uint32)PANIC(0x00C8),                                    // 0x00C8 (200) NtFsControlFile
	(uint32)FUNC(&xboxkrnl::NtOpenDirectoryObject),           // 0x00C9 (201)
	(uint32)FUNC(&xboxkrnl::NtOpenFile),                      // 0x00CA (202)
	(uint32)FUNC(&xboxkrnl::NtOpenSymbolicLinkObject),        // 0x00CB (203)
	(uint32)FUNC(&xboxkrnl::NtProtectVirtualMemory),          // 0x00CC (204)
	(uint32)FUNC(&xboxkrnl::NtPulseEvent),                    // 0x00CD (205)
	(uint32)FUNC(&xboxkrnl::NtQueueApcThread),                // 0x00CE (206)
	(uint32)FUNC(&xboxkrnl::NtQueryDirectoryFile),            // 0x00CF (207)
	(uint32)PANIC(0x00D0),                                    // 0x00D0 (208) NtQueryDirectoryObject
	(uint32)PANIC(0x00D1),                                    // 0x00D1 (209) NtQueryEvent
	(uint32)FUNC(&xboxkrnl::NtQueryFullAttributesFile),       // 0x00D2 (210)
	(uint32)FUNC(&xboxkrnl::NtQueryInformationFile),          // 0x00D3 (211)
	(uint32)PANIC(0x00D4),                                    // 0x00D4 (212) NtQueryIoCompletion
	(uint32)PANIC(0x00D5),                                    // 0x00D5 (213) NtQueryMutant
	(uint32)PANIC(0x00D6),                                    // 0x00D6 (214) NtQuerySemaphore
	(uint32)FUNC(&xboxkrnl::NtQuerySymbolicLinkObject),       // 0x00D7 (215)
	(uint32)PANIC(0x00D8),                                    // 0x00D8 (216) NtQueryTimer
	(uint32)FUNC(&xboxkrnl::NtQueryVirtualMemory),            // 0x00D9 (217)
	(uint32)FUNC(&xboxkrnl::NtQueryVolumeInformationFile),    // 0x00DA (218)
	(uint32)FUNC(&xboxkrnl::NtReadFile),                      // 0x00DB (219)
	(uint32)PANIC(0x00DC),                                    // 0x00DC (220) NtReadFileScatter
	(uint32)FUNC(&xboxkrnl::NtReleaseMutant),                 // 0x00DD (221)
	(uint32)FUNC(&xboxkrnl::NtReleaseSemaphore),              // 0x00DE (222)
	(uint32)PANIC(0x00DF),                                    // 0x00DF (223) NtRemoveIoCompletion
	(uint32)FUNC(&xboxkrnl::NtResumeThread),                  // 0x00E0 (224)
	(uint32)FUNC(&xboxkrnl::NtSetEvent),                      // 0x00E1 (225)
	(uint32)FUNC(&xboxkrnl::NtSetInformationFile),            // 0x00E2 (226)
	(uint32)PANIC(0x00E3),                                    // 0x00E3 (227) NtSetIoCompletion
	(uint32)FUNC(&xboxkrnl::NtSetSystemTime),                 // 0x00E4 (228)
	(uint32)PANIC(0x00E5),                                    // 0x00E5 (229) NtSetTimerEx
	(uint32)PANIC(0x00E6),                                    // 0x00E6 (230) NtSignalAndWaitForSingleObjectEx
	(uint32)FUNC(&xboxkrnl::NtSuspendThread),                 // 0x00E7 (231)
	(uint32)FUNC(&xboxkrnl::NtUserIoApcDispatcher),           // 0x00E8 (232)
	(uint32)FUNC(&xboxkrnl::NtWaitForSingleObject),           // 0x00E9 (233)
	(uint32)FUNC(&xboxkrnl::NtWaitForSingleObjectEx),         // 0x00EA (234)
	(uint32)FUNC(&xboxkrnl::NtWaitForMultipleObjectsEx),      // 0x00EB (235)
	(uint32)FUNC(&xboxkrnl::NtWriteFile),                     // 0x00EC (236)
	(uint32)PANIC(0x00ED),                                    // 0x00ED (237) NtWriteFileGather
	(uint32)FUNC(&xboxkrnl::NtYieldExecution),                // 0x00EE (238)
	(uint32)PANIC(0x00EF),                                    // 0x00EF (239) ObCreateObject
	(uint32)VARIABLE(&xboxkrnl::ObDirectoryObjectType),       // 0x00F0 (240)
	(uint32)PANIC(0x00F1),                                    // 0x00F1 (241) ObInsertObject
	(uint32)PANIC(0x00F2),                                    // 0x00F2 (242) ObMakeTemporaryObject 
	(uint32)FUNC(&xboxkrnl::ObOpenObjectByName),              // 0x00F3 (243)
	(uint32)PANIC(0x00F4),                                    // 0x00F4 (244) ObOpenObjectByPointer
	(uint32)VARIABLE(&xboxkrnl::ObpObjectHandleTable),        // 0x00F5 (245)
	(uint32)FUNC(&xboxkrnl::ObReferenceObjectByHandle),       // 0x00F6 (246)
	(uint32)PANIC(0x00F7),                                    // 0x00F7 (247) ObReferenceObjectByName
	(uint32)PANIC(0x00F8),                                    // 0x00F8 (248) ObReferenceObjectByPointer
	(uint32)VARIABLE(&xboxkrnl::ObSymbolicLinkObjectType),    // 0x00F9 (249)
	(uint32)FUNC(&xboxkrnl::ObfDereferenceObject),            // 0x00FA (250)
	(uint32)PANIC(0x00FB),                                    // 0x00FB (251) ObfReferenceObject
	(uint32)FUNC(&xboxkrnl::PhyGetLinkState),                 // 0x00FC (252)
	(uint32)FUNC(&xboxkrnl::PhyInitialize),                   // 0x00FD (253)
	(uint32)FUNC(&xboxkrnl::PsCreateSystemThread),            // 0x00FE (254)
	(uint32)FUNC(&xboxkrnl::PsCreateSystemThreadEx),          // 0x00FF (255)
	(uint32)PANIC(0x0100),                                    // 0x0100 (256) PsQueryStatistics
	(uint32)FUNC(&xboxkrnl::PsSetCreateThreadNotifyRoutine),  // 0x0101 (257)
	(uint32)FUNC(&xboxkrnl::PsTerminateSystemThread),         // 0x0102 (258)
	(uint32)VARIABLE(&xboxkrnl::PsThreadObjectType),          // 0x0103 (259)
	(uint32)FUNC(&xboxkrnl::RtlAnsiStringToUnicodeString),    // 0x0104 (260)
	(uint32)FUNC(&xboxkrnl::RtlAppendStringToString),         // 0x0105 (261)
	(uint32)FUNC(&xboxkrnl::RtlAppendUnicodeStringToString),  // 0x0106 (262)
	(uint32)FUNC(&xboxkrnl::RtlAppendUnicodeToString),        // 0x0107 (263)
	(uint32)FUNC(&xboxkrnl::RtlAssert),                       // 0x0108 (264)
	(uint32)PANIC(0x0109),                                    // 0x0109 (265) RtlCaptureContext
	(uint32)PANIC(0x010A),                                    // 0x010A (266) RtlCaptureStackBackTrace
	(uint32)FUNC(&xboxkrnl::RtlCharToInteger),                // 0x010B (267)
	(uint32)PANIC(0x010C),                                    // 0x010C (268) RtlCompareMemory
	(uint32)PANIC(0x010D),                                    // 0x010D (269) RtlCompareMemoryUlong
	(uint32)PANIC(0x010E),                                    // 0x010E (270) RtlCompareString
	(uint32)PANIC(0x010F),                                    // 0x010F (271) RtlCompareUnicodeString
	(uint32)PANIC(0x0110),                                    // 0x0110 (272) RtlCopyString
	(uint32)PANIC(0x0111),                                    // 0x0111 (273) RtlCopyUnicodeString
	(uint32)PANIC(0x0112),                                    // 0x0112 (274) RtlCreateUnicodeString
	(uint32)PANIC(0x0113),                                    // 0x0113 (275) RtlDowncaseUnicodeChar
	(uint32)PANIC(0x0114),                                    // 0x0114 (276) RtlDowncaseUnicodeString
	(uint32)FUNC(&xboxkrnl::RtlEnterCriticalSection),         // 0x0115 (277)
	(uint32)PANIC(0x0116),                                    // 0x0116 (278) RtlEnterCriticalSectionAndRegion
	(uint32)FUNC(&xboxkrnl::RtlEqualString),                  // 0x0117 (279)
	(uint32)PANIC(0x0118),                                    // 0x0118 (280) RtlEqualUnicodeString
	(uint32)PANIC(0x0119),                                    // 0x0119 (281) RtlExtendedIntegerMultiply
	(uint32)PANIC(0x011A),                                    // 0x011A (282) RtlExtendedLargeIntegerDivide
	(uint32)PANIC(0x011B),                                    // 0x011B (283) RtlExtendedMagicDivide
	(uint32)PANIC(0x011C),                                    // 0x011C (284) RtlFillMemory
	(uint32)PANIC(0x011D),                                    // 0x011D (285) RtlFillMemoryUlong
	(uint32)FUNC(&xboxkrnl::RtlFreeAnsiString),               // 0x011E (286)
	(uint32)PANIC(0x011F),                                    // 0x011F (287) RtlFreeUnicodeString
	(uint32)PANIC(0x0120),                                    // 0x0120 (288) RtlGetCallersAddress
	(uint32)FUNC(&xboxkrnl::RtlInitAnsiString),               // 0x0121 (289)
	(uint32)FUNC(&xboxkrnl::RtlInitUnicodeString),            // 0x0122 (290)
	(uint32)FUNC(&xboxkrnl::RtlInitializeCriticalSection),    // 0x0123 (291)
	(uint32)PANIC(0x0124),                                    // 0x0124 (292) RtlIntegerToChar
	(uint32)PANIC(0x0125),                                    // 0x0125 (293) RtlIntegerToUnicodeString
	(uint32)FUNC(&xboxkrnl::RtlLeaveCriticalSection),         // 0x0126 (294)
	(uint32)PANIC(0x0127),                                    // 0x0127 (295) RtlLeaveCriticalSectionAndRegion
	(uint32)FUNC(&xboxkrnl::RtlLowerChar),                    // 0x0128 (296)
	(uint32)PANIC(0x0129),                                    // 0x0129 (297) RtlMapGenericMask
	(uint32)PANIC(0x012A),                                    // 0x012A (298) RtlMoveMemory
	(uint32)PANIC(0x012B),                                    // 0x012B (299) RtlMultiByteToUnicodeN
	(uint32)PANIC(0x012C),                                    // 0x012C (300) RtlMultiByteToUnicodeSize
	(uint32)FUNC(&xboxkrnl::RtlNtStatusToDosError),           // 0x012D (301)
	(uint32)PANIC(0x012E),                                    // 0x012E (302) RtlRaiseException
	(uint32)PANIC(0x012F),                                    // 0x012F (303) RtlRaiseStatus
	(uint32)FUNC(&xboxkrnl::RtlTimeFieldsToTime),             // 0x0130 (304)
	(uint32)FUNC(&xboxkrnl::RtlTimeToTimeFields),             // 0x0131 (305)
	(uint32)FUNC(&xboxkrnl::RtlTryEnterCriticalSection),      // 0x0132 (306)
	(uint32)PANIC(0x0133),                                    // 0x0133 (307) RtlUlongByteSwap
	(uint32)FUNC(&xboxkrnl::RtlUnicodeStringToAnsiString),    // 0x0134 (308)
	(uint32)PANIC(0x0135),                                    // 0x0135 (309) RtlUnicodeStringToInteger
	(uint32)PANIC(0x0136),                                    // 0x0136 (310) RtlUnicodeToMultiByteN
	(uint32)PANIC(0x0137),                                    // 0x0137 (311) RtlUnicodeToMultiByteSize
	(uint32)PANIC(0x0138),                                    // 0x0138 (312) RtlUnwind
	(uint32)PANIC(0x0139),                                    // 0x0139 (313) RtlUpcaseUnicodeChar
	(uint32)PANIC(0x013A),                                    // 0x013A (314) RtlUpcaseUnicodeString
	(uint32)PANIC(0x013B),                                    // 0x013B (315) RtlUpcaseUnicodeToMultiByteN
	(uint32)PANIC(0x013C),                                    // 0x013C (316) RtlUpperChar
	(uint32)PANIC(0x013D),                                    // 0x013D (317) RtlUpperString
	(uint32)PANIC(0x013E),                                    // 0x013E (318) RtlUshortByteSwap
	(uint32)PANIC(0x013F),                                    // 0x013F (319) RtlWalkFrameChain
	(uint32)FUNC(&xboxkrnl::RtlZeroMemory),                   // 0x0140 (320)
	(uint32)VARIABLE(&xboxkrnl::XboxEEPROMKey),               // 0x0141 (321)
	(uint32)VARIABLE(&xboxkrnl::XboxHardwareInfo),            // 0x0142 (322)
	(uint32)VARIABLE(&xboxkrnl::XboxHDKey),                   // 0x0143 (323)
	(uint32)VARIABLE(&xboxkrnl::XboxKrnlVersion),             // 0x0144 (324)
	(uint32)VARIABLE(&xboxkrnl::XboxSignatureKey),            // 0x0145 (325)
	(uint32)VARIABLE(&xboxkrnl::XeImageFileName),             // 0x0146 (326)
	(uint32)FUNC(&xboxkrnl::XeLoadSection),                   // 0x0147 (327)
	(uint32)FUNC(&xboxkrnl::XeUnloadSection),                 // 0x0148 (328)
	(uint32)FUNC(&xboxkrnl::READ_PORT_BUFFER_UCHAR),          // 0x0149 (329)
	(uint32)FUNC(&xboxkrnl::READ_PORT_BUFFER_USHORT),         // 0x014A (330)
	(uint32)FUNC(&xboxkrnl::READ_PORT_BUFFER_ULONG),          // 0x014B (331)
	(uint32)FUNC(&xboxkrnl::WRITE_PORT_BUFFER_UCHAR),         // 0x014C (332)
	(uint32)FUNC(&xboxkrnl::WRITE_PORT_BUFFER_USHORT),        // 0x014D (333)
	(uint32)FUNC(&xboxkrnl::WRITE_PORT_BUFFER_ULONG),         // 0x014E (334)
	(uint32)FUNC(&xboxkrnl::XcSHAInit),                       // 0x014F (335)
	(uint32)FUNC(&xboxkrnl::XcSHAUpdate),                     // 0x0150 (336)
	(uint32)FUNC(&xboxkrnl::XcSHAFinal),                      // 0x0151 (337)
	(uint32)PANIC(0x0152),                                    // 0x0152 (338) XcRC4Key
	(uint32)PANIC(0x0153),                                    // 0x0153 (339) XcRC4Crypt
	(uint32)FUNC(&xboxkrnl::XcHMAC),                          // 0x0154 (340)
	(uint32)PANIC(0x0155),                                    // 0x0155 (341) XcPKEncPublic
	(uint32)PANIC(0x0156),                                    // 0x0156 (342) XcPKDecPrivate
	(uint32)PANIC(0x0157),                                    // 0x0157 (343) XcPKGetKeyLen
	(uint32)PANIC(0x0158),                                    // 0x0158 (344) XcVerifyPKCS1Signature
	(uint32)PANIC(0x0159),                                    // 0x0159 (345) XcModExp
	(uint32)PANIC(0x015A),                                    // 0x015A (346) XcDESKeyParity
	(uint32)PANIC(0x015B),                                    // 0x015B (347) XcKeyTable
	(uint32)PANIC(0x015C),                                    // 0x015C (348) XcBlockCrypt
	(uint32)PANIC(0x015D),                                    // 0x015D (349) XcBlockCryptCBC
	(uint32)PANIC(0x015E),                                    // 0x015E (350) XcCryptService
	(uint32)PANIC(0x015F),                                    // 0x015F (351) XcUpdateCrypto
	(uint32)PANIC(0x0160),                                    // 0x0160 (352) RtlRip
	(uint32)VARIABLE(&xboxkrnl::XboxLANKey),                  // 0x0161 (353)
	(uint32)VARIABLE(&xboxkrnl::XboxAlternateSignatureKeys),  // 0x0162 (354)
	(uint32)VARIABLE(&xboxkrnl::XePublicKeyData),             // 0x0163 (355)
	(uint32)VARIABLE(&xboxkrnl::HalBootSMCVideoMode),         // 0x0164 (356)
	(uint32)VARIABLE(&xboxkrnl::IdexChannelObject),           // 0x0165 (357)
	(uint32)FUNC(&xboxkrnl::HalIsResetOrShutdownPending),     // 0x0166 (358)
	(uint32)PANIC(0x0167),                                    // 0x0167 (359) IoMarkIrpMustComplete
	(uint32)FUNC(&xboxkrnl::HalInitiateShutdown),             // 0x0168 (360)
	(uint32)PANIC(0x0169),                                    // 0x0169 (361) KRNL(_snprintf)
	(uint32)PANIC(0x016A),                                    // 0x016A (362) KRNL(_sprintf)
	(uint32)PANIC(0x016B),                                    // 0x016B (363) KRNL(_vsnprintf)
	(uint32)PANIC(0x016C),                                    // 0x016C (364) KRNL(_vsprintf)
	(uint32)FUNC(&xboxkrnl::HalEnableSecureTrayEject),        // 0x016D (365)
	(uint32)FUNC(&xboxkrnl::HalWriteSMCScratchRegister),      // 0x016E (366)
	(uint32)FUNC(&xboxkrnl::UnknownAPI367),                   // 0x016F (367)
	(uint32)FUNC(&xboxkrnl::UnknownAPI368),                   // 0x0170 (368)
	(uint32)FUNC(&xboxkrnl::UnknownAPI369),                   // 0x0171 (369)
	(uint32)FUNC(&xboxkrnl::XProfpControl),                   // 0x0172 (370) PROFILING
	(uint32)FUNC(&xboxkrnl::XProfpGetData),                   // 0x0173 (371) PROFILING
	(uint32)FUNC(&xboxkrnl::IrtClientInitFast),               // 0x0174 (372) PROFILING
	(uint32)FUNC(&xboxkrnl::IrtSweep),                        // 0x0175 (373) PROFILING
	(uint32)PANIC(0x0176),                                    // 0x0177 (374) DEVKIT MmDbgAllocateMemory
	(uint32)PANIC(0x0177),                                    // 0x0178 (375) DEVKIT MmDbgFreeMemory - Returns number of pages released.
	(uint32)PANIC(0x0178),                                    // 0x0179 (376) DEVKIT MmDbgQueryAvailablePages
	(uint32)PANIC(0x0179),                                    // 0x017A (377) DEVKIT MmDbgReleaseAddress
	(uint32)PANIC(0x017A),                                    // 0x017A (378) DEVKIT MmDbgWriteCheck
};

/* prevent name collisions */
namespace NtDll
{
	#include "EmuNtDll.h"
};

// Virtual memory location of KUSER_SHARED_DATA :
// See http://research.microsoft.com/en-us/um/redmond/projects/invisible/src/base/md/i386/sim/_pertest2.c.htm
// and http://research.microsoft.com/en-us/um/redmond/projects/invisible/src/base/md/i386/sim/_glue.c.htm
// and http://processhacker.sourceforge.net/doc/ntexapi_8h_source.html
// and http://forum.sysinternals.com/0x7ffe0000-what-is-in-it_topic10012.html
#define MM_SHARED_USER_DATA_VA      0x7FFE0000
#define USER_SHARED_DATA ((NtDll::KUSER_SHARED_DATA * const)MM_SHARED_USER_DATA_VA)

// KUSER_SHARED_DATA Offsets
// See http://native-nt-toolkit.googlecode.com/svn/trunk/ndk/asm.h
// Note : KUSER_SHARED_DATA.TickCountLow seems deprecated
const UINT USER_SHARED_DATA_TICK_COUNT = 0x320;

// Here we define the addresses of the native Windows timers :
// Source: Dxbx
const xboxkrnl::PKSYSTEM_TIME CxbxNtTickCount = (xboxkrnl::PKSYSTEM_TIME)(MM_SHARED_USER_DATA_VA + USER_SHARED_DATA_TICK_COUNT);

void ConnectWindowsTimersToThunkTable()
{
	// Couple the xbox thunks for xboxkrnl::KeInterruptTime and xboxkrnl::KeSystemTime
	// to their actual counterparts on Windows, this way we won't have to spend any
	// time on updating them ourselves, and still get highly accurate timers!
	// See http://www.dcl.hpi.uni-potsdam.de/research/WRK/2007/08/getting-os-information-the-kuser_shared_data-structure/

	// Point Xbox KeInterruptTime to host InterruptTime:
	CxbxKrnl_KernelThunkTable[120] = (uint32)&(USER_SHARED_DATA->InterruptTime);

	// Point Xbox KeSystemTime to host SystemTime; If read directly (thus skipping
	// KeQuerySystemTime), this value is not adjusted with HostSystemTimeDelta!
	CxbxKrnl_KernelThunkTable[154] = (uint32)&(USER_SHARED_DATA->SystemTime);

	// Note that we can't do the same for TickCount, as that timer
	// updates slower on the xbox. See EmuThreadUpdateTickCount().
}
