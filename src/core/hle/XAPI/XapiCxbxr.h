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
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

// This file contains Xapi variables which are not part of the Xbox Xapi but are used by Cxbxr. They are prefixed with CXBX_ to further distinguish them.

#ifndef XAPI_CXBXR_H
#define XAPI_CXBXR_H

// ******************************************************************
// * XINPUT_DEVICE_INFO
// ******************************************************************

typedef struct _CXBX_XINPUT_DEVICE_INFO
{

	UCHAR                   ucType;             //xbox controller type
	UCHAR                   ucSubType;          //xbox controller subtype
	UCHAR                   ucInputStateSize;   //xbox controller input state size in bytes, not include dwPacketNumber
	UCHAR                   ucFeedbackSize;     //xbox controller feedback size in bytes, not include FeedbackHeader
	DWORD                   dwPacketNumber;
}
CXBX_XINPUT_DEVICE_INFO, *PCXBX_XINPUT_DEVICE_INFO;

//this structure is for use of tracking the xbox controllers assigned to 4 ports.
// ******************************************************************
// * CXBX_CONTROLLER_HOST_BRIDGE 
// ******************************************************************
typedef struct _CXBX_CONTROLLER_HOST_BRIDGE
{
	HANDLE                  hXboxDevice;        //xbox device handle to this device, we use the address of this bridge as the handle, only set after opened. cleared after closed.
	int                     XboxPort;           //xbox port# for this xbox controller
	XBOX_INPUT_DEVICE       XboxType;           //xbox device type
	void*                   InState;
	bool                    bPendingRemoval;
	bool                    bSignaled;
	bool                    bIoInProgress;
	CXBX_XINPUT_DEVICE_INFO XboxDeviceInfo;
}
CXBX_CONTROLLER_HOST_BRIDGE, *PCXBX_CONTROLLER_HOST_BRIDGE;

// ******************************************************************
// * offsets into analog button array
// ******************************************************************
#define CXBX_XINPUT_GAMEPAD_A                0
#define CXBX_XINPUT_GAMEPAD_B                1
#define CXBX_XINPUT_GAMEPAD_X                2
#define CXBX_XINPUT_GAMEPAD_Y                3
#define CXBX_XINPUT_GAMEPAD_BLACK            4
#define CXBX_XINPUT_GAMEPAD_WHITE            5
#define CXBX_XINPUT_GAMEPAD_LEFT_TRIGGER     6
#define CXBX_XINPUT_GAMEPAD_RIGHT_TRIGGER    7

// ******************************************************************
// * masks for digital buttons
// ******************************************************************
#define CXBX_XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define CXBX_XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define CXBX_XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define CXBX_XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define CXBX_XINPUT_GAMEPAD_START            0x00000010
#define CXBX_XINPUT_GAMEPAD_BACK             0x00000020
#define CXBX_XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define CXBX_XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

// ******************************************************************
// * masks for digital buttons of SBC_GAMEPAD
// ******************************************************************

#define    CXBX_SBC_GAMEPAD_W0_RIGHTJOYMAINWEAPON      0x0001
#define    CXBX_SBC_GAMEPAD_W0_RIGHTJOYFIRE            0x0002
#define    CXBX_SBC_GAMEPAD_W0_RIGHTJOYLOCKON          0x0004
#define    CXBX_SBC_GAMEPAD_W0_EJECT                   0x0008
#define    CXBX_SBC_GAMEPAD_W0_COCKPITHATCH            0x0010
#define    CXBX_SBC_GAMEPAD_W0_IGNITION                0x0020
#define    CXBX_SBC_GAMEPAD_W0_START                   0x0040
#define    CXBX_SBC_GAMEPAD_W0_MULTIMONOPENCLOSE       0x0080
#define    CXBX_SBC_GAMEPAD_W0_MULTIMONMAPZOOMINOUT    0x0100
#define    CXBX_SBC_GAMEPAD_W0_MULTIMONMODESELECT      0x0200
#define    CXBX_SBC_GAMEPAD_W0_MULTIMONSUBMONITOR      0x0400
#define    CXBX_SBC_GAMEPAD_W0_MAINMONZOOMIN           0x0800
#define    CXBX_SBC_GAMEPAD_W0_MAINMONZOOMOUT          0x1000
#define    CXBX_SBC_GAMEPAD_W0_FUNCTIONFSS             0x2000
#define    CXBX_SBC_GAMEPAD_W0_FUNCTIONMANIPULATOR     0x4000
#define    CXBX_SBC_GAMEPAD_W0_FUNCTIONLINECOLORCHANGE 0x8000
#define    CXBX_SBC_GAMEPAD_W1_WASHING                 0x0001
#define    CXBX_SBC_GAMEPAD_W1_EXTINGUISHER            0x0002
#define    CXBX_SBC_GAMEPAD_W1_CHAFF                   0x0004
#define    CXBX_SBC_GAMEPAD_W1_FUNCTIONTANKDETACH      0x0008
#define    CXBX_SBC_GAMEPAD_W1_FUNCTIONOVERRIDE        0x0010
#define    CXBX_SBC_GAMEPAD_W1_FUNCTIONNIGHTSCOPE      0x0020
#define    CXBX_SBC_GAMEPAD_W1_FUNCTIONF1              0x0040
#define    CXBX_SBC_GAMEPAD_W1_FUNCTIONF2              0x0080
#define    CXBX_SBC_GAMEPAD_W1_FUNCTIONF3              0x0100
#define    CXBX_SBC_GAMEPAD_W1_WEAPONCONMAIN           0x0200
#define    CXBX_SBC_GAMEPAD_W1_WEAPONCONSUB            0x0400
#define    CXBX_SBC_GAMEPAD_W1_WEAPONCONMAGAZINE       0x0800
#define    CXBX_SBC_GAMEPAD_W1_COMM1                   0x1000
#define    CXBX_SBC_GAMEPAD_W1_COMM2                   0x2000
#define    CXBX_SBC_GAMEPAD_W1_COMM3                   0x4000
#define    CXBX_SBC_GAMEPAD_W1_COMM4                   0x8000
#define    CXBX_SBC_GAMEPAD_W2_COMM5                   0x0001
#define    CXBX_SBC_GAMEPAD_W2_LEFTJOYSIGHTCHANGE      0x0002
#define    CXBX_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL     0x0004
#define    CXBX_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY      0x0008
#define    CXBX_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE      0x0010
#define    CXBX_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL    0x0020
#define    CXBX_SBC_GAMEPAD_W2_TOGGLEVTLOCATION        0x0040

