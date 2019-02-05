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

#include <pcap.h>
#include "EmuShared.h"

#include "DlgNetworkConfig.h"
#include "resource/ResCxbx.h"

#include <Iphlpapi.h>

/*! handle to network adapter list window */
static HWND g_hNetworkAdapters = nullptr;
static Settings::s_network g_XBNetwork;

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgNetworkConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID ShowNetworkConfig(HWND hwnd)
{
	g_XBNetwork = g_Settings->m_network;
    /*! show dialog box */
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NETWORK_CFG), hwnd, DlgNetworkConfigProc);
}

VOID FetchNetworkInterfaces(HWND hwnd)
{
	static IP_ADAPTER_INFO AdapterInfo[128];
	static PIP_ADAPTER_INFO pAdapterInfo;
	ULONG dwBufferLength = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufferLength);
	if (dwStatus != ERROR_SUCCESS) {
		return;
	}

	pAdapterInfo = AdapterInfo;

	LRESULT counter = SendMessage(g_hNetworkAdapters, CB_ADDSTRING, 0, (LPARAM)"Disabled");
	SendMessage(g_hNetworkAdapters, CB_SETITEMDATA, counter, (LPARAM)"");
	SendMessage(g_hNetworkAdapters, CB_SETCURSEL, counter, 0);

	do {
		counter = SendMessage(g_hNetworkAdapters, CB_ADDSTRING, 0, (LPARAM)pAdapterInfo->Description);
		SendMessage(g_hNetworkAdapters, CB_SETITEMDATA, counter, (LPARAM)pAdapterInfo->AdapterName);

		if (strcmp(g_XBNetwork.adapter_name, pAdapterInfo->AdapterName) == 0) {
			SendMessage(g_hNetworkAdapters, CB_SETCURSEL, counter, 0);
		}

		pAdapterInfo = pAdapterInfo->Next;

	} while (pAdapterInfo);
}

INT_PTR CALLBACK DlgNetworkConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
			g_hNetworkAdapters = GetDlgItem(hWndDlg, IDC_NETWORK_ADAPTER);

            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

			SendMessage(g_hNetworkAdapters, CB_RESETCONTENT, 0, 0);
			FetchNetworkInterfaces(hWndDlg);
        }
        break;

        case WM_CLOSE:
        {
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
					HWND hNetworkAdapter = GetDlgItem(hWndDlg, IDC_NETWORK_ADAPTER);
					LRESULT lRet = SendMessage(hNetworkAdapter, CB_GETCURSEL, 0, 0);
					char* deviceName = nullptr;
					deviceName = (char*)SendMessage(hNetworkAdapter, CB_GETITEMDATA, (WPARAM)lRet, NULL);
					strncpy(g_XBNetwork.adapter_name, deviceName, std::size(g_XBNetwork.adapter_name));
					g_Settings->m_network = g_XBNetwork;
                    EndDialog(hWndDlg, wParam);
					break;
            }
        }
        break;
    }
    return FALSE;
}
