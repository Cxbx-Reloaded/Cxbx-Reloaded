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
#include "XNet.1.0.3911.h"
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
    XREF_FCLOSEDEVICE,
    XREF_CLEARSTATEBLOCKFLAGS,
    XREF_RECORDSTATEBLOCK,
    XREF_SETDISTANCEFACTORA,
    XREF_SETDISTANCEFACTORB,
    XREF_SETROLLOFFFACTORA,
    XREF_SETROLLOFFFACTORB,
    XREF_SETDOPPLERFACTOR,
    XREF_SETBUFFERDATA,
    XREF_SETCURRENTPOSITION,
    XREF_SETCURRENTPOSITION2,
    XREF_GETCURRENTPOSITION,
    XREF_GETCURRENTPOSITION2,
    XREF_DSOUNDPLAY,
    XREF_DSOUNDPLAY2,
    XREF_DSOUNDSTOP,
    XREF_DSOUNDSTOP2,
    XREF_DSSETBUFFERDATA,
    XREF_DSSETBUFFERDATA2,
    XREF_DSCREATESOUNDBUFFER,
    XREF_DSCREATESOUNDSTREAM,
    XREF_DSSTREAMPAUSE,
    XREF_DSSTREAMSETVOLUME,
    XREF_DSSETI3DL2LISTENER,
    XREF_DSSETMIXBINHEADROOMA,
    XREF_DSSETMIXBINHEADROOMB,
    XREF_DSSETPOSITIONA,
    XREF_DSSETPOSITIONB,
    XREF_DSSETVELOCITYA,
    XREF_DSSETVELOCITYB,
    XREF_DSSETALLPARAMETERSA,
    XREF_DSSETALLPARAMETERSB,
    XREF_DSSETHEADROOMA,
    XREF_DSSETI3DL2SOURCE1A,
    XREF_DSSETI3DL2SOURCE2A,
    XREF_DSBUFFERSETLOOPREGIONA,
    XREF_DSSTREAMSETMAXDISTANCE1A,
    XREF_DSSTREAMSETMAXDISTANCE1B,
    XREF_DSSTREAMSETMAXDISTANCE1C,
    XREF_DSSTREAMSETMINDISTANCE1A,
    XREF_DSSTREAMSETMINDISTANCE1B,
    XREF_DSSTREAMSETMINDISTANCE1C,
    XREF_DSSTREAMSETCONEANGLES1A,
    XREF_DSSTREAMSETCONEANGLES1B,
    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A,
    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B,
    XREF_DSSTREAMSETALLPARAMETERS1A,
    XREF_DSSTREAMSETALLPARAMETERS1B,
    XREF_DSSTREAMSETALLPARAMETERS1C,
    XREF_DSSTREAMSETVELOCITY1A,
    XREF_DSSTREAMSETVELOCITY1B,
    XREF_DSSTREAMSETVELOCITY1C,
    XREF_DSSTREAMSETCONEORIENTATION1A,
    XREF_DSSTREAMSETCONEORIENTATION1B,
    XREF_DSSTREAMSETCONEORIENTATION1C,
    XREF_DSSTREAMSETPOSITION1A,
    XREF_DSSTREAMSETPOSITION1B,
    XREF_DSSTREAMSETPOSITION1C,
    XREF_DSSTREAMSETFREQUENCY1A,
    XREF_DSSTREAMSETFREQUENCY1B,
    XREF_DSSTREAMSETROLLOFFFACTOR1A,
    XREF_DSSTREAMSETROLLOFFFACTOR1B,
    XREF_GET2DSURFACEDESCB,
    XREF_COMMITDEFERREDSETTINGSA,
    XREF_COMMITDEFERREDSETTINGSB
};

#endif
