// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->core->HLE->DSound->XbDSoundLogging.cpp
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

#include "Logging.h"
#include "XbDSoundLogging.hpp"

extern LOGRENDER_HEADER(WAVEFORMATEX)

LOGRENDER(XTL::X_DSBUFFERDESC)
{
	return os
		LOGRENDER_MEMBER(dwSize)
		LOGRENDER_MEMBER(dwFlags)
		LOGRENDER_MEMBER(dwBufferBytes)
		LOGRENDER_MEMBER_TYPE(LPWAVEFORMATEX, lpwfxFormat)
		LOGRENDER_MEMBER(lpMixBins)
		LOGRENDER_MEMBER(dwInputMixBin)
		;
}

LOGRENDER(XTL::X_DSSTREAMDESC)
{
	return os
		LOGRENDER_MEMBER(dwFlags)
		LOGRENDER_MEMBER(dwMaxAttachedPackets)
		LOGRENDER_MEMBER_TYPE(LPWAVEFORMATEX, lpwfxFormat)
		LOGRENDER_MEMBER_TYPE(void*, lpfnCallback)
		LOGRENDER_MEMBER(lpvContext)
		LOGRENDER_MEMBER(lpMixBins)
		;
}
