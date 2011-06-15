// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Emu.cpp
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

#include "CxbxKrnl.h"
#define COMPILE_MULTIMON_STUBS
#include "Emu.h"
#include "EmuFS.h"
#include "EmuAlloc.h"

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"
#include "EmuShared.h"
#include "HLEDataBase.h"
#include "HLEIntercept.h"

#ifdef _DEBUG
#include <Dbghelp.h>
CRITICAL_SECTION dbgCritical;
#endif

// Global Variable(s)
HANDLE           g_hCurDir    = NULL;
CHAR            *g_strCurDrive= NULL;
HANDLE           g_hTDrive    = NULL;
CHAR            *g_strTDrive  = NULL;
HANDLE           g_hUDrive    = NULL;
CHAR            *g_strUDrive  = NULL;
HANDLE           g_hZDrive    = NULL;
CHAR            *g_strZDrive  = NULL;
volatile bool    g_bEmuException = false;
volatile bool    g_bEmuSuspended = false;
volatile bool    g_bPrintfOn = true;

// global exception patching address
uint32 g_HaloHack[4] = {0};

// Dead to Rights hack
uint32 g_DeadToRightsHack[2] = {0};

// Static Function(s)
static int ExitException(LPEXCEPTION_POINTERS e);

// Dll entry point, exit point, ...
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    static HINSTANCE hInitInstance = NULL;

    if(fdwReason == DLL_PROCESS_ATTACH)
    {
#ifdef _DEBUG
        InitializeCriticalSection(&dbgCritical);
#endif

        EmuShared::Init();
        hInitInstance = hinstDLL;
    }

    if(fdwReason == DLL_PROCESS_DETACH)
    {
        if(hInitInstance == hinstDLL)
            EmuShared::Cleanup();

#ifdef _DEBUG
        DeleteCriticalSection(&dbgCritical);
#endif
    }

    return TRUE;
}

// print out a warning message to the kernel debug log file
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...)
{
    if(szWarningMessage == NULL)
        return;

    char szBuffer1[255];
    char szBuffer2[255];

    va_list argp;

    sprintf(szBuffer1, "EmuWarn (0x%X): ", GetCurrentThreadId());

    va_start(argp, szWarningMessage);

    vsprintf(szBuffer2, szWarningMessage, argp);

    va_end(argp);

    strcat(szBuffer1, szBuffer2);

    if(g_bPrintfOn)
    {
        printf("%s\n", szBuffer1);
    }

    fflush(stdout);

    return;
}
#endif