// ******************************************************************
// * enum for feedback status variables of SBC_FEEDBACK, it's a byte array after FeedbackHeader, each variable take 1 nibble, that's half byte.
// ******************************************************************
#define    CXBX_SBC_FEEDBACK_EMERGENCYEJECT            0
#define    CXBX_SBC_FEEDBACK_COCKPITHATCH              1
#define    CXBX_SBC_FEEDBACK_IGNITION                  2
#define    CXBX_SBC_FEEDBACK_START                     3
#define    CXBX_SBC_FEEDBACK_OPENCLOSE                 4
#define    CXBX_SBC_FEEDBACK_MAPZOOMINOUT              5
#define    CXBX_SBC_FEEDBACK_MODESELECT                6
#define    CXBX_SBC_FEEDBACK_SUBMONITORMODESELECT      7
#define    CXBX_SBC_FEEDBACK_MAINMONITORZOOMIN         8
#define    CXBX_SBC_FEEDBACK_MAINMONITORZOOMOUT        9
#define    CXBX_SBC_FEEDBACK_FORECASTSHOOTINGSYSTEM    10
#define    CXBX_SBC_FEEDBACK_MANIPULATOR               11
#define    CXBX_SBC_FEEDBACK_LINECOLORCHANGE           12
#define    CXBX_SBC_FEEDBACK_WASHING                   13
#define    CXBX_SBC_FEEDBACK_EXTINGUISHER              14
#define    CXBX_SBC_FEEDBACK_CHAFF                     15
#define    CXBX_SBC_FEEDBACK_TANKDETACH                16
#define    CXBX_SBC_FEEDBACK_OVERRIDE                  17
#define    CXBX_SBC_FEEDBACK_NIGHTSCOPE                18
#define    CXBX_SBC_FEEDBACK_F1                        19
#define    CXBX_SBC_FEEDBACK_F2                        20
#define    CXBX_SBC_FEEDBACK_F3                        21
#define    CXBX_SBC_FEEDBACK_MAINWEAPONCONTROL         22
#define    CXBX_SBC_FEEDBACK_SUBWEAPONCONTROL          23
#define    CXBX_SBC_FEEDBACK_MAGAZINECHANGE            24
#define    CXBX_SBC_FEEDBACK_COMM1                     25
#define    CXBX_SBC_FEEDBACK_COMM2                     26
#define    CXBX_SBC_FEEDBACK_COMM3                     27
#define    CXBX_SBC_FEEDBACK_COMM4                     28
#define    CXBX_SBC_FEEDBACK_COMM5                     29
#define    CXBX_SBC_FEEDBACK_UNKNOWN                   30
#define    CXBX_SBC_FEEDBACK_GEARR                     31
#define    CXBX_SBC_FEEDBACK_GEARN                     32
#define    CXBX_SBC_FEEDBACK_GEAR1                     33
#define    CXBX_SBC_FEEDBACK_GEAR2                     34
#define    CXBX_SBC_FEEDBACK_GEAR3                     35
#define    CXBX_SBC_FEEDBACK_GEAR4                     36
#define    CXBX_SBC_FEEDBACK_GEAR5                     37

#define    CXBX_SBC_FEEDBACK_MAX                       38

#endif
