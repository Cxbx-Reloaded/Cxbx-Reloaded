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
// *  (c) 2018 Luke Usher
// *
// *  All rights reserved
// *
// ******************************************************************


#include <core\kernel\exports\xboxkrnl.h>

#define LOG_PREFIX CXBXR_MODULE::KRNL

#include <cstdio>
#include <cctype>
#include <clocale>

#include "Logging.h"
#include "EmuKrnlLogging.h"
#include "EmuKrnl.h" // for HalSystemInterrupts
#include "EmuKrnlKi.h" // for KiLockDispatcherDatabase
#include "core/kernel/init/CxbxKrnl.h"
#include "devices/Xbox.h"

// prevent name collisions
namespace NtDll
{
    #include "core/kernel/support/EmuNtDll.h"
};

DWORD PhyInitFlag = 0;
DWORD PhyLinkState = 0;
LONG PhyLockFlag;

// Flags returned by PhyGetLinkState
#define XNET_ETHERNET_LINK_ACTIVE           0x01
#define XNET_ETHERNET_LINK_100MBPS          0x02
#define XNET_ETHERNET_LINK_10MBPS           0x04
#define XNET_ETHERNET_LINK_FULL_DUPLEX      0x08
#define XNET_ETHERNET_LINK_HALF_DUPLEX      0x10

#define PhyLock() InterlockedCompareExchange((::PLONG)(&PhyLockFlag), 1, 0)
#define PhyUnlock() (PhyLockFlag = 0)
#define NETERR(_err)        HRESULT_FROM_WIN32(_err)
#define NETERR_OK           STATUS_SUCCESS
#define NETERR_HARDWARE     0x801f0001  // hardware not responding

#define BIT(n)              (1u << (n))
#define MDIOADR_LOCK        BIT(15)
#define MDIOADR_WRITE       BIT(10)
#define PHY_ADDR			1
#define MDIOADR_PHYSHIFT    5
#define MDIOADR_REGSHIFT    0
#define MIITM_INTERVAL  5
#define PHYRW_TIMEOUT   ((64*2*2*400*MIITM_INTERVAL/1000)*16)
#define MIIREG_CONTROL 0
#define MIIREG_STATUS  1
#define MIIREG_ANAR    4
#define MIIREG_LPANAR  5
#define MIICONTROL_RESET                        BIT(15)
#define MIICONTROL_SPEED_SELECTION_BIT1         BIT(13)
#define MIICONTROL_RESTART_AUTO_NEGOTIATION     BIT(9)
#define MIICONTROL_SPEED_SELECTION_BIT0         BIT(6)
#define MIISTATUS_100MBS_T4_CAPABLE             BIT(15)
#define MIISTATUS_100MBS_X_HALF_DUPLEX_CAPABLE  BIT(13)
#define MIISTATUS_10MBS_HALF_DUPLEX_CAPABLE     BIT(11)
#define MIISTATUS_100MBS_T2_HALF_DUPLEX_CAPABLE BIT(9)
#define MIISTATUS_AUTO_NEGOTIATION_COMPLETE     BIT(5)
#define MIISTATUS_AUTO_NEGOTIATION_CAPABLE      BIT(3)
#define MII4_100BASE_T4                         BIT(9)
#define MII4_100BASE_T_FULL_DUPLEX              BIT(8)
#define MII4_100BASE_T_HALF_DUPLEX              BIT(7)
#define MII4_10BASE_T_FULL_DUPLEX               BIT(6)
#define MII4_10BASE_T_HALF_DUPLEX               BIT(5)
#define MIISTATUS_LINK_IS_UP                    BIT(2)

void PhyClearMDIOLOCK()
{
	g_NVNet->MMIOWrite(0, NvRegMIIControl, MDIOADR_LOCK, sizeof(DWORD));
	int timeout = PHYRW_TIMEOUT;

	do {
		xbox::KeStallExecutionProcessor(50);
		timeout -= 50;
	} while (timeout > 0 && (g_NVNet->MMIORead(0, NvRegMIIControl, sizeof(DWORD)) & MDIOADR_LOCK));
}

