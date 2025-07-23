// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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

#define LOG_PREFIX CXBXR_MODULE::AV


#include <core\kernel\exports\xboxkrnl.h> // For AvGetSavedDataAddress, etc.
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "core\kernel\support\EmuNtDll.h"
};

#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "core\hle\D3D8\Direct3D9\Direct3D9.h"
#include "devices\x86\EmuX86.h"

#include "EmuKrnlAvModes.h"
#include "devices\Xbox.h" // For g_NV2A
#include "devices\video\nv2a_int.h"
#include "devices\video\nv2a.h" // For NV2ABlockInfo, EmuNV2A_Block()


// HW Register helper functions
xbox::uchar_xt REG_RD08(void* Ptr, xbox::ulong_xt Addr)
{
	return EmuX86_Read((xbox::addr_xt)Ptr + Addr, sizeof(uint8_t));
}

void REG_WR08(void* Ptr, xbox::ulong_xt Addr, xbox::uchar_xt Val)
{
	EmuX86_Write((xbox::addr_xt)Ptr + Addr, Val, sizeof(uint8_t));
}

xbox::ulong_xt REG_RD32(void* Ptr, xbox::ulong_xt Addr)
{
	return EmuX86_Read((xbox::addr_xt)Ptr + Addr, sizeof(uint32_t));
}

void REG_WR32(void* Ptr, xbox::ulong_xt Addr, xbox::ulong_xt Val)
{
	EmuX86_Write((xbox::addr_xt)Ptr + Addr, Val, sizeof(uint32_t));
}

void CRTC_WR(void* Ptr, xbox::uchar_xt i, xbox::uchar_xt d)
{
	static const NV2ABlockInfo* block = EmuNV2A_Block(NV_PRMCIO_CRX__COLOR);

	g_NV2A->BlockWrite(block, NV_PRMCIO_CRX__COLOR, i, sizeof(uint8_t));
	g_NV2A->BlockWrite(block, NV_PRMCIO_CR__COLOR, d, sizeof(uint8_t));
}

void SRX_WR(void *Ptr, xbox::uchar_xt i, xbox::uchar_xt d)
{
	static const NV2ABlockInfo* block = EmuNV2A_Block(NV_PRMVIO_SRX);

	g_NV2A->BlockWrite(block, NV_PRMVIO_SRX, i, sizeof(uint8_t));
	g_NV2A->BlockWrite(block, NV_PRMVIO_SR, d, sizeof(uint8_t));
}

void GRX_WR(void *Ptr, xbox::uchar_xt i, xbox::uchar_xt d)
{
	static const NV2ABlockInfo* block = EmuNV2A_Block(NV_PRMVIO_GRX);

	g_NV2A->BlockWrite(block, NV_PRMVIO_GRX, i, sizeof(uint8_t));
	g_NV2A->BlockWrite(block, NV_PRMVIO_GX, d, sizeof(uint8_t));
}

void ARX_WR(void *Ptr, xbox::uchar_xt i, xbox::uchar_xt d)
{
	static const NV2ABlockInfo* block = EmuNV2A_Block(NV_PRMCIO_ARX);

	g_NV2A->BlockWrite(block, NV_PRMCIO_ARX, i, sizeof(uint8_t));
	g_NV2A->BlockWrite(block, NV_PRMCIO_ARX, d, sizeof(uint8_t));
}

ULONG AvSMCVideoModeToAVPack(ULONG VideoMode)
{
	switch (VideoMode) {
		case 0x0: return g_bIsChihiro ? AV_PACK_VGA : AV_PACK_SCART; // Scart on Retail/Debug, VGA on Chihiro
		case 0x1: return AV_PACK_HDTV;
		case 0x2: return AV_PACK_VGA;
		case 0x3: return AV_PACK_RFU;
		case 0x4: return AV_PACK_SVIDEO;
		case 0x6: return AV_PACK_STANDARD;
	}

	return AV_PACK_NONE;
}

