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
bool            g_bVBSkipStream = false;
bool            g_bVBSkipPusher = false;
ResourceTracker g_VBTrackTotal;
ResourceTracker g_VBTrackDisable;
ResourceTracker g_PBTrackTotal;
ResourceTracker g_PBTrackDisable;

ResourceTracker::~ResourceTracker()
{
	RTNode *cur = m_head;

	while(cur != 0)
	{
        RTNode *tmp = cur->pNext;

        delete cur;

		cur = tmp;
	}
}

void ResourceTracker::insert(void *pResource)
{
    this->Lock();

    if(exists(pResource))
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

	m_tail->pNext = new RTNode();

    m_tail = m_tail->pNext;

    m_tail->pResource = 0;
	m_tail->pNext = 0;

    this->Unlock();

    return;
}

void ResourceTracker::remove(void *pResource)
{
    this->Lock();

    RTNode *pre = 0;
    RTNode *cur = m_head;

    while(cur != 0)
    {
        if(cur->pResource == pResource)
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
    this->Lock();

    RTNode *cur = m_head;

    while(cur != 0)
    {
        if(cur->pResource == pResource)
        {
            this->Unlock();
            return true;
        }

        cur = cur->pNext;
    }

    this->Unlock();

    return false;
}
