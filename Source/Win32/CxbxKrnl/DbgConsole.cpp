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

#ifdef _DEBUG_TRACK_VB
static void EnableVB(int n, bool enable)
{
    using namespace XTL;

    int v=0;

    VBNode *cur = g_VBTrackTotal.getHead();

    for(v=0;v<n;v++)
    {
        if(cur == NULL || (cur->next == NULL))
            break;

        cur = cur->next;
    }

    if(n == v && (cur != NULL) && (cur->next != NULL))
    {
        if(enable)
        {
            g_VBTrackDisable.remove(cur->vb);
        }
        else
        {
            g_VBTrackDisable.insert(cur->vb);
        }

        printf("CxbxDbg: %.02d (0x%.08X) %s\n", n, cur->vb, enable ? "enabled" : "disabled");
    }
    else
    {
        printf("CxbxDbg: # out of range\n");
    }

    return;
}
#endif

void DbgConsole::ParseCommand()
{
    printf("\n");

    char szCmd[32];

    sscanf(m_szInput, "%s", szCmd);

    // TODO: as command list grows, turn into static string/ptr lookup

    if(stricmp(szCmd, "h") == 0 || stricmp(szCmd, "help") == 0)
    {
        printf("CxbxDbg: \n");
        printf("CxbxDbg: Cxbx Debug Command List:\n");
        printf("CxbxDbg: \n");
        printf("CxbxDbg:  HELP        (H)\n");
        printf("CxbxDbg:  QUIT        (Q or EXIT)\n");
        printf("CxbxDbg:  TRACE       (T)\n");
        printf("CxbxDbg:  ListVB      (LVB)\n");
        printf("CxbxDbg:  DisableVB # (DVB #)\n");
        printf("CxbxDbg:  EnableVB #  (EVB #)\n");
        printf("CxbxDbg:  CLS\n");
        printf("CxbxDbg: \n");
    }
    else if(stricmp(szCmd, "q") == 0 || stricmp(szCmd, "quit") == 0 || stricmp(szCmd, "exit") == 0)
    {
        printf("CxbxDbg: Goodbye...\n");
        EmuCleanup(NULL);
    }
    else if(stricmp(szCmd, "t") == 0 || stricmp(szCmd, "trace") == 0)
    {
        g_bPrintfOn = !g_bPrintfOn;
        printf("CxbxDbg: Trace is now %s\n", g_bPrintfOn ? "ON" : "OFF");
	}
    else if(stricmp(szCmd, "lvb") == 0 || stricmp(szCmd, "ListVB") == 0)
    {
        #ifdef _DEBUG_TRACK_VB
        {
            using namespace XTL;

            int v=0;
            
            VBNode *cur = g_VBTrackTotal.getHead();

            while(cur != NULL && cur->next != NULL)
            {
                bool enabled = !g_VBTrackDisable.exists(cur->vb);

                printf("CxbxDbg: %.02d : 0x%.08X (%s)\n", v++, cur->vb, enabled ? "enabled" : "disabled");

                cur = cur->next;
            }
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_VB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "dvb") == 0 || stricmp(szCmd, "DisableVB") == 0)
    {
        #ifdef _DEBUG_TRACK_VB
        {
            using namespace XTL;

            int n=0;

            if(sscanf(m_szInput, "%*s %d", &n) == 1)
            {
                EnableVB(n, false);
            }
            else
            {
                printf("CxbxDbg: Syntax Incorrect (dvb #)\n");
            }
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_VB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "evb") == 0 || stricmp(szCmd, "EnableVB") == 0)
    {
        #ifdef _DEBUG_TRACK_VB
        {
            using namespace XTL;

            int n=0;

            if(sscanf(m_szInput, "%*s %d", &n) == 1)
            {
                EnableVB(n, true);
            }
            else
            {
                printf("CxbxDbg: Syntax Incorrect (dvb #)\n");
            }
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_VB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "cls") == 0)
    {
        // clear screen using system call
        system("cls");
    }
    else
    {
        printf("CxbxDbg: Cmd \"%s\" not recognized!\n", szCmd);
    }
}