// NON-Exported Phy Kernel functions
BOOL PhyReadReg(DWORD phyreg, DWORD* val)
{
	DWORD mdioadr;
	INT timeout;

	if (g_NVNet->MMIORead(0, NvRegMIIControl, sizeof(DWORD)) & MDIOADR_LOCK) {
		PhyClearMDIOLOCK();
	}

	mdioadr = (PHY_ADDR << MDIOADR_PHYSHIFT) | (phyreg << MDIOADR_REGSHIFT);
	g_NVNet->MMIOWrite(0, NvRegMIIControl, mdioadr, sizeof(DWORD));
	mdioadr |= MDIOADR_LOCK;

	for (timeout = PHYRW_TIMEOUT; timeout > 0 && (mdioadr & MDIOADR_LOCK); timeout -= 50) {
		xbox::KeStallExecutionProcessor(50);
		mdioadr = g_NVNet->MMIORead(0, NvRegMIIControl, sizeof(DWORD));
	}

	*val = g_NVNet->MMIORead(0, NvRegMIIData, sizeof(DWORD));

	if (mdioadr & MDIOADR_LOCK) {
		EmuLog(LOG_LEVEL::WARNING, "PHY read failed: reg %d.\n", phyreg);
		return FALSE;
	}

	return TRUE;
}

BOOL PhyWriteReg(DWORD phyreg, DWORD val)
{

	DWORD mdioadr;
	INT timeout;

	if (g_NVNet->MMIORead(0, NvRegMIIControl, sizeof(DWORD)) & MDIOADR_LOCK) {
		PhyClearMDIOLOCK();
	}

	g_NVNet->MMIOWrite(0, NvRegMIIData, val, sizeof(DWORD));

	mdioadr = (PHY_ADDR << MDIOADR_PHYSHIFT) | (phyreg << MDIOADR_REGSHIFT) | MDIOADR_WRITE;
	g_NVNet->MMIOWrite(0, NvRegMIIControl, mdioadr, sizeof(DWORD));
	mdioadr |= MDIOADR_LOCK;

	for (timeout = PHYRW_TIMEOUT; timeout > 0 && (mdioadr & MDIOADR_LOCK); timeout -= 50) {
		xbox::KeStallExecutionProcessor(50);
		mdioadr = g_NVNet->MMIORead(0, NvRegMIIControl, sizeof(DWORD));
	}

	if (mdioadr & MDIOADR_LOCK) {
		EmuLog(LOG_LEVEL::WARNING, "PHY write failed: reg %d.\n", phyreg);
		return FALSE;
	}

	return TRUE;
}

DWORD PhyWaitForLinkUp()
{
	DWORD miiStatus = 0;
	INT timeout = 1000;
	while (timeout-- && !(miiStatus & MIISTATUS_LINK_IS_UP)) {
		xbox::KeStallExecutionProcessor(500);
		if (!PhyReadReg(MIIREG_STATUS, &miiStatus)) {
			break;
		}
	}

	return miiStatus;
}

BOOL PhyUpdateLinkState()
{
	DWORD anar, lpanar, miiStatus, state = 0;

	if (!PhyReadReg(MIIREG_ANAR, &anar) || !PhyReadReg(MIIREG_LPANAR, &lpanar) || !PhyReadReg(MIIREG_STATUS, &miiStatus)) {
		return FALSE;
	}

	anar &= lpanar;
	if (anar & (MII4_100BASE_T_FULL_DUPLEX | MII4_100BASE_T_HALF_DUPLEX)) {
		state |= XNET_ETHERNET_LINK_100MBPS;
	} else if (anar & (MII4_10BASE_T_FULL_DUPLEX | MII4_10BASE_T_HALF_DUPLEX)) {
		state |= XNET_ETHERNET_LINK_10MBPS;
	}

	if (anar & (MII4_10BASE_T_FULL_DUPLEX | MII4_100BASE_T_FULL_DUPLEX)) {
		state |= XNET_ETHERNET_LINK_FULL_DUPLEX;
	} else if (anar & (MII4_10BASE_T_HALF_DUPLEX | MII4_100BASE_T_HALF_DUPLEX)) {
		state |= XNET_ETHERNET_LINK_HALF_DUPLEX;
	}

	if (miiStatus & MIISTATUS_LINK_IS_UP) {
		state |= XNET_ETHERNET_LINK_ACTIVE;
	}

	PhyLinkState = state;

	return TRUE;
}