ULONG AvQueryAvCapabilities()
{
	ULONG avpack = AvSMCVideoModeToAVPack(xbox::HalBootSMCVideoMode);
	ULONG type;
	ULONG resultSize;

	// First, read the factory AV settings
	ULONG avRegion;
	NTSTATUS result = xbox::ExQueryNonVolatileSetting(
		xbox::XC_FACTORY_AV_REGION,
		(xbox::dword_xt *)&type,
		&avRegion,
		sizeof(ULONG),
		(xbox::PSIZE_T)&resultSize);

	// If this failed, default to AV_STANDARD_NTSC_M | AV_FLAGS_60Hz
	if (result != X_STATUS_SUCCESS || resultSize != sizeof(ULONG)) {
		avRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
	}

	// Read the user-configurable (via the dashboard) settings
	ULONG userSettings;
	result = xbox::ExQueryNonVolatileSetting(
		xbox::XC_VIDEO,
		(xbox::dword_xt *) &type,
		&userSettings,
		sizeof(ULONG),
		(xbox::PSIZE_T)&resultSize);

	// If this failed, default to no user-options set
	if (result != X_STATUS_SUCCESS || resultSize != sizeof(ULONG)) {
		userSettings = 0;
	}

	return avpack | (avRegion & (AV_STANDARD_MASK | AV_REFRESH_MASK)) | (userSettings & ~(AV_STANDARD_MASK | AV_PACK_MASK));
}

xbox::PVOID xbox::AvSavedDataAddress = xbox::zeroptr;

// ******************************************************************
// * 0x0001 - AvGetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(1) xbox::PVOID NTAPI xbox::AvGetSavedDataAddress(void)
{
	LOG_FUNC();

	RETURN(AvSavedDataAddress);
}

