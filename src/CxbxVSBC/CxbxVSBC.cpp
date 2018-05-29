// CxbxVSBC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


// MathFuncsDll.cpp : Defines the exported functions for the DLL application.
//

#include "CxbxVSBC.h"
extern "C"
{
#include "SBCUSB.h"
}
//#include "../CxbxKrnl/EmuXapi.h"
#include <stdexcept>
#include "DlgVirtualSBCFeedback.h"


using namespace std;

namespace CxbxVSBC
{



    DWORD g_dwSBCType = 0;
#define MYMENU_EXIT         (WM_APP + 101)
    //#define MYMENU_MESSAGEBOX   (WM_APP + 102)

    HINSTANCE  inj_hModule;          //Injected Modules Handle
    HWND       prnt_hWnd;            //Parent Window Handle

                                     //WndProc for the new window
    LRESULT CALLBACK DLLWindowProc(HWND, UINT, WPARAM, LPARAM);

    //Register our windows Class
    BOOL RegisterDLLWindowClass()//wchar_t szClassName[])
    {
        WNDCLASSEX wc;
        wc.hInstance = inj_hModule;
        wc.lpszClassName = (LPCSTR)"CxbxVSBCClass";
        //wc.lpszClassName = (LPCWSTR)szClassName;
        wc.lpfnWndProc = DLLWindowProc;
        wc.style = CS_DBLCLKS;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszMenuName = NULL;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
        if (!RegisterClassEx(&wc))
            return 0;
    }
    //Creating our windows Menu
    HMENU CreateDLLWindowMenu()
    {
        HMENU hMenu;
        hMenu = CreateMenu();
        HMENU hMenuPopup;
        if (hMenu == NULL)
            return FALSE;
        hMenuPopup = CreatePopupMenu();
        AppendMenu(hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("File"));

        hMenuPopup = CreatePopupMenu();
        //AppendMenu(hMenuPopup, MF_STRING, MYMENU_MESSAGEBOX, TEXT("MessageBox"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("Test"));
        return hMenu;
    }

    //The new window thread
    DWORD WINAPI ThreadProc(LPVOID lpParam)
    {
        MSG messages;
        wchar_t *pString = reinterpret_cast<wchar_t * > (lpParam);
        HMENU hMenu = CreateDLLWindowMenu();
        RegisterDLLWindowClass();
        //prnt_hWnd = FindWindow("Window Injected Into ClassName", "Window Injected Into Caption");
        HWND hwnd = CreateWindowEx(0, (LPCSTR)"InjectedDLLWindowClass", (LPCSTR)pString, WS_EX_PALETTEWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, prnt_hWnd, hMenu, inj_hModule, NULL);
        //this window is only for bridging the dialog, so do not show it.
        //ShowWindow(hwnd, SW_SHOWNORMAL);
        //call function to creat the dialog, pass in the handle of the newly created window as parent handle.
        DlgVSBCFeedback::ShowVirtualSBCFeedback(hwnd);
        while (GetMessage(&messages, NULL, 0, 0))
        {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        return 1;
    }
    //new windows proc
    LRESULT CALLBACK DLLWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_COMMAND:
            switch (wParam)
            {
            case MYMENU_EXIT:
                SendMessage(hwnd, WM_CLOSE, 0, 0);
                break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        return 0;
    }




    // Get input state
    extern "C" __declspec(dllexport) int __stdcall WINAPI  MyCxbxVSBC::VSBCGetState(UCHAR * pSBCGamepad)
    {
        switch (g_dwSBCType) {
        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC:
            //DlgVSBCFeedback::UpdateVirtualSBCFeedbackDlg(pSBCFeedback);
            return 0;

        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_USB_SBC:
            if (SBCUSB_GetState(pSBCGamepad) == 0) {
                return 0;
            }
            return -1;

        default:
            return -1;
        }
    }

    // Set feedback state
    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCSetState(UCHAR * pSBCFeedback)
    {

        switch (g_dwSBCType) {
        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC:
            //DlgVSBCFeedback::ShowVirtualSBCFeedback(NULL);
            DlgVSBCFeedback::UpdateVirtualSBCFeedbackDlg(pSBCFeedback);
            return 0;

        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_USB_SBC:
            if (SBCUSB_SetState(pSBCFeedback) == 0) {
                return 0;
            }
            return -1;

        default:
            return -1;
        }
    }

    // open SBC, initialize virtual controller and dialog
    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCOpen(DWORD dwVSBCType)
    {
        switch (dwVSBCType) {
        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC:
            //inj_hModule = hModule;
            CreateThread(0, NULL, ThreadProc, (LPVOID)L"Window Title", NULL, NULL);
            g_dwSBCType = X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC;
            return 0;

        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_USB_SBC:
            if (SBCUSB_Init() == 0) {
                g_dwSBCType = X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_USB_SBC;
                return 0;
            }
            return -1;

        default:
            return -1;
        }
    }

    extern "C" __declspec(dllexport) int __stdcall WINAPI MyCxbxVSBC::VSBCClose(DWORD dwVSBCType)
    {
        switch (dwVSBCType) {
        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC:
            //DlgVSBCFeedback::ShowVirtualSBCFeedback(NULL);
            //X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_SBC_VIRTUAL;
            g_dwSBCType = 0;
            return 0;

        case  X_CONTROLLER_HOST_BRIDGE_HOSTTYPE_USB_SBC:
            SBCUSB_Close();
            g_dwSBCType = 0;
            return 0;

        default:
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
