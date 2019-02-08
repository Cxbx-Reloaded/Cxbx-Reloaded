// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->core->HLE->DSound->XbDSoundLogging.hpp
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
// *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// *
// *  (c) 2019 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef XBDSOUNDLOGGING_H
#define XBDSOUNDLOGGING_H

#include "Logging.h"
namespace XTL {
#include "XbDSoundTypes.h"

// DSound flag/enum
enum DSMIXBIN_SPEAKER;
enum DSSPEAKER_FLAG;
// DSound generic flag/enum
enum DSFILTER_MODE;
// DSound Buffer flag/enum
enum DSBPAUSE_FLAG;
enum DSBPLAY_FLAG;
enum DSBSTATUS_FLAG;
enum DSBSTOPEX_FLAG;
// DSound Stream flag/enum
enum DSSFLUSHEX_FLAG;
enum DSSPAUSE_FLAG;
enum DSSSTATUS_FLAG;
// DSound XMedia flag/enum
enum XMP_STATUS;
enum XMO_STREAMF;

// DSound flag/enum
FLAGS2STR_HEADER(DSSPEAKER_FLAG)
// DSound generic flag/enum
ENUM2STR_HEADER(DSFILTER_MODE)
// DSound Buffer flag/enum
FLAGS2STR_HEADER(DSBPAUSE_FLAG)
FLAGS2STR_HEADER(DSBPLAY_FLAG)
FLAGS2STR_HEADER(DSBSTATUS_FLAG)
FLAGS2STR_HEADER(DSBSTOPEX_FLAG)
// DSound Stream flag/enum
FLAGS2STR_HEADER(DSSFLUSHEX_FLAG)
FLAGS2STR_HEADER(DSSPAUSE_FLAG)
FLAGS2STR_HEADER(DSSSTATUS_FLAG)

// DSound class usage
LOGRENDER_HEADER(X_DSCAPS)
LOGRENDER_HEADER(X_DSI3DL2LISTENER)
LOGRENDER_HEADER(X_DSMIXBINS)
LOGRENDER_HEADER(X_DSMIXBINSVOLUMEPAIR)
LOGRENDER_HEADER(X_DSOUTPUTLEVELS)
// DSound generic class usage
LOGRENDER_HEADER(X_DS3DBUFFER)
LOGRENDER_HEADER(X_DSENVOLOPEDESC)
LOGRENDER_HEADER(X_DSFILTERDESC)
LOGRENDER_HEADER(X_DSI3DL2BUFFER)
LOGRENDER_HEADER(X_DSI3DL2OBSTRUCTION)
LOGRENDER_HEADER(X_DSI3DL2OCCLUSION)
LOGRENDER_HEADER(DSLFODESC)
LOGRENDER_HEADER(XBOXADPCMWAVEFORMAT)
// DSound Buffer class usage
LOGRENDER_HEADER(X_DSBUFFERDESC)
// DSound Stream class usage
LOGRENDER_HEADER(X_DSSTREAMDESC)
// DSound XMedia class usage
LOGRENDER_HEADER(XMEDIAINFO)
LOGRENDER_HEADER(XMEDIAPACKET)
}
#endif