// ******************************************************************
// * 0x0002 - AvSendTVEncoderOption()
// ******************************************************************
XBSYSAPI EXPORTNUM(2) xbox::void_xt NTAPI xbox::AvSendTVEncoderOption
(
	IN  PVOID   RegisterBase,
	IN  ulong_xt   Option,
	IN  ulong_xt   Param,
	OUT ulong_xt   *Result
)
{
	LOG_FUNC_BEGIN;
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
		// Test case: Is called from AvSetDisplayMode (kernel) and D3DDevice_SetFlickerFilter (D3D8) functions.
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ZERO_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_QUERY_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ENABLE_LUMA_FILTER:
		// Test case: Is called from AvSetDisplayMode (kernel) and D3DDevice_SetSoftDisplayFilter (D3D8) functions.
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
XBSYSAPI EXPORTNUM(3) xbox::ulong_xt NTAPI xbox::AvSetDisplayMode
(
	IN  PVOID   RegisterBase,
	IN  ulong_xt   Step,
	IN  ulong_xt   Mode,
	IN  ulong_xt   Format,
	IN  ulong_xt   Pitch,
	IN  ulong_xt   FrameBuffer
)
{
	LOG_FUNC_BEGIN;
		LOG_FUNC_ARG(RegisterBase)
		LOG_FUNC_ARG(Step)
		LOG_FUNC_ARG(Mode)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_ARG(Pitch)
		LOG_FUNC_ARG(FrameBuffer)
		LOG_FUNC_END;

	if (Mode == AV_MODE_OFF) {
		Mode = AV_MODE_640x480_TO_NTSC_M_YC	| AV_MODE_FLAGS_DACA_DISABLE | AV_MODE_FLAGS_DACB_DISABLE
			| AV_MODE_FLAGS_DACC_DISABLE | AV_MODE_FLAGS_DACD_DISABLE;
	}

	ULONG OutputMode = Mode & AV_MODE_OUT_MASK;
	ULONG iRegister = (Mode & 0x00FF0000) >> 16;
	ULONG iCRTC = (Mode & 0x0000FF00) >> 8;
	ULONG iTV = (Mode & 0x0000007F);
	UCHAR DACs = (UCHAR)((Mode & 0x0F000000) >> 24);

	ULONG GeneralControl = 0;
	UCHAR CR28Depth = 0;

	switch (Format)
	{
	case xbox::X_D3DFMT_LIN_A1R5G5B5:
	case xbox::X_D3DFMT_LIN_X1R5G5B5:
		GeneralControl = NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS | NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON; /*=0x00100030*/
		CR28Depth = 2;
		break;
	case xbox::X_D3DFMT_LIN_R5G6B5:
		GeneralControl = NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS | NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_SEL | NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON; /*=0x00101030*/
		CR28Depth = 2;
		break;
	case xbox::X_D3DFMT_LIN_A8R8G8B8:
	case xbox::X_D3DFMT_LIN_X8R8G8B8:
		GeneralControl = NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS | NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON; /*=0x00100030*/
		CR28Depth = 3;
		break;
	}

	Pitch /= 8;

	static xbox::ulong_xt AvpCurrentMode = 0;
	if (AvpCurrentMode == Mode) {
		REG_WR32(RegisterBase, NV_PRAMDAC_GENERAL_CONTROL, GeneralControl);
		CRTC_WR(RegisterBase, NV_CIO_SR_LOCK_INDEX /*=0x1c*/, NV_CIO_SR_UNLOCK_RW_VALUE); /* crtc lock */
		CRTC_WR(RegisterBase, NV_CIO_CR_OFFSET_INDEX /*=0x13*/, (UCHAR)(Pitch & 0xFF)); /* sets screen pitch */
		CRTC_WR(RegisterBase, NV_CIO_CRE_RPC0_INDEX /*=0x19*/, (UCHAR)((Pitch & 0x700) >> 3)); /* repaint control 0 */
		CRTC_WR(RegisterBase, NV_CIO_CRE_PIXEL_INDEX /*=0x28*/, 0x80 | CR28Depth);
		REG_WR32(RegisterBase, NV_PCRTC_START, FrameBuffer);

		AvSendTVEncoderOption(RegisterBase, AV_OPTION_FLICKER_FILTER, 5, NULL);
		AvSendTVEncoderOption(RegisterBase, AV_OPTION_ENABLE_LUMA_FILTER, FALSE, NULL);

		RETURN(X_STATUS_SUCCESS);
	}

	CRTC_WR(RegisterBase, NV_CIO_CRE_PIXEL_INDEX /*=0x28*/, 0x80 | CR28Depth);

	// TODO: Lots of setup/TV encoder configuration
	// Ignored for now since we don't emulate that stuff yet...

	LOG_INCOMPLETE();

	REG_WR32(RegisterBase, NV_PRAMDAC_GENERAL_CONTROL, GeneralControl);

	const ULONG* pLong = AvpRegisters[iRegister];
	const ULONG* pLongMax = pLong + sizeof(AvpRegisters[0]) / sizeof(ULONG);

	for (long i = 0; pLong < pLongMax; pLong++, i++)	{
		REG_WR32(RegisterBase, AvpRegisters[0][i], *pLong);
	}

	if (Mode & AV_MODE_FLAGS_SCART)	{
		REG_WR32(RegisterBase, 0x00680630, 0); // NV_RAMDAC + 0x0630
		REG_WR32(RegisterBase, 0x006808C4, 0); // NV_RAMDAC + 0x08C4
		REG_WR32(RegisterBase, 0x0068084C, 0); // NV_RAMDAC + 0x084C
	}

	const UCHAR* pByte = AvpSRXRegisters;
	const UCHAR* pByteMax = pByte + sizeof(AvpSRXRegisters);

	for (long i = 0; pByte < pByteMax; pByte++, i++) {
		SRX_WR(RegisterBase, (UCHAR)i, *pByte);
	}

	pByte = AvpGRXRegisters;
	pByteMax = pByte + sizeof(AvpGRXRegisters);

	for (long i = 0; pByte < pByteMax; pByte++, i++)	{
		GRX_WR(RegisterBase, (UCHAR)i, *pByte);
	}

	REG_RD08(RegisterBase, NV_PRMCIO_INP0__COLOR);

	pByte = AvpARXRegisters;
	pByteMax = pByte + sizeof(AvpARXRegisters);

	for (long i = 0; pByte < pByteMax; pByte++, i++)	{
		ARX_WR(RegisterBase, (UCHAR)i, *pByte);
	}

	REG_WR08(RegisterBase, NV_PRMCIO_ARX, 0x20);

	CRTC_WR(RegisterBase, NV_CIO_CR_VRE_INDEX /*=0x11*/, 0x00);
	pByte = AvpCRTCRegisters[iCRTC];
	pByteMax = pByte + sizeof(AvpCRTCRegisters[0]);

	for (long i = 0; pByte < pByteMax; pByte++, i++) {
		UCHAR Register = AvpCRTCRegisters[0][i];
		UCHAR Data = *pByte;

		switch (Register) {
		case NV_CIO_CR_OFFSET_INDEX /*=0x13*/:
			Data = (UCHAR)(Pitch & 0xFF);
			break;
		case NV_CIO_CRE_RPC0_INDEX /*=0x19*/:
			Data |= (UCHAR)((Pitch & 0x700) >> 3);
			break;
		case NV_CIO_CRE_LSR_INDEX /*=0x25*/:
			Data |= (UCHAR)((Pitch & 0x800) >> 6);
			break;
		}

		CRTC_WR(RegisterBase, Register, Data);
	}

	// TODO: More TV Encoder stuff...

	REG_WR32(RegisterBase, NV_PCRTC_START, FrameBuffer);
	AvpCurrentMode = Mode;

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0004 - AvSetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(4) xbox::void_xt NTAPI xbox::AvSetSavedDataAddress
(
	IN  PVOID   Address
)
{
	LOG_FUNC_ONE_ARG(Address);

	AvSavedDataAddress = Address;
}
