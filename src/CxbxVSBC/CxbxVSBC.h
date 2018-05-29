#ifdef CXBXVSBC_EXPORTS
#define CXBXVSBC_API __declspec(dllexport) 
#else
#define CXBXVSBC_API __declspec(dllimport) 
#endif#pragma once
//#include <windef.h>
//#include "../CxbxKrnl/EmuXapi.h"
#define X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC 0x80
/*
extern "C" static CXBXVSBC_API DWORD VSBCGetState(UCHAR * pSBCGamepad);
extern "C" static CXBXVSBC_API DWORD VSBCSetState(UCHAR * pSBCFeedback);
extern "C" static CXBXVSBC_API DWORD VSBCOpen(DWORD dwVSBCType);
extern "C" static CXBXVSBC_API DWORD VSBCShow(void);
extern "C" static CXBXVSBC_API DWORD VSBCHide(void);
*/
namespace CxbxVSBC
{
    //
    class MyCxbxVSBC
    {
    public:
        // Get input state
        static  __declspec(dllexport) int __stdcall WINAPI VSBCGetState(UCHAR * pSBCGamepad);

        // Set feedback state
        static __declspec(dllexport) int __stdcall WINAPI VSBCSetState(UCHAR * pSBCFeedback);

        // open SBC, initialize virtual controller and dialog

        static __declspec(dllexport) int __stdcall WINAPI VSBCOpen(DWORD dwVSBCType);

        // open SBC, initialize virtual controller and dialog
        // static CXBXVSBC_API DWORD VSBCOpen(DWORD dwVSBCType);

        // Show the VSBC dialog
        static __declspec(dllexport) int __stdcall WINAPI VSBCShow(void);
        
        // Hide the VSBC dialog
        static __declspec(dllexport) int __stdcall WINAPI VSBCHide(void);

    private:
        HMODULE hCurrentModule = 0;
        int   dwVSBCOpened = 0;
        int   dwVSBCOpenType = 0;
    };
}
