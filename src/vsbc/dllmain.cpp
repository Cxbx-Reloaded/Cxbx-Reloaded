// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DlgVirtualSBCFeedback.h"

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

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        inj_hModule = hModule;
        CreateThread(0, NULL, ThreadProc, (LPVOID)L"Window Title", NULL, NULL);
    case DLL_THREAD_ATTACH:
        //ShowVirtualSBCFeedback(NULL);
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;
}

