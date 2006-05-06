// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->WndAbout.h
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
#ifndef WNDABOUT_H
#define WNDABOUT_H

#include "Wnd.h"

// ******************************************************************
// * class : WndAbout
// ******************************************************************
class WndAbout : public Wnd
{
    public:
        // ******************************************************************
        // * Constructor
        // ******************************************************************
        WndAbout(HINSTANCE x_hInstance, HWND x_parent);

        // ******************************************************************
        // * Deconstructor
        // ******************************************************************
       ~WndAbout();

        // ******************************************************************
        // * Window Message Handling Procedure
        // ******************************************************************
        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        // ******************************************************************
        // * Drawing Data
        // ******************************************************************
        HDC         m_BackDC;
        HBITMAP     m_OrigBmp;
        HBITMAP     m_BackBmp;
        HFONT		m_hFont;
};

#endif