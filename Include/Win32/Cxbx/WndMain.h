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

// ******************************************************************
// * class : WndMain
// ******************************************************************
class WndMain : public Wnd
{
    public:

        WndMain(HINSTANCE x_hInstance);
       ~WndMain();

        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:

        void SuggestFilename(const char *x_orig_filename, char *x_filename, char x_extension[4]);

        void XbeLoaded();           // after an xbe is loaded, some stuff needs to update

        void LoadLogo();            // refresh the logo in the main window

        void UpdateDebugConsoles(); // Allocate / Deallocate debug consoles as per configuration

        bool ConvertToExe();

        void SaveXbe(const char *x_filename);
        void SaveXbeAs();

        void CloseXbe();

        HDC         m_back_dc;
        HDC         m_logo_dc;

        HBITMAP     m_orig_bmp;
        HBITMAP     m_orig_logo;

        HBITMAP     m_back_bmp;
        HBITMAP     m_logo_bmp;

        Xbe        *m_xbe;
        Exe        *m_exe;

        char       *m_XbeFilename;
        char       *m_ExeFilename;

        bool        m_xbe_changed;
        bool        m_exe_changed;

        enum DebugMode m_CxbxDebug;
        enum DebugMode m_KrnlDebug;

        char       *m_CxbxDebugFilename;
        char       *m_KrnlDebugFilename;
};

#endif