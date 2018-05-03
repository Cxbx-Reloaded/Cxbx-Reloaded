// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->WndMain.h
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
#include "Common/Xbe.h"

// ******************************************************************
// * constants
// ******************************************************************
#define RECENT_XBE_SIZE 10

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
        static void SuggestFilename(const char *x_orig_filename, char *x_filename, char x_extension[4]);

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

		// ******************************************************************
		// * crash monitoring wrapper function
		// ******************************************************************
		static DWORD WINAPI CrashMonitorWrapper(LPVOID lpVoid);

		// ******************************************************************
		// * crash monitoring function thread
		// ******************************************************************
		void CrashMonitor();

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
		HBITMAP		m_GameLogoBMP;
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

        // ******************************************************************
        // * cached filenames
        // ******************************************************************
        char       *m_XbeFilename;

        // ******************************************************************
        // * cached child window handle
        // ******************************************************************
        HWND        m_hwndChild;

        // ******************************************************************
        // * Recent Xbe files
        // ******************************************************************
        int         m_dwRecentXbe;
        char       *m_szRecentXbe[RECENT_XBE_SIZE];

        // ******************************************************************
        // * is this window fully initialized?
        // ******************************************************************
        bool        m_bCreated;

        // ******************************************************************
        // * current debug mode type
        // ******************************************************************
        DebugMode   m_CxbxDebug;
        DebugMode   m_KrnlDebug;

		// ******************************************************************
		// * LLE flags
		// ******************************************************************
		int         m_FlagsLLE;

		// ******************************************************************
		// * XInput Enabled Flag
		// ******************************************************************		
		int			m_XInputEnabled;

		// ******************************************************************
		// * Hack Flags
		// ******************************************************************	
		int		m_DisablePixelShaders;
		int		m_UncapFramerate;
		int		m_UseAllCores;
		int		m_SkipRdtscPatching;
		int     m_ScaleViewport;

        // ******************************************************************
        // * debug output filenames
        // ******************************************************************
        char       *m_CxbxDebugFilename;
        char       *m_KrnlDebugFilename;

};

#endif