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
#ifndef WNDMAIN_H
#define WNDMAIN_H

#include "Wnd.h"
#include "common\xbe\Xbe.h"

#include <thread>

// ******************************************************************
// * constants
// ******************************************************************
#define RECENT_XBE_LIST_MAX 10

typedef struct _Crash_Manager_Data {
	LPVOID  pWndMain;
	DWORD   dwChildProcID;
} Crash_Manager_Data;

// ******************************************************************
// * class : WndMain
// ******************************************************************
class WndMain : public Wnd
{
    public:
        WndMain(HINSTANCE x_hInstance);
       ~WndMain();

        // ******************************************************************
        // * window message procedure
        // ******************************************************************
        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // ******************************************************************
        // * Xbe operations
        // ******************************************************************
        void OpenXbe(const char *x_filename);
        void CloseXbe();
        void OpenMRU(int mru);
		void OpenDashboard();
        void SaveXbe(const char *x_filename);
        void SaveXbeAs();

        // ******************************************************************
        // * start emulation
        // ******************************************************************
        void StartEmulation(HWND hwndParent, DebuggerState LocalDebuggerState = debuggerOff);

        // ******************************************************************
        // * stop emulation (close existing child window)
        // ******************************************************************
        void StopEmulation();

        // ******************************************************************
        // * accessor
        // ******************************************************************
        bool isCreated() { return m_bCreated; }

        // ******************************************************************
        // * suggest appropriate filename based on input
        // ******************************************************************
        static void SuggestFilename(const char *x_orig_filename, char *x_filename, const char x_extension[4]);

    private:
		// ******************************************************************
		// * resize the main window for either GUI or emulation
		// ******************************************************************
		void ResizeWindow(HWND hwnd, bool bForGUI);

        // ******************************************************************
        // * after an xbe is loaded, some things must be updated
        // ******************************************************************
        void XbeLoaded();

        // ******************************************************************
        // * refresh the logo in the main window
        // ******************************************************************
        void LoadLogo();

		// ******************************************************************
		// * refresh the game logo in the main window
		// ******************************************************************
		void LoadGameLogo();

        // ******************************************************************
        // * refresh all menus (checks, enabled, disabled, etc)
        // ******************************************************************
        void RefreshMenus();

        // ******************************************************************
        // * allocate / deallocate debug consoles
        // ******************************************************************
        void UpdateDebugConsoles();

        // ******************************************************************
        // * update menu containing list of recent files
        // ******************************************************************
        void UpdateRecentFiles();

		// ******************************************************************
		// * update title bar caption with xbe name and FPS/MSF
		// ******************************************************************
		void UpdateCaption();
		void RefreshAllStatus();
		void UpdateFpsStatus();
		void UpdateLogStatus();

		// ******************************************************************
		// * crash monitoring wrapper function
		// ******************************************************************
		static DWORD CrashMonitorWrapper(LPVOID lpParam);

		// ******************************************************************
		// * crash monitoring function thread
		// ******************************************************************
		void CrashMonitor(DWORD dwChildProcID);

		// ******************************************************************
		// * Debugger monitoring function thread
		// ******************************************************************
		static DWORD DebuggerMonitor(LPVOID lpVoid);

		// ******************************************************************
		// * Close debugger monitoring function
		// ******************************************************************
		void DebuggerMonitorClose();

		// ******************************************************************
		// * clear registry values and keys
		// ******************************************************************
		void InitializeSettings();

		// ******************************************************************
		// * draw Xbox LED bitmap
		// ******************************************************************
		void DrawLedBitmap(HWND hwnd, bool boolbDefault);

		// ******************************************************************
		// * drawing information
		// ******************************************************************
		HDC         m_SplashDC;
		HDC         m_LogoDC;
		HDC         m_GameLogoDC;
		HDC         m_LedDC;
		HBITMAP     m_OrigBmp;
		HBITMAP     m_OrigLogo;
		HBITMAP     m_OrigGameLogo;
		HBITMAP     m_OriLed;
		HBITMAP     m_SplashBmp;
		HBITMAP     m_LogoBmp;
		HBITMAP     m_GameLogoBMP;
		HBITMAP     m_LedBmp;
		HBRUSH      m_Brushes[4];
		HPEN        m_Pens[4];
		int         m_xBmp, m_yBmp;
        HBRUSH      m_BackgroundColor;

        // ******************************************************************
        // * Xbe objects
        // ******************************************************************
        Xbe        *m_Xbe;

        // ******************************************************************
        // * changes remembered for internal purposes
        // ******************************************************************
        bool        m_bXbeChanged;
        bool        m_bIsStarted;
        size_t      m_iIsEmulating;

        // ******************************************************************
        // * cached filenames
        // ******************************************************************
        char        m_XbeFilename[MAX_PATH];

        // ******************************************************************
        // * cached window, process, and thread handle
        // ******************************************************************
        HWND        m_hwndChild;
        HANDLE      m_hDebuggerProc;
        std::thread m_hDebuggerMonitorThread;

        // ******************************************************************
        // * Recent Xbe files
        // ******************************************************************
        int         m_dwRecentXbe;

        // ******************************************************************
        // * is this window fully initialized?
        // ******************************************************************
        bool        m_bCreated;

		// ******************************************************************
		// * Previous GUI window location (before start emulation)
		// ******************************************************************
		POINT       m_prevWindowLoc;

		// ******************************************************************
		// * Kernel process status
		// ******************************************************************
		float               m_FPS_status;
		float               m_MSpF_status;
		union {
			UINT            m_LedSeq_status_block;
			UCHAR           m_LedSeq_status[4];
		};
		UINT                m_FlagsLLE_status;
		bool                m_LogKrnl_status;
};

#endif
