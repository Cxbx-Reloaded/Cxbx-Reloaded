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

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuAlloc.h"
#include "DbgConsole.h"
#include "ResourceTracker.h"
#include "EmuXTL.h"

#include <conio.h>

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

#if defined(_DEBUG_TRACK_VB) || defined(_DEBUG_TRACK_PB)
typedef enum _ETAction
{
    ETA_ENABLE  = 0,
    ETA_DISABLE = 1,
    ETA_SHOW    = 2
}
ETAction;

static void EnableTracker(ResourceTracker &trackTotal, ResourceTracker &tracker, int a, int b, ETAction action)
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

            if(action == ETA_ENABLE)
                printf("CxbxDbg: #%.02d (0x%.08X) enabled\n", a, cur->pResource);
            else if(action == ETA_DISABLE)
                printf("CxbxDbg: #%.02d (0x%.08X) disabled\n", a, cur->pResource);
            else if(action == ETA_SHOW)
                printf("CxbxDbg: #%.02d (0x%.08X) queued for show info..\n", a, cur->pResource);

            if(action == ETA_ENABLE)
            {
                tracker.remove(cur->pResource);
            }
            else
            {
                tracker.insert(cur->pResource);
            }

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

    if(_stricmp(szCmd, "h") == 0 || _stricmp(szCmd, "help") == 0)
    {
        printf("CxbxDbg: \n");
        printf("CxbxDbg: Cxbx Debug Command List:\n");
        printf("CxbxDbg: \n");
        printf("CxbxDbg:  Help            [H]     : Show Command List\n");
        printf("CxbxDbg:  Quit/Exit       [Q]     : Stop Emulation\n");
        printf("CxbxDbg:  Trace           [T]     : Toggle Debug Trace\n");

        #ifdef _DEBUG_TRACK_VB
        printf("CxbxDbg:  ListVB          [LVB]   : List Active Vertex Buffers\n");
        printf("CxbxDbg:  DisableVB       [DVB #] : Disable Active Vertex Buffer(s)\n");
        printf("CxbxDbg:  EnableVB        [EVB #] : Enable Active Vertex Buffer(s)\n");
        printf("CxbxDbg:  DumpStreamCache [DSC]   : Dumps the patched streams cache\n");
        #endif

        #ifdef _DEBUG_TRACK_PB
        printf("CxbxDbg:  ListPB          [LPB]   : List Active Push Buffers\n");
        printf("CxbxDbg:  ShowPB          [SPB #] : Show Push Buffer(s)\n");
        printf("CxbxDbg:  DisablePB       [DPB #] : Disable Push Buffer(s)\n");
        printf("CxbxDbg:  EnablePB        [EPB #] : Enable Push Buffer(s)\n");
        printf("CxbxDbg:  ClearPB         [CPB]   : Clear Push Buffer List\n");
        #endif

        #ifdef _DEBUG_ALLOC
        printf("CxbxDbg:  DumpMem         [DMEM]  : Dump the heap allocation tracking table\n");
        #endif // _DEBUG_ALLOCC

        printf("CxbxDbg:  CLS\n");
        printf("CxbxDbg: \n");
        printf("CxbxDbg: # denotes parameter of form [#] or [#-#]\n");
        printf("CxbxDbg: \n");
    }
    else if(_stricmp(szCmd, "q") == 0 || _stricmp(szCmd, "quit") == 0 || _stricmp(szCmd, "exit") == 0)
    {
        printf("CxbxDbg: Goodbye...\n");
        CxbxKrnlCleanup(NULL);
    }
    else if(_stricmp(szCmd, "t") == 0 || _stricmp(szCmd, "trace") == 0)
    {
        g_bPrintfOn = !g_bPrintfOn;
        printf("CxbxDbg: Trace is now %s\n", g_bPrintfOn ? "ON" : "OFF");
    }
    #ifdef _DEBUG_TRACK_VB
    else if(_stricmp(szCmd, "lvb") == 0 || _stricmp(szCmd, "ListVB") == 0)
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
    else if(_stricmp(szCmd, "dvb") == 0 || _stricmp(szCmd, "DisableVB") == 0)
    {
        int n=0, m=0;

        int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
        
        if(c == 1)
        {
            EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, n, ETA_DISABLE);
        }
        else if(c == 2)
        {
            EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, m, ETA_DISABLE);
        }
        else
        {
            printf("CxbxDbg: Syntax Incorrect (dvb #)\n");
        }
    }
    else if(_stricmp(szCmd, "evb") == 0 || _stricmp(szCmd, "EnableVB") == 0)
    {
        int n=0, m=0;

        int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
        
        if(c == 1)
        {
            EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, n, ETA_ENABLE);
        }
        else if(c == 2)
        {
            EnableTracker(g_VBTrackTotal, g_VBTrackDisable, n, m, ETA_ENABLE);
        }
        else
        {
            printf("CxbxDbg: Syntax Incorrect (dvb #)\n");
        }
    }
    else if(_stricmp(szCmd, "dsc") == 0 || _stricmp(szCmd, "DumpStreamCache") == 0)
    {
        XTL::VertexPatcher::DumpCache();
    }
    #endif
    #ifdef _DEBUG_TRACK_PB
    else if(_stricmp(szCmd, "lpb") == 0 || _stricmp(szCmd, "ListPB") == 0)
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
    else if(_stricmp(szCmd, "spb") == 0 || _stricmp(szCmd, "ShowPB") == 0)
    {
        int n=0, m=0;

        int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
        
        if(c == 1)
        {
            EnableTracker(g_PBTrackTotal, g_PBTrackShowOnce, n, n, ETA_SHOW);
        }
        else if(c == 2)
        {
            EnableTracker(g_PBTrackTotal, g_PBTrackShowOnce, n, m, ETA_SHOW);
        }
        else
        {
            printf("CxbxDbg: Syntax Incorrect (spb #)\n");
        }
    }
    else if(_stricmp(szCmd, "dpb") == 0 || _stricmp(szCmd, "DisablePB") == 0)
    {
        int n=0, m=0;

        int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);
        
        if(c == 1)
        {
            EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, n, ETA_DISABLE);
        }
        else if(c == 2)
        {
            EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, m, ETA_DISABLE);
        }
        else
        {
            printf("CxbxDbg: Syntax Incorrect (dpb #)\n");
        }
    }
    else if(_stricmp(szCmd, "epb") == 0 || _stricmp(szCmd, "EnablePB") == 0)
    {
        int n=0, m=0;

        int c = sscanf(m_szInput, "%*s %d-%d", &n, &m);

        if(c == 1)
        {
            EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, n, ETA_ENABLE);
        }
        else if(c == 2)
        {
            EnableTracker(g_PBTrackTotal, g_PBTrackDisable, n, m, ETA_ENABLE);
        }
        else
        {
            printf("CxbxDbg: Syntax Incorrect (dpb #)\n");
        }
    }
    else if(_stricmp(szCmd, "cpb") == 0 || _stricmp(szCmd, "ClearPB") == 0)
    {
        g_PBTrackTotal.clear();

        printf("CxbxDbg: Push Buffer List Cleared!\n");
    }
    #endif
    #ifdef _DEBUG_ALLOC
    else if(_stricmp(szCmd, "dmem") == 0 || _stricmp(szCmd, "DumpMem") == 0)
    {
        int Full;
        int c = sscanf(m_szInput, "%*s %d", &Full);
        CxbxAllocDump(Full != 0);
    }
    #endif // _DEBUG_ALLOC
    else if(_stricmp(szCmd, "cls") == 0)
    {
        // clear screen using system call
        system("cls");
    }
    else
    {
        printf("CxbxDbg: Cmd \"%s\" not recognized!\n", szCmd);
    }
}
