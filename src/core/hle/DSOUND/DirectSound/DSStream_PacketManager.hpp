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
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2017-2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <vector>

#include "DirectSound.hpp"

#define vector_hvp_iterator std::vector<XTL::host_voice_packet>::iterator

extern void DSStream_Packet_Clear(
    vector_hvp_iterator        &buffer,
    DWORD                       status,
    XTL::LPFNXMOCALLBACK        Xb_lpfnCallback,
    LPVOID                      Xb_lpvContext,
    XTL::X_CDirectSoundStream*  pThis);

extern bool DSStream_Packet_Process(XTL::X_CDirectSoundStream* pThis);

extern void DSStream_Packet_FlushEx_Reset(XTL::X_CDirectSoundStream* pThis);

extern bool DSStream_Packet_Flush(XTL::X_CDirectSoundStream* pThis);
