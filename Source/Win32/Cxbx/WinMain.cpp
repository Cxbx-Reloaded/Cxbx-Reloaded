// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->WinMain.cpp
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
#include "Emu.h"
#include "WndMain.h"
#include "EmuShared.h"
#include "EmuExe.h"

// ******************************************************************
// * func : WinMain
// ******************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if(!EmuVerifyVersion(_CXBX_VERSION))
    {
        MessageBox(NULL, "Cxbx.dll is the incorrect version", "Cxbx", MB_OK);
        return 1;
    }

    EmuShared::Init();

    WndMain *MainWindow = new WndMain(hInstance);

    while(!MainWindow->isCreated() && MainWindow->ProcessMessages())
        Sleep(10);

    if(__argc > 1 && MainWindow->GetError() == 0)
    {
        MainWindow->OpenXbe(__argv[1]);

        MainWindow->StartEmulation(AUTO_CONVERT_WINDOWS_TEMP);
    }

    while(MainWindow->GetError() == 0 && MainWindow->ProcessMessages())
        Sleep(10);

    if(MainWindow->GetError() != 0)
        MessageBox(NULL, MainWindow->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);

    delete MainWindow;

    EmuShared::Cleanup();

    return 0;
}