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
#include "Exe.h"
#include "Xbe.h"

// ******************************************************************
// * constants
// ******************************************************************
#define RECENT_XBE_SIZE 10
#define RECENT_EXE_SIZE 10

// ******************************************************************
// * AutoConvert methods
// ******************************************************************
enum EnumAutoConvert
{
    AUTO_CONVERT_MANUAL       = 0,
    AUTO_CONVERT_XBE_PATH     = 1,
    AUTO_CONVERT_WINDOWS_TEMP = 2
};

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
        void SaveXbe(const char *x_filename);
        void SaveXbeAs();

        // ******************************************************************
        // * Exe operations
        // ******************************************************************
        void ImportExe(const char *x_filename);
        bool ConvertToExe(const char *x_filename, bool x_bVerifyIfExists);

        // ******************************************************************
        // * start emulation (converting to .exe if not done already)
        // ******************************************************************
        void StartEmulation(EnumAutoConvert x_bAutoConvert);

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
        // * after an xbe is loaded, some things must be updated
        // ******************************************************************
        void XbeLoaded();

        // ******************************************************************
        // * refresh the logo in the main window
        // ******************************************************************
        void LoadLogo();

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
        // * drawing information
        // ******************************************************************
        HDC         m_BackDC;
        HDC         m_LogoDC;
        HBITMAP     m_OrigBmp;
        HBITMAP     m_OrigLogo;
        HBITMAP     m_BackBmp;
        HBITMAP     m_LogoBmp;

        // ******************************************************************
        // * Xbe/Exe objects
        // ******************************************************************
        Xbe        *m_Xbe;
        Exe        *m_Exe;

        // ******************************************************************
        // * changes remembered for internal purposes
        // ******************************************************************
        bool        m_bXbeChanged;
        bool        m_bExeChanged;

        // ******************************************************************
        // * cached filenames
        // ******************************************************************
        char       *m_XbeFilename;
        char       *m_ExeFilename;

        // ******************************************************************
        // * should emulation always auto-create the .exe?
        // ******************************************************************
        EnumAutoConvert m_AutoConvertToExe;

        // ******************************************************************
        // * Recent Xbe files
        // ******************************************************************
        int         m_dwRecentXbe;
        char       *m_szRecentXbe[RECENT_XBE_SIZE];

        // ******************************************************************
        // * Recent Exe files
        // ******************************************************************
        int         m_dwRecentExe;
        char       *m_szRecentExe[RECENT_EXE_SIZE];

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
        // * debug output filenames
        // ******************************************************************
        char       *m_CxbxDebugFilename;
        char       *m_KrnlDebugFilename;
};

#endif