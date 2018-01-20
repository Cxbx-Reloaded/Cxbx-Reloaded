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
// *   Cxbx->Cxbx->DlgAudioConfig.cpp
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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "CxbxKrnl/EmuShared.h"

#include "DlgAudioConfig.h"
#include "ResCxbx.h"

#include <dsound.h>

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgAudioConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*! directsound audio devices procedure*/
static BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext);
/*! refresh UI based on selected audio adapter */
static VOID RefreshAudioAdapter();

/*! audio configuration */
static XBAudio g_XBAudio;
/*! changes flag */
static BOOL g_bHasChanges = FALSE;
/*! handle to audio adapter list window */
static HWND g_hAudioAdapter = nullptr;

VOID ShowAudioConfig(HWND hwnd)
{
    /*! reset changes flag */
    g_bHasChanges = FALSE;

    /*! retrieve audio configuration */
    g_EmuShared->GetXBAudio(&g_XBAudio);

    /*! show dialog box */
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_AUDIO_CFG), hwnd, DlgAudioConfigProc);
}

BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
{
    /*! enumerate audio adapters */

    LRESULT counter = SendMessage(g_hAudioAdapter, CB_ADDSTRING, 0, (LPARAM)lpszDesc);
    SendMessage(g_hAudioAdapter, CB_SETITEMDATA, counter, (LPARAM)lpGUID);

    GUID binGUID;

    if (lpGUID != nullptr) {
        binGUID = *lpGUID;
    } else {
        binGUID = { 0 };
    }

    GUID curGUID = g_XBAudio.GetAudioAdapter();

    /*! activate configured audio adapter */
    if (curGUID == binGUID) {
        SendMessage(g_hAudioAdapter, CB_SETCURSEL, counter, 0);
    }

    return TRUE;
}

INT_PTR CALLBACK DlgAudioConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            g_hAudioAdapter  = GetDlgItem(hWndDlg, IDC_AC_AUDIO_ADAPTER);

            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

            /*! enumerate audio adapters */
            {

                SendMessage(g_hAudioAdapter, CB_RESETCONTENT, 0, 0);

                XTL::DirectSoundEnumerate(&DSEnumProc, NULL);
            }

            /*! refresh UI based on currently selected audio adapter */
            RefreshAudioAdapter();

            /*! check appropriate options */
            {
                SendMessage(GetDlgItem(hWndDlg, IDC_AC_LEGACY_AUDIO_HACK), BM_SETCHECK, (WPARAM)g_XBAudio.GetLegacyAudioHack(), 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_AC_PCM), BM_SETCHECK, (WPARAM)g_XBAudio.GetPCM(), 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_AC_XADPCM), BM_SETCHECK, (WPARAM)g_XBAudio.GetXADPCM(), 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_AC_UNKNOWN_CODEC), BM_SETCHECK, (WPARAM)g_XBAudio.GetUnknownCodec(), 0);
            }
        }
        break;

        case WM_CLOSE:
        {
            /*! if changes have been made, check if the user wants to save them */
            if(g_bHasChanges)
            {
                int ret = MessageBox(hWndDlg, "Do you wish to apply your changes?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNOCANCEL);

                switch(ret)
                {
                    case IDYES:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_AC_ACCEPT, 0);
                        break;
                    case IDNO:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_AC_CANCEL, 0);
                        break;
                }
                break;
            }

            PostMessage(hWndDlg, WM_COMMAND, IDC_AC_CANCEL, 0);
        }
        break;

        case WM_COMMAND:
        {
            HWND hWndButton = GetDlgItem(hWndDlg, LOWORD(wParam));

            switch(LOWORD(wParam))
            {
                case IDC_AC_CANCEL:
                    EndDialog(hWndDlg, wParam);
                    break;

                case IDC_AC_ACCEPT:
                {

                    /*! save LegacyAudioHack/PCM/XADPCM/UnknownCodec options */
                    {
                        LRESULT lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_LEGACY_AUDIO_HACK), BM_GETCHECK, 0, 0);

                        g_XBAudio.SetLegacyAudioHack(lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_PCM), BM_GETCHECK, 0, 0);

                        g_XBAudio.SetPCM(lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_XADPCM), BM_GETCHECK, 0, 0);

                        g_XBAudio.SetXADPCM(lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_UNKNOWN_CODEC), BM_GETCHECK, 0, 0);

                        g_XBAudio.SetUnknownCodec(lRet == BST_CHECKED);
                    }

                    /*! save audio configuration */
                    g_EmuShared->SetXBAudio(&g_XBAudio);

                    EndDialog(hWndDlg, wParam);
                }
                break;

                case IDC_AC_AUDIO_ADAPTER:
                {
                    if(HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        RefreshAudioAdapter();
                        break;
                    }
                }
                break;
            }
        }
        break;
    }
    return FALSE;
}

VOID RefreshAudioAdapter()
{
    /*! save configured audio adapter */
    {
        GUID oldGUID = g_XBAudio.GetAudioAdapter();

        DWORD dwAudioAdapter = (DWORD)SendMessage(g_hAudioAdapter, CB_GETCURSEL, 0, 0);
        
        LPGUID pGUID = (LPGUID)SendMessage(g_hAudioAdapter, CB_GETITEMDATA, dwAudioAdapter, 0);

        GUID binGUID;

        if (pGUID) {
            binGUID = *pGUID;
        } else {
            binGUID = { 0 };
        }

        if(binGUID != oldGUID)
        {
            g_bHasChanges = TRUE;

            g_XBAudio.SetAudioAdapter(binGUID);
        }
    }
}
