//#include "stdafx.h"
#include "SBCUSB.h"

// Example configuration:
#define WRITE_EP_ADDRESS		0x01
#define READ_EP_ADDRESS			0x82
#define MAX_TRANSFER_SIZE		4096
#define MAX_PENDING_TRANSFERS	128
#define MAX_PENDING_IO			3

// ******************************************************************
// * X_SBC_GAMEPAD for xbox SteelBatalion GAMEPAD struc 
// ******************************************************************
typedef struct _X_SBC_GAMEPAD {
    WORD    wButtons[3];
    SHORT   sAimingX;
    SHORT   sAimingY;
    SHORT   sRotationLever;//maybe only high byte was used.
    SHORT   sSightChangeX;
    SHORT   sSightChangeY;
    WORD    wLeftPedal;//maybe only high byte was used.
    WORD    wMiddlePedal;//maybe only high byte was used.
    WORD    wRightPedal;//maybe only high byte was used.
    UCHAR   ucTunerDial;//low nibble, The 9 o'clock postion is 0, and the 6 o'clock position is 12. The blank area between the 6 and 9 o'clock positions is 13, 14, and 15 clockwise.
    UCHAR   ucGearLever;//GearLever 1~5 for gear 1~5, 7~13 for gear R,N,1~5, 15 for gear R. we use the continues range from 7~13
}
X_SBC_GAMEPAD, *PX_SBC_GAMEPAD;

PX_SBC_GAMEPAD g_pSBCGamepad;

/// The byte buffer that the raw control data is stored
/// </summary>
UCHAR rawControlData[26];

/// <summary>
/// The byte buffer that the raw LED data is stored
/// </summary>
UCHAR rawLEDData[34];

// Gobals:
KUSB_DRIVER_API		Usb;
DATA_COUNTER_STATS	Dcs;
KLST_HANDLE g_deviceList = NULL;
KLST_DEVINFO_HANDLE g_deviceInfo = NULL;
static KUSB_HANDLE g_usbHandle = NULL;
static KSTM_HANDLE streamHandleRead = NULL;
static KSTM_HANDLE streamHandleWrite = NULL;

DWORD SBCUSB_Init(void)
{
    KLST_HANDLE deviceList = NULL;
    KLST_DEVINFO_HANDLE deviceInfo = NULL;
    KUSB_HANDLE usbHandle = NULL;
    KSTM_HANDLE streamHandleRead = NULL;
    KSTM_HANDLE streamHandleWrite = NULL;
    DWORD errorCode = ERROR_SUCCESS;
    DWORD transferLength = 0;
    BOOL success;
    UCHAR myBuffer[MAX_TRANSFER_SIZE * MAX_PENDING_IO];
    //BM_TEST_TYPE testType = USB_ENDPOINT_DIRECTION_IN(EP_ADDRESS) ? BM_TEST_TYPE_READ : BM_TEST_TYPE_WRITE;

    /*
    Find the test device. Uses "vid=hhhh pid=hhhh" arguments supplied on the
    command line. (default is: vid=04D8 pid=FA2E)
    */
    //if (!Examples_GetTestDevice(&deviceList, &deviceInfo, argc, argv))
    //	return GetLastError();

    ULONG vidArg = SBC_VID;
    ULONG pidArg = SBC_PID;
    ULONG deviceCount = 0;
    int argPos;

    // init
    // deviceList = NULL;
    //deviceInfo = NULL;

    // Get the device list
    if (!LstK_Init(&deviceList, 0))
    {
        printf("Error initializing device list.\n");
        return FALSE;
    }

    LstK_Count(deviceList, &deviceCount);
    if (!deviceCount)
    {
        printf("Device list empty.\n");
        SetLastError(ERROR_DEVICE_NOT_CONNECTED);

        // If LstK_Init returns TRUE, the list must be freed.
        LstK_Free(deviceList);

        return FALSE;
    }

    printf("Looking for device vid/pid %04X/%04X..\n", vidArg, pidArg);

    LstK_FindByVidPid(deviceList, vidArg, pidArg, &deviceInfo);

    if (deviceInfo)
    {
        // This function returns the device list and the device info
        // element which matched.  The caller is responsible for freeing
        // this list when it is no longer needed.

        // Report the connection state of the example device
        printf("Using %04X:%04X (%s): %s - %s\n",
            deviceInfo->Common.Vid,
            deviceInfo->Common.Pid,
            deviceInfo->Common.InstanceID,
            deviceInfo->DeviceDesc,
            deviceInfo->Mfg);
    }
    else
    {
        // Display some simple usage information for the example applications.
        CHAR programPath[MAX_PATH] = { 0 };
        PCHAR programExe = programPath;
        GetModuleFileNameA(GetModuleHandleA(NULL), programPath, sizeof(programPath));
        while (strpbrk(programExe, "\\/")) programExe = strpbrk(programExe, "\\/") + 1;
        printf("Device vid/pid %04X/%04X not found.\n\n", vidArg, pidArg);
        printf("USAGE: %s vid=%04X pid=%04X\n\n", programExe, vidArg, pidArg);

        // If LstK_Init returns TRUE, the list must be freed.
        LstK_Free(deviceList);

        return FALSE;
    }

    LibK_LoadDriverAPI(&Usb, deviceInfo->DriverID);

    /*
    Initialize the device. This creates the physical usb handle.
    */
    if (!Usb.Init(&usbHandle, deviceInfo))
    {
        errorCode = GetLastError();
        printf("Usb.Init failed. ErrorCode: %08Xh\n", errorCode);
        return errorCode;
    }
    printf("Device opened successfully!\n");

    g_usbHandle=usbHandle;

    g_deviceList = deviceList;

    return errorCode;
}

