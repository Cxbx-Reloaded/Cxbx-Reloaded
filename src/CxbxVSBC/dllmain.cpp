// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //inj_hModule = hModule;
        //CreateThread(0, NULL, ThreadProc, (LPVOID)L"Window Title", NULL, NULL);
    case DLL_THREAD_ATTACH:
        //ShowVirtualSBCFeedback(NULL);
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;
}