// exception handler
extern int EmuException(LPEXCEPTION_POINTERS e)
{
    if(EmuIsXboxFS())
        EmuSwapFS();

    g_bEmuException = true;

    // check for Halo hack
    {
        if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
        {
            // Halo Access Adjust 1
            if(e->ContextRecord->Eip == 0x0003394C || e->ContextRecord->Eip == 0x0003387C)
            {
				// blueshogun96 4/10/2010
				// Quite frankly, I don't know how Caustik came up with this hack.
				// A similar situation comes up prior to going ingame almost immediately
				// after the menus.  The hack wasn't applied because the register values
				// were different, but the situation looks the same.  Also, I don't know
				// where the 0x803A6000 came from in the first hack.  Is this accomodating
				// for size??  Or does this need to stay constant.  More information is
				// welcome!

                if(e->ContextRecord->Ecx == 0x803BD800 || e->ContextRecord->Ecx == 0x803E853C || 
				   e->ContextRecord->Ecx == 0x813B5018)
                {
                    // Halo BINK skip
                    {
                        // nop sled over bink calls
                        /*
                        memset((void*)0x2CBA4, 0x90, 0x2CBAF - 0x2CBA4);
                        memset((void*)0x2CBBD, 0x90, 0x2CBD5 - 0x2CBBD);
                        //*/
                        memset((void*)0x2CAE0, 0x90, 0x2CE1E - 0x2CAE0);
                    }

                    uint32 fix = g_HaloHack[1] + (e->ContextRecord->Eax - 0x803A6000);

                    e->ContextRecord->Eax = e->ContextRecord->Ecx = fix;

                    *(uint32*)e->ContextRecord->Esp = fix;

                    ((XTL::X_D3DResource*)fix)->Data = g_HaloHack[1] + (((XTL::X_D3DResource*)fix)->Data - 0x803A6000);

                    // go through and fix any other pointers in the ESI allocation chunk
                    {
                        DWORD dwESI = e->ContextRecord->Esi;
                        DWORD dwSize = EmuCheckAllocationSize((PVOID)dwESI, false);

                        // dword aligned
                        dwSize -= 4 - dwSize%4;

                        for(DWORD v=0;v<dwSize;v+=4)
                        {
                            DWORD dwCur = *(DWORD*)(dwESI+v);

                            if(dwCur >= 0x803A6000 && dwCur < 0x819A6000)
                                *(DWORD*)(dwESI+v) = g_HaloHack[1] + (dwCur - 0x803A6000);
                        }
                    }

                    // fix this global pointer
                    {
                        DWORD dwValue = *(DWORD*)0x39CE24;

                        *(DWORD*)0x39CE24 = g_HaloHack[1] + (dwValue - 0x803A6000);
                    }

                    DbgPrintf("EmuMain (0x%X): Halo Access Adjust 1 was applied!\n", GetCurrentThreadId());

                    g_bEmuException = false;

                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
            // Halo Access Adjust 2
            else if(e->ContextRecord->Eip == 0x00058D8C)
            {
                if(e->ContextRecord->Eax == 0x819A5818)
                {
                    uint32 fix = g_HaloHack[1] + (e->ContextRecord->Eax - 0x803A6000);

                    *(DWORD*)0x0039BE58 = e->ContextRecord->Eax = fix;

                    // go through and fix any other pointers in the 0x2DF1C8 allocation chunk
                    {
                        DWORD dwPtr = *(DWORD*)0x2DF1C8;
                        DWORD dwSize = EmuCheckAllocationSize((PVOID)dwPtr, false);

                        // dword aligned
                        dwSize -= 4 - dwSize%4;

                        for(DWORD v=0;v<dwSize;v+=4)
                        {
                            DWORD dwCur = *(DWORD*)(dwPtr+v);

                            if(dwCur >= 0x803A6000 && dwCur < 0x819A6000)
                                *(DWORD*)(dwPtr+v) = g_HaloHack[1] + (dwCur - 0x803A6000);
                        }
                    }

                    DbgPrintf("EmuMain (0x%X): Halo Access Adjust 2 was applied!\n", GetCurrentThreadId());

                    g_bEmuException = false;

                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }
    }

    // check for Battlestar Galactica hack *PAL Version*
    {
        if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
        {
            // Battlestar Galactica Hack 1
            if(e->ContextRecord->Eip == 0x000CB580 || e->ContextRecord->Eip == 0x000CB340)
            {
                //if(e->ContextRecord->Ecx == 0x00000200 || e->ContextRecord->Ecx == 0x00000100)
                //{
                    // Battlestar Galactica WBINVD skip
                    e->ContextRecord->Eip += 2;

                    DbgPrintf("EmuMain (0x%X): Battlestar Galactica Hack 1 was applied!\n", GetCurrentThreadId());

                    g_bEmuException = false;

                    return EXCEPTION_CONTINUE_EXECUTION;
                //}
            }
        }
    }

	// Smashing drive *NTSC* 
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0xA41F5)
		{
			// Skip call to D3D::SwapStart(class D3D::CDevice *)
			memset( (void*) 0xA41F0, 0x90, 0x10 );
			DbgPrintf("EmuMain (0x%X): Smashing drive hack 1 applied!\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	// House of the Dead 3 *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0xB961E)
		{
			e->ContextRecord->Eip += 2;

			DbgPrintf("EmuMain (0x%X): House of the Dead hack 1 applied!\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if(e->ContextRecord->Eip == 0x1197F4)
		{
			e->ContextRecord->Eip += 2;

			DbgPrintf("EmuMain (0x%X): House of the Dead hack 2 applied!\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	// Zapper *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		if(e->ContextRecord->Eip == 0xE2250)
		{
			// Zapper WBINVD skip
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Zapper Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	// Fusion Frenzy (Demo) *NTSC*
	// The full version probably has the same problem.  If so, add the address here :)
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		if(e->ContextRecord->Eip == 0x4ED70)
		{
			// WBINVD skip
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Fusion Frenzy hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	// Taz: Wanted *NTSC*
	// Okay this is getting rediculous!
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		if(e->ContextRecord->Eip == 0x151090)
		{
			// WBINVD skip
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Taz Wanted Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	// Phantom Crash *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0xB2EF9)
		{
			// fstp dword ptr [eax]
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Phantom Crash hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }

		if(e->ContextRecord->Eip == 0xB2EFB || e->ContextRecord->Eip == (0xB2EFB + 3) ||
		   e->ContextRecord->Eip == (0xB2EFB + 6) || e->ContextRecord->Eip == (0xB2EFB + 9))
		{
			// mov [eax-0xX], edi
            e->ContextRecord->Eip += 3;

            DbgPrintf("EmuMain (0x%X): Phantom Crash hack 2 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	// Blood Rayne *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x12707D)
		{
			// rep movsd
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Blood Rayne hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
	else if(e->ExceptionRecord->ExceptionCode == 0x80000003)
    {
		if(e->ContextRecord->Eip == 0xDF138)
		{
			// int 3
			e->ContextRecord->Eip++;

			DbgPrintf("EmuMain (0x%X): Blood Rayne hack 2 was applied!\n", GetCurrentThreadId());
			
            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	// JetSetRadio Future *NTSC*
	/*if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x48226)
		{
			// mov edx, [eax]
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): JetSetRadio Future hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }

		if(e->ContextRecord->Eip == 0x48233)
		{
			// call dword ptr [edx+0x10]
            e->ContextRecord->Eip += 3;

            DbgPrintf("EmuMain (0x%X): JetSetRadio Future hack 2 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
		if(e->ContextRecord->Eip == 0x47448)
		{
			// mov edx, [ecx]
			// push ecx
			// call dword ptr [edx+0x0C]
            e->ContextRecord->Eip += 6;

            DbgPrintf("EmuMain (0x%X): JetSetRadio Future hack 3 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }

		//if(e->ContextRecord->Eip == 0x48233)
		//{
		//	// call dword ptr [edx+0x10]
  //          e->ContextRecord->Eip += 3;

  //          DbgPrintf("EmuMain (0x%X): JetSetRadio Future hack 2 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //      }
    }*/

	// Unreal Championship *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		if(e->ContextRecord->Eip == 0x4DD40 || e->ContextRecord->Eip == 0x4E7D0)
		{
			// WBINVD skip
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Unreal Championship Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}
	else if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x6E94B)
		{
			// mov ecx, [eax+4]
			e->ContextRecord->Eip += 3;

            DbgPrintf("EmuMain (0x%X): Unreal Championship Hack 2 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
		if(e->ContextRecord->Eip == 0x6E954)
		{
			// mov [eax+4], ecx
			e->ContextRecord->Eip += 3;

            DbgPrintf("EmuMain (0x%X): Unreal Championship Hack 3 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	// Frogger Beyond *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x19F9D0)
		{
			// 
            e->ContextRecord->Eip += 3;

            DbgPrintf("EmuMain (0x%X): Frogger Beyond Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}

	// Rayman Arena *NTSC*
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		//if(e->ContextRecord->Eip == 0x18B40C)
		//{
		//	// call dword ptr [ecx+4]
  //          e->ContextRecord->Eip += 3;

  //          DbgPrintf("EmuMain (0x%X): Rayman Arena Hack 1 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //      }
		if(e->ContextRecord->Eip == 0x1B8F978)
		{
			// db ff (may be driver specific problem!)
			e->ContextRecord->Eip++;
			 
			g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	// Dead to Rights (Region shouldn't matter)
	/*if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(g_DeadToRightsHack[0])
		{
			DbgPrintf("EmuMain (0x%X): Dead to Rights Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}*/

	/* Metal Gear Solid II (NTSC) */
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		if(e->ContextRecord->Eip == 0x4E2A0D)
		{
			// WBINVD skip
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Metal Gear Solid II Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}

	// Soul Calibur II
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		if(e->ContextRecord->Eip == 0x25452)
		{
			// WBINVD skip
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Soul Calibur II Hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}

	/* Munch Trial(NTSC)*/
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x254D96)
		{
			// rep movsd
            e->ContextRecord->Eip += 2;

            DbgPrintf("EmuMain (0x%X): Munch Trial hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}

	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x346B4)
		{
			// cmp dword ptr [eax+0x208], 0
            e->ContextRecord->Eip += 7;

            DbgPrintf("EmuMain (0x%X): Xbox Dashboard (3944) hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}

	// Splinter Cell
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if(e->ContextRecord->Eip == 0x1421A)
		{
			// mov ecx, [eax+4]
            e->ContextRecord->Eip += 3;

            DbgPrintf("EmuMain (0x%X): Splinter Cell hack 1 was applied!\n", GetCurrentThreadId());

            g_bEmuException = false;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
	}

	// Metal Slug 3
	if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		//if(e->ContextRecord->Eip == 0x1B59BC)
		//{
		//	// mov [ecx+0x28], eax
  //          e->ContextRecord->Eip += 3;

  //          DbgPrintf("EmuMain (0x%X): Metal Slug 3 hack 1 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //      }

		//if(e->ContextRecord->Eip == 0x1B59CA)
		//{
		//	// mov [ecx+0x28], eax
  //          e->ContextRecord->Eip += 3;

  //          DbgPrintf("EmuMain (0x%X): Metal Slug 3 hack 2 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //      }

		//if(e->ContextRecord->Eip == 0x1B59DE)
		//{
		//	// mov [ecx+0x28], eax
  //          e->ContextRecord->Eip += 3;

  //          DbgPrintf("EmuMain (0x%X): Metal Slug 3 hack 3 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //      }

		//if(e->ContextRecord->Eip == 0x1B59EC)
		//{
		//	// mov [ecx+0x28], eax
  //          e->ContextRecord->Eip += 3;

  //          DbgPrintf("EmuMain (0x%X): Metal Slug 3 hack 4 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //  `    }

		//if(e->ContextRecord->Eip == 0x1B59BC)
		//{
		//	// mov [ecx+0x28], eax
  //          e->ContextRecord->Eip += 7;

  //          DbgPrintf("EmuMain (0x%X): Metal Slug 3 hack 1 was applied!\n", GetCurrentThreadId());

  //          g_bEmuException = false;

  //          return EXCEPTION_CONTINUE_EXECUTION;
  //      }
	}

	// Privileged instruction check
	if(e->ExceptionRecord->ExceptionCode == 0xC0000096)
	{
		DWORD dwEip = e->ContextRecord->Eip;

		// WRMSR
		if(*((BYTE*)dwEip) == 0x0F && *((BYTE*)dwEip+1) == 0x30)
		{
			e->ContextRecord->Eip += 2;

			DbgPrintf("EmuMain (0x%X): Skipping privileged instruction (WRMSR)\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		// RDMSR
		if(*((BYTE*)dwEip) == 0x0F && *((BYTE*)dwEip+1) == 0x32)
		{
			e->ContextRecord->Eip += 2;

			DbgPrintf("EmuMain (0x%X): Skipping privileged instruction (RDMSR)\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		// RDPMC
		if(*((BYTE*)dwEip) == 0x0F && *((BYTE*)dwEip+1) == 0x33)
		{
			e->ContextRecord->Eip += 2;

			DbgPrintf("EmuMain (0x%X): Skipping privileged instruction (RDPMC)\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		// WBINVD
		if(*((BYTE*)dwEip) == 0x0F && *((BYTE*)dwEip+1) == 0x09)
		{
			e->ContextRecord->Eip += 2;

			DbgPrintf("EmuMain (0x%X): Skipping privileged instruction (WBINVD)\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		// CLI
		if(*((BYTE*)dwEip) == 0xFA)
		{
			e->ContextRecord->Eip += 1;

			DbgPrintf("EmuMain (0x%X): Skipping privileged instruction (CLI)\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		// STI
		if(*((BYTE*)dwEip) == 0xFB)
		{
			e->ContextRecord->Eip += 1;

			DbgPrintf("EmuMain (0x%X): Skipping privileged instruction (STI)\n", GetCurrentThreadId());

			g_bEmuException = false;

			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}


    // print debug information
    {
        if(e->ExceptionRecord->ExceptionCode == 0x80000003)
            printf("EmuMain (0x%X): Recieved Breakpoint Exception (int 3)\n", GetCurrentThreadId());
        else
            printf("EmuMain (0x%X): Recieved Exception (Code := 0x%.08X)\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode);

        printf("\n"
            " EIP := 0x%.08X EFL := 0x%.08X\n"
            " EAX := 0x%.08X EBX := 0x%.08X ECX := 0x%.08X EDX := 0x%.08X\n"
            " ESI := 0x%.08X EDI := 0x%.08X ESP := 0x%.08X EBP := 0x%.08X\n"
            "\n",
            e->ContextRecord->Eip, e->ContextRecord->EFlags,
            e->ContextRecord->Eax, e->ContextRecord->Ebx, e->ContextRecord->Ecx, e->ContextRecord->Edx,
            e->ContextRecord->Esi, e->ContextRecord->Edi, e->ContextRecord->Esp, e->ContextRecord->Ebp);

#ifdef _DEBUG
        CONTEXT Context = *(e->ContextRecord);
        EmuPrintStackTrace(&Context);
#endif
    }

    fflush(stdout);

    // notify user
    {
        char buffer[256];

        if(e->ExceptionRecord->ExceptionCode == 0x80000003)
        {
            sprintf(buffer,
                "Recieved Breakpoint Exception (int 3) @ EIP := 0x%.08X\n"
                "\n"
                "  Press Abort to terminate emulation.\n"
                "  Press Retry to debug.\n"
                "  Press Ignore to continue emulation.",
                e->ContextRecord->Eip, e->ContextRecord->EFlags);

            e->ContextRecord->Eip += 1;

            int ret = MessageBox(g_hEmuWindow, buffer, "Cxbx", MB_ICONSTOP | MB_ABORTRETRYIGNORE);

            if(ret == IDABORT)
            {
                printf("EmuMain (0x%X): Aborting Emulation\n", GetCurrentThreadId());
                fflush(stdout);

                if(CxbxKrnl_hEmuParent != NULL)
                    SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

                ExitProcess(1);
            }
            else if(ret == IDIGNORE)
            {
                printf("EmuMain (0x%X): Ignored Breakpoint Exception\n", GetCurrentThreadId());

                g_bEmuException = false;

                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
        else
        {
            sprintf(buffer,
                "Recieved Exception Code 0x%.08X @ EIP := 0x%.08X\n"
                "\n"
                "  Press \"OK\" to terminate emulation.\n"
                "  Press \"Cancel\" to debug.",
                e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip, e->ContextRecord->EFlags);

            if(MessageBox(g_hEmuWindow, buffer, "Cxbx", MB_ICONSTOP | MB_OKCANCEL) == IDOK)
            {
                printf("EmuMain (0x%X): Aborting Emulation\n", GetCurrentThreadId());
                fflush(stdout);

                if(CxbxKrnl_hEmuParent != NULL)
                    SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

                ExitProcess(1);
            }
        }
    }

    g_bEmuException = false;

    return EXCEPTION_CONTINUE_SEARCH;
}

// check how many bytes were allocated for a structure
extern int EmuCheckAllocationSize(LPVOID pBase, bool largeBound)
{
    MEMORY_BASIC_INFORMATION MemoryBasicInfo;

    DWORD dwRet;
#ifdef _DEBUG_ALLOC
    dwRet = CxbxVirtualQueryDebug(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));
    if (dwRet == -1)
#endif
    dwRet = VirtualQuery(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));

    if(dwRet == 0)
        return 0;

    if(MemoryBasicInfo.State != MEM_COMMIT)
        return 0;

    // this is a hack in order to determine when pointers come from a large write-combined database
    if(largeBound && MemoryBasicInfo.RegionSize > 5*1024*1024)
        return -1;

    return MemoryBasicInfo.RegionSize - ((DWORD)pBase - (DWORD)MemoryBasicInfo.BaseAddress);
}

// exception handle for that tough final exit :)
int ExitException(LPEXCEPTION_POINTERS e)
{
    if(EmuIsXboxFS())
        EmuSwapFS();

    static int count = 0;

    // debug information
    printf("EmuMain (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
    printf("EmuMain (0x%X): Recieved Exception [0x%.08X]@0x%.08X\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);
    printf("EmuMain (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());

    fflush(stdout);

    MessageBox(g_hEmuWindow, "Warning: Could not safely terminate process!", "Cxbx", MB_OK);

    count++;

    if(count > 1)
    {
        MessageBox(g_hEmuWindow, "Warning: Multiple Problems!", "Cxbx", MB_OK);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    if(CxbxKrnl_hEmuParent != NULL)
        SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

    ExitProcess(1);

    return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef _DEBUG
// print call stack trace
void EmuPrintStackTrace(PCONTEXT ContextRecord)
{
    static int const STACK_MAX     = 16;
    static int const SYMBOL_MAXLEN = 64;

    EnterCriticalSection(&dbgCritical);

    IMAGEHLP_MODULE64 module = { sizeof(IMAGEHLP_MODULE) };

    BOOL fSymInitialized;
    fSymInitialized = SymInitialize(GetCurrentProcess(), NULL, TRUE);

    STACKFRAME64 frame = { sizeof(STACKFRAME64) };
    frame.AddrPC.Offset    = ContextRecord->Eip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = ContextRecord->Ebp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = ContextRecord->Esp;
    frame.AddrStack.Mode   = AddrModeFlat;

    for(int i = 0; i < STACK_MAX; i++)
    {
        if(!StackWalk64(
            IMAGE_FILE_MACHINE_I386,
            GetCurrentProcess(),
            GetCurrentThread(),
            &frame,
            ContextRecord,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
            break;

        DWORD64 dwDisplacement = 0;
        PSYMBOL_INFO pSymbol = 0;
        BYTE symbol[sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN];

        SymGetModuleInfo64(GetCurrentProcess(), frame.AddrPC.Offset, &module);

        if(fSymInitialized)
        {
            pSymbol = (PSYMBOL_INFO)symbol;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN - 1;
            pSymbol->MaxNameLen = SYMBOL_MAXLEN;

            if(!SymFromAddr(GetCurrentProcess(), frame.AddrPC.Offset, &dwDisplacement, pSymbol))
                pSymbol = 0;
        }

        if(module.ModuleName)
            printf(" %2d: %-8s 0x%.08X", i, module.ModuleName, frame.AddrPC.Offset);
        else
            printf(" %2d: %8c 0x%.08X", i, ' ', frame.AddrPC.Offset);

        if(pSymbol)
        {
            printf(" %s+0x%.04X\n", pSymbol->Name, dwDisplacement);
        }
        else
            printf("\n");
    }
    printf("\n");

    if(fSymInitialized)
        SymCleanup(GetCurrentProcess());

    LeaveCriticalSection(&dbgCritical);
}
#endif
