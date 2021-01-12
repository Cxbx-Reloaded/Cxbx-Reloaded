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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef WNDSBCFEEDBACK_H
#define WNDSBCFEEDBACK_H

#include "Wnd.h"

// ******************************************************************
// * constants
// ******************************************************************

// ******************************************************************
// * class : WndSBCFeedback
// ******************************************************************
class WndSBCFeedback : public Wnd
{
    public:
        WndSBCFeedback(HINSTANCE x_hInstance);
       ~WndSBCFeedback();

        // ******************************************************************
        // * window message procedure
        // ******************************************************************
        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // ******************************************************************
        // * accessor
        // ******************************************************************
        bool isCreated() { return m_bCreated; }

    private:
		// ******************************************************************
		// * resize the main window for either GUI or emulation
		// ******************************************************************
		void ResizeWindow(HWND hwnd, bool bForGUI);

        // ******************************************************************
        // * refresh all menus (checks, enabled, disabled, etc)
        // ******************************************************************
        void RefreshMenus();

		// ******************************************************************
		// * update title bar caption with xbe name and FPS/MSF
		// ******************************************************************
		void UpdateCaption();

		// ******************************************************************
		// * crash monitoring wrapper function
		// ******************************************************************
		static DWORD WINAPI CrashMonitorWrapper(LPVOID lpVoid);

		// ******************************************************************
		// * clear registry values and keys
		// ******************************************************************
		void InitializeSettings();

		// ******************************************************************
		// * drawing information
		// ******************************************************************

        // ******************************************************************
        // * changes remembered for internal purposes
        // ******************************************************************
        bool        m_bIsStarted;

        // ******************************************************************
        // * cached dialog window handle
        // ******************************************************************
        HWND        m_hwndDlg;

        // ******************************************************************
        // * is this window fully initialized?
        // ******************************************************************
        bool        m_bCreated;

};

#endif
