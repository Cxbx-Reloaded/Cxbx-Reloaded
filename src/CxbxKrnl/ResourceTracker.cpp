// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->ResourceTracker.cpp
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
#include "ResourceTracker.h"

// exported globals

bool g_bVBSkipStream = false;
bool g_bVBSkipPusher = false;

//
// all of our resource trackers
//

ResourceTracker g_VBTrackTotal;
ResourceTracker g_VBTrackDisable;
ResourceTracker g_PatchedStreamsCache;
ResourceTracker g_DataToTexture;
ResourceTracker g_AlignCache;

ResourceTracker::~ResourceTracker()
{
    clear();
}

void ResourceTracker::clear()
{
    this->Lock();

    RTNode *cur = m_head;

    while(cur != 0)
    {
        RTNode *tmp = cur->pNext;

        delete cur;

        cur = tmp;
    }

    m_head = m_tail = 0;

    this->Unlock();
}

void ResourceTracker::insert(void *pResource)
{
    insert((uint32)pResource, pResource);
}

void ResourceTracker::insert(uint32 uiKey, void *pResource)
{
    this->Lock();

    if(exists(uiKey))
    {
        this->Unlock();
        return;
    }

    if(m_head == 0)
    {
        m_tail = m_head = new RTNode();
        m_tail->pResource = 0;
        m_tail->pNext = 0;
    }

    m_tail->pResource = pResource;
    m_tail->uiKey = uiKey;

    m_tail->pNext = new RTNode();

    m_tail = m_tail->pNext;

    m_tail->pResource = 0;
    m_tail->uiKey = 0;
    m_tail->pNext = 0;

    this->Unlock();

    return;
}

void ResourceTracker::remove(void *pResource)
{
    remove((uint32)pResource);
}

void ResourceTracker::remove(uint32 uiKey)
{
    this->Lock();

    RTNode *pre = 0;
    RTNode *cur = m_head;

    while(cur != 0)
    {
        if(cur->uiKey == uiKey)
        {
            if(pre != 0)
            {
                pre->pNext = cur->pNext;
            }
            else
            {
                m_head = cur->pNext;

                if(m_head->pNext == 0)
                {
                    delete m_head;

                    m_head = 0;
                    m_tail = 0;
                }
            }

            delete cur;

            this->Unlock();

            return;
        }

        pre = cur;
        cur = cur->pNext;
    }

    this->Unlock();

    return;
}

bool ResourceTracker::exists(void *pResource)
{
    return exists((uint32)pResource);
}

bool ResourceTracker::exists(uint32 uiKey)
{
    this->Lock();

    RTNode *cur = m_head;

    while(cur != 0)
    {
        if(cur->uiKey == uiKey)
        {
            this->Unlock();
            return true;
        }

        cur = cur->pNext;
    }

    this->Unlock();

    return false;
}

void *ResourceTracker::get(void *pResource)
{
    return get((uint32)pResource);
}

void *ResourceTracker::get(uint32 uiKey)
{
    RTNode *cur = m_head;

    while(cur != 0)
    {
        if(cur->uiKey == uiKey)
        {
            return cur->pResource;
        }

        cur = cur->pNext;
    }

    return 0;
}

uint32 ResourceTracker::get_count(void)
{
    uint32 uiCount = 0;

    this->Lock();

    RTNode *cur = m_head;

    while(cur != 0)
    {
        uiCount++;

        cur = cur->pNext;
    }

    this->Unlock();

    return uiCount;
}
