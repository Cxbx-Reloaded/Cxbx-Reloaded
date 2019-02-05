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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "common\Settings.hpp" // for g_Settings

#include "DlgAudioConfig.h"
#include "resource/ResCxbx.h"

#include <dsound.h>

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgAudioConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*! directsound audio devices procedure*/
static BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext);
/*! refresh UI based on selected audio adapter */
static VOID RefreshAudioAdapter();

/*! audio configuration */
static Settings::s_audio g_XBAudio;
/*! changes flag */
static BOOL g_bHasChanges = FALSE;
/*! handle to audio adapter list window */
static HWND g_hAudioAdapter = nullptr;

VOID ShowAudioConfig(HWND hwnd)
{
    /*! reset changes flag */
    g_bHasChanges = FALSE;

    /*! retrieve audio configuration */
    g_XBAudio = g_Settings->m_audio;

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

    GUID curGUID = g_XBAudio.adapterGUID;

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

                DirectSoundEnumerate(&DSEnumProc, NULL);
            }

            /*! refresh UI based on currently selected audio adapter */
            RefreshAudioAdapter();

            /*! check appropriate options */
            {
                SendMessage(GetDlgItem(hWndDlg, IDC_AC_PCM), BM_SETCHECK, (WPARAM)g_XBAudio.codec_pcm, 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_AC_XADPCM), BM_SETCHECK, (WPARAM)g_XBAudio.codec_xadpcm, 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_AC_UNKNOWN_CODEC), BM_SETCHECK, (WPARAM)g_XBAudio.codec_unknown, 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_AC_MUTE_ON_UNFOCUS_DISABLE), BM_SETCHECK, (WPARAM)g_XBAudio.mute_on_unfocus==0, 0);
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

                    /*! save PCM/XADPCM/UnknownCodec options */
                    {
                        LRESULT lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_PCM), BM_GETCHECK, 0, 0);

                        g_XBAudio.codec_pcm = (lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_XADPCM), BM_GETCHECK, 0, 0);

                        g_XBAudio.codec_xadpcm = (lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_UNKNOWN_CODEC), BM_GETCHECK, 0, 0);

                        g_XBAudio.codec_unknown = (lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_AC_MUTE_ON_UNFOCUS_DISABLE), BM_GETCHECK, 0, 0);

                        g_XBAudio.mute_on_unfocus = (lRet != BST_CHECKED);
                    }

                    /*! save audio configuration */
                    g_Settings->m_audio = g_XBAudio;

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
        GUID oldGUID = g_XBAudio.adapterGUID;

        DWORD dwAudioAdapter = (DWORD)SendMessage(g_hAudioAdapter, CB_GETCURSEL, 0, 0);
        
        LPGUID pGUID = (LPGUID)SendMessage(g_hAudioAdapter, CB_GETITEMDATA, dwAudioAdapter, 0);

        GUID binGUID;

        // Check if pGUID doesn't have CB_ERR. (source of cause to crash)
        if (pGUID != nullptr && pGUID != (LPGUID)CB_ERR) {
            binGUID = *pGUID;
        }
        else {
            binGUID = { 0 };
        }

        if(binGUID != oldGUID) {
            g_bHasChanges = TRUE;

            g_XBAudio.adapterGUID = binGUID;
        }

        // Force save default audio device if selected audio device is invalid.
        if (pGUID == (LPGUID)CB_ERR) {
            SendMessage(g_hAudioAdapter, CB_SETCURSEL, 0, 0);
            g_Settings->m_audio = g_XBAudio;
            MessageBox(nullptr, "Your selected audio adapter is invalid,\n"
                                "reverting to default audio adapter.", "Cxbx-Reloaded", MB_OK | MB_ICONEXCLAMATION);
        }
    }
}
