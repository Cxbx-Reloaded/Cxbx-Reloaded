// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->Wnd.h
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
#ifndef WND_H
#define WND_H

#include "Error.h"

#include <windows.h>

// ******************************************************************
// * class : Wnd
// ******************************************************************
class Wnd : public Error
{
    public:
        // ******************************************************************
        // * Constructor
        // ******************************************************************
        Wnd(HINSTANCE x_hInstance);

        // ******************************************************************
        // * Deconstructor
        // ******************************************************************
       ~Wnd();

        // ******************************************************************
        // * Process Window Messages
        // ******************************************************************
        bool    ProcessMessages();

        // ******************************************************************
        // * Forward WndProc to thiscall member function
        // ******************************************************************
        static  LRESULT CALLBACK WndProcForward(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // ******************************************************************
        // * Window Message Handling Procedure
        // ******************************************************************
        virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // ******************************************************************
        // * Retrieve this window's handle
        // ******************************************************************
        HWND GetHwnd() { return m_hwnd; }

    protected:
        // ******************************************************************
        // * Module's Instance Handle
        // ******************************************************************
        HINSTANCE   m_hInstance;

        // ******************************************************************
        // * Module's Window Handle
        // ******************************************************************
        HWND        m_hwnd;

        // ******************************************************************
        // * Etc
        // ******************************************************************
        ATOM        m_class;
        const char *m_classname;
        const char *m_wndname;
        UINT        m_clsstyle;
        DWORD       m_wndstyle;
        int         m_x, m_y, m_w, m_h;
        HWND        m_parent;
        HBRUSH      m_background;
        bool        m_initialized;

    private:
};

#endif