// ******************************************************************
// * 0x00FC - PhyGetLinkState()
// ******************************************************************
XBSYSAPI EXPORTNUM(252) xbox::dword_t NTAPI xbox::PhyGetLinkState
(
	IN ulong_t	Mode
)
{
	LOG_FUNC_ONE_ARG(Mode);

	if ((!PhyLinkState || Mode) && PhyLock() == 0) {
		PhyUpdateLinkState();
		PhyUnlock();
	}

	RETURN(PhyLinkState);
}

// ******************************************************************
// * 0x00FD - PhyInitialize()
// ******************************************************************
XBSYSAPI EXPORTNUM(253) xbox::NTSTATUS NTAPI xbox::PhyInitialize
(
	IN ulong_t	forceReset,
	IN PVOID	Parameter2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(forceReset)
		LOG_FUNC_ARG(Parameter2)
		LOG_FUNC_END;

	DWORD miiControl, miiStatus;
	INT timeout;
	NTSTATUS status = NETERR_HARDWARE;
	
	if (PhyLock() != 0) {
		return NETERR(ERROR_BUSY);
	}
	
	if (forceReset) {
		PhyInitFlag = 0;
		PhyLinkState = 0;

		miiControl = MIICONTROL_RESET;
		if (!PhyWriteReg(MIIREG_CONTROL, miiControl)) {
			goto err;
		}

		timeout = 1000;
		while (timeout-- && (miiControl & MIICONTROL_RESET)) {
			KeStallExecutionProcessor(500);
			if (!PhyReadReg(MIIREG_CONTROL, &miiControl)) {
				goto err;
			}
		}

		if (miiControl & MIICONTROL_RESET) {
			goto err;
		}
	} else if (PhyInitFlag) {
		PhyUpdateLinkState();
		status = NETERR_OK;
		goto exit;
	}

	timeout = 6000;
	miiStatus = 0;
	while (timeout-- && !(miiStatus & MIISTATUS_AUTO_NEGOTIATION_COMPLETE)) {
		KeStallExecutionProcessor(500);
		if (!PhyReadReg(MIIREG_STATUS, &miiStatus)) goto err;
	}

	if (!PhyReadReg(MIIREG_CONTROL, &miiControl)) {
		goto err;
	}

	if (miiControl & MIICONTROL_RESTART_AUTO_NEGOTIATION) {
		if (miiStatus & (MIISTATUS_100MBS_T4_CAPABLE | MIISTATUS_100MBS_X_HALF_DUPLEX_CAPABLE |	MIISTATUS_100MBS_T2_HALF_DUPLEX_CAPABLE)) {
			miiControl |= MIICONTROL_SPEED_SELECTION_BIT1;
			miiControl &= ~MIICONTROL_SPEED_SELECTION_BIT0;
			PhyLinkState |= XNET_ETHERNET_LINK_100MBPS;
		} else if (miiStatus & MIISTATUS_10MBS_HALF_DUPLEX_CAPABLE) {
			miiControl &= ~MIICONTROL_SPEED_SELECTION_BIT1;
			miiControl |= MIICONTROL_SPEED_SELECTION_BIT0;
			PhyLinkState |= XNET_ETHERNET_LINK_10MBPS;
		} else {
			goto err;
		}

		PhyLinkState |= XNET_ETHERNET_LINK_HALF_DUPLEX;
		PhyWriteReg(MIIREG_CONTROL, miiControl);
		miiStatus = PhyWaitForLinkUp();

		if (miiStatus & MIISTATUS_LINK_IS_UP) {
			PhyLinkState |= XNET_ETHERNET_LINK_ACTIVE;
		}
	} else {
		PhyWaitForLinkUp();
		if (!PhyUpdateLinkState()) {
			goto err;
		}
	}

	PhyInitFlag = 1;
	status = NETERR_OK;

exit:
	PhyUnlock();
	RETURN(status);
err:
	EmuLog(LOG_LEVEL::WARNING, "Ethernet PHY initialization failed.\n");
	goto exit;
}
