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
#include "EmuAlloc.h"
#include "DbgConsole.h"
#include "ResourceTracker.h"

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
static void EnableTracker(ResourceTracker &trackTotal, ResourceTracker &trackDisable, int a, int b, bool enable)
{
    int v=0;

    trackTotal.Lock();

    RTNode *cur = trackTotal.getHead();

    for(v=0;v<a;v++)
    {
        if(cur == NULL || (cur->pNext == NULL))
            break;

        cur = cur->pNext;
    }

    if((a == v) && (cur != NULL) && (cur->pNext != NULL))
    {
        for(;a<=b;a++)
        {
            if((cur == NULL) || (cur->pNext == NULL))
                break;

            if(enable)
            {
                trackDisable.remove(cur->pResource);
            }
            else
            {
                trackDisable.insert(cur->pResource);
            }

            printf("CxbxDbg: %.02d (0x%.08X) %s\n", a, cur->pResource, enable ? "enabled" : "disabled");

            cur = cur->pNext;
        }
    }
    else
    {
        printf("CxbxDbg: # out of range\n");
    }

    trackTotal.Unlock();

    return;
}
#endif

void DbgConsole::ParseCommand()
{
    printf("\n");

    char szCmd[32];

    szCmd[0] = '\0';

    sscanf(m_szInput, "%s", szCmd);

    // TODO: as command list grows, turn into static string/ptr lookup

    if(stricmp(szCmd, "h") == 0 || stricmp(szCmd, "help") == 0)
    {
        printf("CxbxDbg: \n");
        printf("CxbxDbg: Cxbx Debug Command List:\n");
        printf("CxbxDbg: \n");
        printf("CxbxDbg:  Help      [H]     : Show Command List\n");
        printf("CxbxDbg:  Quit/Exit [Q]     : Stop Emulation\n");
        printf("CxbxDbg:  Trace     [T]     : Toggle Debug Trace\n");
        printf("CxbxDbg:  ListVB    [LVB]   : List Active Vertex Buffers\n");
        printf("CxbxDbg:  DisableVB [DVB #] : Disable Active Vertex Buffer(s)\n");
        printf("CxbxDbg:  EnableVB  [EVB #] : Enable Active Vertex Buffer(s)\n");
        printf("CxbxDbg:  ListPB    [LPB]   : List Active Push Buffers\n");
        printf("CxbxDbg:  DisablePB [DPB #] : Disable Active Push Buffer(s)\n");
        printf("CxbxDbg:  EnablePB  [EPB #] : Enable Active Push Buffer(s)\n");
        #ifdef _DEBUG_ALLOC
        printf("CxbxDbg:  DumpMem   [DMEM]  : Dump the heap allocation tracking table\n");
        #endif // _DEBUG_ALLOCC
        printf("CxbxDbg:  CLS\n");
        printf("CxbxDbg: \n");
        printf("CxbxDbg: # denotes parameter of form [#] or [#-#]\n");
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
            int v=0;
            
            g_VBTrackTotal.Lock();

            RTNode *cur = g_VBTrackTotal.getHead();

            while(cur != NULL && cur->pNext != NULL)
            {
                bool enabled = !g_VBTrackDisable.exists(cur->pResource);

                printf("CxbxDbg: %.02d : 0x%.08X (%s)\n", v++, cur->pResource, enabled ? "enabled" : "disabled");

                cur = cur->pNext;
            }

            g_VBTrackTotal.Unlock();
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_VB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "dvb") == 0 || stricmp(szCmd, "DisableVB") == 0)
    {
        #ifdef _DEBUG_TRACK_VB
        {
            int n=0, m=0;

            int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
            
            if(c == 1)
            {
                EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, n, false);
            }
            else if(c == 2)
            {
                EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, m, false);
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
            int n=0, m=0;

            int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
            
            if(c == 1)
            {
                EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, n, true);
            }
            else if(c == 2)
            {
                EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, m, true);
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
    else if(stricmp(szCmd, "lpb") == 0 || stricmp(szCmd, "ListPB") == 0)
    {
        #ifdef _DEBUG_TRACK_PB
        {
            int v=0;

            g_PBTrackTotal.Lock();

            RTNode *cur = g_PBTrackTotal.getHead();

            while(cur != NULL && cur->pNext != NULL)
            {
                bool enabled = !g_PBTrackDisable.exists(cur->pResource);

                printf("CxbxDbg: %.02d : 0x%.08X (%s)\n", v++, cur->pResource, enabled ? "enabled" : "disabled");

                cur = cur->pNext;
            }

            g_PBTrackTotal.Unlock();
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_PB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "dpb") == 0 || stricmp(szCmd, "DisablePB") == 0)
    {
        #ifdef _DEBUG_TRACK_PB
        {
            int n=0, m=0;

            int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
            
            if(c == 1)
            {
                EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, n, false);
            }
            else if(c == 2)
            {
                EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, m, false);
            }
            else
            {
                printf("CxbxDbg: Syntax Incorrect (dpb #)\n");
            }
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_PB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "epb") == 0 || stricmp(szCmd, "EnablePB") == 0)
    {
        #ifdef _DEBUG_TRACK_PB
        {
            int n=0, m=0;

            int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);

            if(c == 1)
            {
                EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, n, true);
            }
            else if(c == 2)
            {
                EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, m, true);
            }
            else
            {
                printf("CxbxDbg: Syntax Incorrect (dpb #)\n");
            }
        }
        #else
        printf("CxbxDbg: _DEBUG_TRACK_PB is not defined!\n");
        #endif
    }
    else if(stricmp(szCmd, "cls") == 0)
    {
        // clear screen using system call
        system("cls");
    }
    #ifdef _DEBUG_ALLOC
    else if(stricmp(szCmd, "dmem") == 0 || stricmp(szCmd, "DumpMem") == 0)
    {
        int Full;
        int c = sscanf(m_szInput, "%*s %d", &Full);
        CxbxAllocDump(Full != 0);
    }
    #endif // _DEBUG_ALLOC
    else
    {
        printf("CxbxDbg: Cmd \"%s\" not recognized!\n", szCmd);
    }
}
