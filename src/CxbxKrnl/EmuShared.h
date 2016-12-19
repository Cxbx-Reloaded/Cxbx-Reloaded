// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuShared.h
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
#ifndef EMUSHARED_H
#define EMUSHARED_H

#include "Cxbx.h"
#include "Common/Win32/XBController.h"
#include "Common/Win32/XBVideo.h"

#include <memory.h>

// ******************************************************************
// * EmuShared : Shared memory
// ******************************************************************
class EmuShared : public Mutex
{
    public:
        // ******************************************************************
        // * Constructor / Deconstructor
        // ******************************************************************
        CXBXKRNL_API  EmuShared();
        CXBXKRNL_API ~EmuShared();

        // ******************************************************************
        // * Each process needs to call this to initialize shared memory
        // ******************************************************************
        CXBXKRNL_API static void Init();

        // ******************************************************************
        // * Each process needs to call this to cleanup shared memory
        // ******************************************************************
        CXBXKRNL_API static void Cleanup();

        // ******************************************************************
        // * Xbox Video Accessors
        // ******************************************************************
        CXBXKRNL_API void GetXBVideo(      XBVideo *video) { Lock(); memcpy(video, &m_XBVideo, sizeof(XBVideo)); Unlock(); }
        CXBXKRNL_API void SetXBVideo(const XBVideo *video) { Lock(); memcpy(&m_XBVideo, video, sizeof(XBVideo)); Unlock(); }

        // ******************************************************************
        // * Xbox Controller Accessors
        // ******************************************************************
        CXBXKRNL_API void GetXBController(      XBController *ctrl) { Lock(); memcpy(ctrl, &m_XBController, sizeof(XBController)); Unlock();}
        CXBXKRNL_API void SetXBController(const XBController *ctrl) { Lock(); memcpy(&m_XBController, ctrl, sizeof(XBController)); Unlock();}

        // ******************************************************************
        // * Xbe Path Accessors
        // ******************************************************************
        CXBXKRNL_API void GetXbePath(      char *path) { Lock(); strcpy(path, m_XbePath); Unlock(); }
        CXBXKRNL_API void SetXbePath(const char *path) { Lock(); strcpy(m_XbePath, path); Unlock(); }

    private:
        // ******************************************************************
        // * Shared configuration
        // ******************************************************************
        XBController m_XBController;
        XBVideo      m_XBVideo;
        char         m_XbePath[MAX_PATH];
};

// ******************************************************************
// * Exported Global Shared Memory Pointer
// ******************************************************************
extern CXBXKRNL_API EmuShared *g_EmuShared;
extern CXBXKRNL_API int        g_EmuSharedRefCount;

#endif
