// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DbgConsole.cpp
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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "Emu.h"
#include "DbgConsole.h"

#include <conio.h>

// prevent name collisions
namespace XTL
{
    #include "EmuXTL.h"
};

extern XTL::LPDIRECT3DDEVICE8 g_pD3DDevice8;  // Direct3D8 Device

DbgConsole::DbgConsole()
{
    m_cur = 0;

    printf("CxbxDbg> ");
    fflush(stdout);

    m_szInput[0] = '\0';
}

DbgConsole::~DbgConsole()
{
}

void DbgConsole::Process()
{
    // process all queued key presses
    while(_kbhit())
    {
        char c = _getche();

        if(c == '\r')
        {
            ParseCommand();

            printf("CxbxDbg> ");
            fflush(stdout);

            m_szInput[0] = '\0';
            m_cur = 0;
        }
        else if(c == '\b')
        {
            if(m_cur > 0)
            {
                printf(" \b");

                m_szInput[--m_cur] = '\0';
            }
            else
            {
                printf(" ");
            }
        }
        else
        {
            m_szInput[m_cur++] = c;
            m_szInput[m_cur] = '\0';
        }
    }
}

void DbgConsole::Reset()
{
    m_cur = 0;

    fflush(stdout);

    printf("\n");

    printf("CxbxDbg> ");
    fflush(stdout);
    
    m_szInput[0] = '\0';
}

void DbgConsole::ParseCommand()
{
    printf("\n");

    // TODO: as command list grows, turn into static string/ptr lookup

    if(stricmp(m_szInput, "h") == 0 || stricmp(m_szInput, "help") == 0)
    {
        printf("CxbxDbg: \n");
        printf("CxbxDbg: Cxbx Debug Command List:\n");
        printf("CxbxDbg: \n");
        printf("CxbxDbg:  HELP  (H)\n");
        printf("CxbxDbg:  QUIT  (Q or EXIT)\n");
        printf("CxbxDbg:  TRACE (T)\n");
        printf("CxbxDbg:  CLS\n");
        printf("CxbxDbg: \n");
    }
    else if(stricmp(m_szInput, "cls") == 0)
    {
        // clear screen using system call
        system("cls");
    }
    else if(stricmp(m_szInput, "q") == 0 || stricmp(m_szInput, "quit") == 0 || stricmp(m_szInput, "exit") == 0)
    {
        printf("CxbxDbg: Goodbye...\n");
        EmuCleanup(NULL);
    }
    else if(stricmp(m_szInput, "t") == 0 || stricmp(m_szInput, "trace") == 0)
    {
        g_bPrintfOn = !g_bPrintfOn;
        printf("CxbxDbg: Trace is now %s\n", g_bPrintfOn ? "ON" : "OFF");
    }
    else
    {
        printf("CxbxDbg: Cmd \"%s\" not recognized!\n", m_szInput);
    }
}
