// ******************************************************************
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
#include "XbDSoundTypes.h"

namespace xbox {

// TODO: Everything, only small portions had been implemented.

enum WAVEFORMAT_CHANNEL : int;
enum WAVEFORMAT_TAG : int;
// DSound flag/enum
enum DSMIXBIN_SPEAKER : int;
enum DSSPEAKER_FLAG : int;
// DSound generic flag/enum
enum DSFILTER_MODE : int;
// DSound Buffer flag/enum
enum DSBCAPS_FLAG : int;
enum DSBPAUSE_FLAG : int;
enum DSBPLAY_FLAG : int;
enum DSBSTATUS_FLAG : int;
enum DSBSTOPEX_FLAG : int;
// DSound Stream flag/enum
enum DSSCAPS_FLAG : int;
enum DSSFLUSHEX_FLAG : int;
enum DSSPAUSE_FLAG : int;
enum DSSSTATUS_FLAG : int;
// DSound XMedia flag/enum
enum XMP_STATUS : int;
enum XMO_STREAMF : int;

// DSound flag/enum
FLAGS2STR_HEADER(DSSPEAKER_FLAG)
// DSound generic flag/enum
ENUM2STR_HEADER(DSFILTER_MODE)
// DSound Buffer flag/enum
FLAGS2STR_HEADER(DSBCAPS_FLAG)
FLAGS2STR_HEADER(DSBPAUSE_FLAG)
FLAGS2STR_HEADER(DSBPLAY_FLAG)
FLAGS2STR_HEADER(DSBSTATUS_FLAG)
FLAGS2STR_HEADER(DSBSTOPEX_FLAG)
// DSound Stream flag/enum
FLAGS2STR_HEADER(DSSCAPS_FLAG)
FLAGS2STR_HEADER(DSSFLUSHEX_FLAG)
FLAGS2STR_HEADER(DSSPAUSE_FLAG)
FLAGS2STR_HEADER(DSSSTATUS_FLAG)

// DSound class usage
LOGRENDER_HEADER(X_DSCAPS)
LOGRENDER_HEADER(X_DSI3DL2LISTENER)
LOGRENDER_HEADER(X_DSMIXBINS)
LOGRENDER_HEADER(X_DSMIXBINVOLUMEPAIR)
LOGRENDER_HEADER(X_DSOUTPUTLEVELS)
// DSound generic class usage
LOGRENDER_HEADER(X_DS3DBUFFER)
LOGRENDER_HEADER(X_DSENVOLOPEDESC)
LOGRENDER_HEADER(X_DSFILTERDESC)
LOGRENDER_HEADER(X_DSI3DL2BUFFER)
LOGRENDER_HEADER(X_DSI3DL2OBSTRUCTION)
LOGRENDER_HEADER(X_DSI3DL2OCCLUSION)
LOGRENDER_HEADER(DSLFODESC)
LOGRENDER_HEADER(WAVEFORMATEX)
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
