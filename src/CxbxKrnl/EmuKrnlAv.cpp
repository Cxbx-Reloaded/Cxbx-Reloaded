// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlAv.cpp
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For AvGetSavedDataAddress, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "MemoryManager.h"

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

#include "Emu.h" // For EmuWarning()
#include "EmuAlloc.h" // For CxbxFree(), g_MemoryManager.Allocate(), etc.

// Global Variable(s)
PVOID g_pPersistedData = NULL;

ULONG AvQueryAvCapabilities()
{
	ULONG Type;
	ULONG Result;
	ULONG ResultLength;

	// TODO: For the AV Pack, read SMC_COMMAND_VIDEO_MODE (or HalBootSMCVideoMode) and convert it to a AV_PACK_*
	ULONG Capabilities = AV_PACK_HDTV;

	// Read the AV Region
	NTSTATUS status = xboxkrnl::ExQueryNonVolatileSetting(
		xboxkrnl::XC_FACTORY_AV_REGION, 
		&Type,
		&Result,
		sizeof(Result),
		&ResultLength);

	// If the value was not set, default to NTSC
	if (status != STATUS_SUCCESS || ResultLength != sizeof(Result)) {
		Result = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
	}

	Capabilities |= Result & (AV_STANDARD_MASK | AV_REFRESH_MASK);

	// Read the User's configuration (Set in the Dashboard)
	status = xboxkrnl::ExQueryNonVolatileSetting(xboxkrnl::XC_VIDEO,
		&Type,
		&Result,
		sizeof(Result),
		&ResultLength);

	// If no result, default to no options selected
	if (status != STATUS_SUCCESS || ResultLength != sizeof(Result))	{
		Result = 0;
	}

	return Capabilities | Result & ~(AV_STANDARD_MASK | AV_PACK_MASK);
}

// ******************************************************************
// * 0x0001 - AvGetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(1) xboxkrnl::PVOID NTAPI xboxkrnl::AvGetSavedDataAddress(void)
{
	LOG_FUNC();

	__asm int 3;

	// Allocate a buffer the size of the screen buffer and return that.
	// TODO: Fill this buffer with the contents of the front buffer.
	// TODO: This isn't always the size we need...

	if (g_pPersistedData)
	{
		g_MemoryManager.Free(g_pPersistedData);
		g_pPersistedData = NULL;
	}

	g_pPersistedData = g_MemoryManager.Allocate(640 * 480 * 4);

#if 0
	// Get a copy of the front buffer
	IDirect3DSurface8* pFrontBuffer = NULL;

	if (SUCCEEDED(g_pD3DDevice8->GetFrontBuffer(pFrontBuffer)))
	{
		D3DLOCKED_RECT LockedRect;
		pFrontBuffer->LockRect(0, NULL, &LockedRect);

		CopyMemory(g_pPersistedData, LockRect.pBits, 640 * 480 * 4);

		pFrontBuffer->UnlockRect();
	}
#endif

	// TODO: We might want to return something sometime...
	/*if( !g_pPersistedData )
	{
	FILE* fp = fopen( "PersistedSurface.bin", "rb" );
	fseek( fp, 0, SEEK_END );
	long size = ftell( fp );
	g_pPersistedData = g_MemoryManager.Allocate( size );
	fread( g_pPersistedData, size, 1, fp );
	fclose(fp);
	}*/

	RETURN (NULL); //g_pPersistedData;
}

// ******************************************************************
// * 0x0002 - AvSendTVEncoderOption()
// ******************************************************************
XBSYSAPI EXPORTNUM(2) xboxkrnl::VOID NTAPI xboxkrnl::AvSendTVEncoderOption
(
	IN  PVOID   RegisterBase,
	IN  ULONG   Option,
	IN  ULONG   Param,
	OUT ULONG   *Result
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(RegisterBase)
		LOG_FUNC_ARG(Option)
		LOG_FUNC_ARG(Param)
		LOG_FUNC_ARG_OUT(Result)
		LOG_FUNC_END;

	//if (RegisterBase == NULL)
	//	RegisterBase = (void *)NV20_REG_BASE_KERNEL;

	switch (Option) {
	case AV_OPTION_MACROVISION_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ENABLE_CC:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_DISABLE_CC:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_SEND_CC_DATA:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_CC_STATUS:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_AV_CAPABILITIES:
		*Result = AvQueryAvCapabilities();
		break;
	case AV_OPTION_BLANK_SCREEN:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_MACROVISION_COMMIT:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_FLICKER_FILTER:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ZERO_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_QUERY_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ENABLE_LUMA_FILTER:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_GUESS_FIELD:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_ENCODER_TYPE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_MODE_TABLE_VERSION:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_CGMS:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_WIDESCREEN:
		LOG_UNIMPLEMENTED();
		break;
	default:
		// do nothing
		break;
	}
}

// ******************************************************************
// * 0x0003 - AvSetDisplayMode()
// ******************************************************************
XBSYSAPI EXPORTNUM(3) xboxkrnl::ULONG NTAPI xboxkrnl::AvSetDisplayMode
(
	IN  PVOID   RegisterBase,
	IN  ULONG   Step,
	IN  ULONG   Mode,
	IN  ULONG   Format,
	IN  ULONG   Pitch,
	IN  ULONG   FrameBuffer
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(RegisterBase)
		LOG_FUNC_ARG(Step)
		LOG_FUNC_ARG(Mode)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_ARG(Pitch)
		LOG_FUNC_ARG(FrameBuffer)
		LOG_FUNC_END;

	ULONG result = S_OK;

	LOG_UNIMPLEMENTED();

	RETURN(result);
}

// ******************************************************************
// * 0x0004 - AvSetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(4) xboxkrnl::VOID NTAPI xboxkrnl::AvSetSavedDataAddress
(
	IN  PVOID   Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}