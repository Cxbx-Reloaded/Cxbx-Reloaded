// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->ResourceTracker.h
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
#ifndef RESOURCETRACKER_H
#define RESOURCETRACKER_H

#include "Cxbx.h"
#include "Mutex.h"

extern bool g_bVBSkipStream;
extern bool g_bPBSkipPusher;

extern class ResourceTracker : public Mutex
{
    public:
		ResourceTracker() : m_head(0), m_tail(0) {};
	   ~ResourceTracker();

        // clear the tracker
        void clear();

        // insert a ptr
		void insert(void *pResource);

		// remove a ptr
		void remove(void *pResource);

		// check for existance of ptr
		bool exists(void *pResource);

        // for traversal
        struct RTNode *getHead() { return m_head; }

    private:
		// list of "live" vertex buffers for debugging purposes
		struct RTNode *m_head;
		struct RTNode *m_tail;
}
g_VBTrackTotal, g_VBTrackDisable,
g_PBTrackTotal, g_PBTrackDisable, g_PBTrackShowOnce;

struct RTNode
{
	void    *pResource;
	RTNode  *pNext;
};

#endif