BOOL SBCUSB_GetState(UCHAR * pSBCGamepad)
{
    BOOL success;
    DWORD errorCode = ERROR_SUCCESS;
    DWORD transferLength = 0;

    success = Usb.ReadPipe(g_usbHandle, READ_EP_ADDRESS, rawControlData, sizeof(rawControlData), &transferLength, NULL);
	if (success)
	{
        //copy input data to gamepad structure. 24 bytes total
        memcpy(pSBCGamepad, rawControlData+2, 24);
        g_pSBCGamepad= rawControlData + 2;
        //transferLength += length;
	}
	else
	{
		// If the return result is ERROR_NO_MORE_ITEMS then there is no more data
		// to read.  Other errors indicate a problem.
        //not sure how to handle the error here.
        /*
        if (GetLastError() != ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		if (transferLength >= sizeof(myBuffer) * 8)
		{
			mDcs_MarkStop(&Dcs, transferLength);
			printf("Transferred bytes:%d BPS:%.2f\n", transferLength, Dcs.Bps);
			transferLength = 0;
		}
		if ((Dcs.TotalBytes >= (sizeof(myBuffer) * 128)))
		{
			if (transferLength)
			{
				mDcs_MarkStop(&Dcs, transferLength);
				transferLength = 0;
			}

			// Stop and start the stream; This is done here to excercise the API only.
			success = StmK_Stop(streamHandle, 0);
			errorCode = (success) ? ERROR_SUCCESS  : GetLastError();
			if (!success) printf("StmK_Stop failed. ErrorCode: %08Xh\n", errorCode);
			if (!success) goto Done;

			printf("[StreamResult] "
				    "Bytes-total:%I64d Seconds-total:%.3f BPS-average:%.2f\n",
				    Dcs.TotalBytes, Dcs.Duration, Dcs.Bps);

			success = StmK_Start(streamHandle);
			errorCode = (success) ? ERROR_SUCCESS  : GetLastError();
			if (!success) printf("StmK_Start failed. ErrorCode: %08Xh\n", errorCode);
			if (!success) goto Done;
			//////////////////////////////////////////////////////////////////////////

			mDcs_Init(&Dcs);
		}
		else
		{
			// ..Emulate some work..
			Sleep(100);
		}
        */
        errorCode = GetLastError();
    }
	return errorCode;
}

BOOL SBCUSB_SetState(UCHAR * pSBCFeedback)
{
    BOOL success;
    DWORD errorCode = ERROR_SUCCESS;
    DWORD transferLength = 0;
    ULONG length;
    //copy feedback data to rawLEDData, total 20 bytes, but only 19 bytes are known.
    memcpy(rawLEDData + 2, pSBCFeedback, 20);
    success = Usb.WritePipe(g_usbHandle, WRITE_EP_ADDRESS, rawLEDData, sizeof(rawLEDData), &length, NULL);

    if (success)
    {
        transferLength += length;
    }
    else
    {
        // If the return result is ERROR_NO_MORE_ITEMS then there is no more data
        // to read.  Other errors indicate a problem.
        //not sure how to handle the error here.
        /*
        if (GetLastError() != ERROR_NO_MORE_ITEMS)
        {
        break;
        }
        if (transferLength >= sizeof(myBuffer) * 8)
        {
        mDcs_MarkStop(&Dcs, transferLength);
        printf("Transferred bytes:%d BPS:%.2f\n", transferLength, Dcs.Bps);
        transferLength = 0;
        }
        if ((Dcs.TotalBytes >= (sizeof(myBuffer) * 128)))
        {
        if (transferLength)
        {
        mDcs_MarkStop(&Dcs, transferLength);
        transferLength = 0;
        }

        // Stop and start the stream; This is done here to excercise the API only.
        success = StmK_Stop(streamHandle, 0);
        errorCode = (success) ? ERROR_SUCCESS  : GetLastError();
        if (!success) printf("StmK_Stop failed. ErrorCode: %08Xh\n", errorCode);
        if (!success) goto Done;

        printf("[StreamResult] "
        "Bytes-total:%I64d Seconds-total:%.3f BPS-average:%.2f\n",
        Dcs.TotalBytes, Dcs.Duration, Dcs.Bps);

        success = StmK_Start(streamHandle);
        errorCode = (success) ? ERROR_SUCCESS  : GetLastError();
        if (!success) printf("StmK_Start failed. ErrorCode: %08Xh\n", errorCode);
        if (!success) goto Done;
        //////////////////////////////////////////////////////////////////////////

        mDcs_Init(&Dcs);
        }
        else
        {
        // ..Emulate some work..
        Sleep(100);
        }
        */
        errorCode = GetLastError();
    }
    return errorCode;
}

BOOL SBCUSB_Close(void)
{
    // Free the stream handle.
    if (streamHandleRead)
    {
        StmK_Free(streamHandleRead);
    }

    // Free the stream handle.
    if (streamHandleWrite)
    {
        StmK_Free(streamHandleWrite);
    }

    // Free the usb handle.
    UsbK_Free(g_usbHandle);

    // Free the device list.
    LstK_Free(g_deviceList);
}
