// CxbxVSBC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


// MathFuncsDll.cpp : Defines the exported functions for the DLL application.
//

#include "CxbxVSBC.h"
#include <stdexcept>
#include "DlgVirtualSBCFeedback.h"


using namespace std;

namespace CxbxVSBC
{
    
    // Get input state
    extern "C" __declspec(dllexport) int __stdcall WINAPI  MyCxbxVSBC::VSBCGetState(UCHAR * pSBCGamepad)
    {
        return 0;
    }

    // Set feedback state
    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCSetState(UCHAR * pSBCFeedback)
    {
        DlgVSBCFeedback::UpdateVirtualSBCFeedbackDlg(pSBCFeedback);
        return 0;
    }

    // open SBC, initialize virtual controller and dialog
    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCOpen(DWORD dwVSBCType)
    {
        if (dwVSBCType == X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC) {
            //DlgVSBCFeedback::ShowVirtualSBCFeedback(NULL);
            return 0;
        }
        else {
            return -1;
        }

    }

    
    // Show the VSBC dialog
    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCShow(void)
    {
        return 0;
    }
    // Hide the VSBC dialog
    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCHide(void)
    {
        return 0;
    }

    
}
