// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase.h
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
#ifndef HLEDATABASE_H
#define HLEDATABASE_H

#include "Xapi.1.0.3911.h"
#include "Xapi.1.0.4034.h"
#include "Xapi.1.0.4134.h"
#include "Xapi.1.0.4361.h"
#include "Xapi.1.0.4627.h"
#include "D3D8.1.0.3925.h"
#include "D3D8.1.0.4034.h"
#include "D3D8.1.0.4134.h"
#include "D3D8.1.0.4361.h"
#include "D3D8.1.0.4627.h"
#include "DSound.1.0.4361.h"
#include "DSound.1.0.4627.h"
#include "XG.1.0.4361.h"
#include "XG.1.0.4627.h"
#include "XOnline.1.0.4361.h"

// ******************************************************************
// * HLEDataBase
// ******************************************************************
extern struct HLEData
{
    char       *Library;

    uint16      MajorVersion;
    uint16      MinorVersion;
    uint16      BuildVersion;

    OOVPATable *OovpaTable;
    uint32      OovpaTableSize;
}
HLEDataBase[];

// ******************************************************************
// * HLEDataBaseSize
// ******************************************************************
extern uint32 HLEDataBaseSize;

// ******************************************************************
// * XRefDataBase
// ******************************************************************
extern uint32 XRefDataBase[];

// ******************************************************************
// * UnResolvedXRefs
// ******************************************************************
extern uint32 UnResolvedXRefs;

// ******************************************************************
// * bXRefFirstPass
// ******************************************************************
extern bool bXRefFirstPass;

// ******************************************************************
// * XRefDataBaseOffset
// ******************************************************************
enum XRefDataBaseOffset
{
    XREF_XNINIT                 = 0,
    XREF_FCLOSEDEVICE           = 1,
    XREF_CLEARSTATEBLOCKFLAGS   = 2,
    XREF_RECORDSTATEBLOCK       = 3,
    XREF_SETDISTANCEFACTORA     = 4,
    XREF_SETDISTANCEFACTORB     = 5,
    XREF_SETROLLOFFFACTOR       = 6,
    XREF_SETDOPPLERFACTOR       = 7,
    XREF_SETBUFFERDATA          = 8,
    XREF_SETCURRENTPOSITION     = 9,
    XREF_SETCURRENTPOSITION2    = 10,
    XREF_GETCURRENTPOSITION     = 11,
    XREF_GETCURRENTPOSITION2    = 12,
    XREF_DSOUNDPLAY             = 13,
    XREF_DSOUNDPLAY2            = 14,
    XREF_DSOUNDSTOP             = 15,
    XREF_DSOUNDSTOP2            = 16,
    XREF_DSSETBUFFERDATA        = 17,
    XREF_DSSETBUFFERDATA2       = 18,
    XREF_DSCREATESOUNDBUFFER    = 19,
    XREF_DSCREATESOUNDSTREAM    = 20,
    XREF_DSSTREAMPAUSE          = 21,
    XREF_DSSTREAMSETVOLUME      = 22,
    XREF_DSSETI3DL2LISTENER     = 23,
    XREF_DSSETMIXBINHEADROOM    = 24,
    XREF_GET2DSURFACEDESCB      = 25
};

#endif
