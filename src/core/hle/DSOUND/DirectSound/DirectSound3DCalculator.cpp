// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  (c) 2019 Luke Usher
// *  (c) 2019-2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::DS3DCALC


#include <xboxkrnl/xboxkrnl.h>
#include <dsound.h>
#include "DirectSoundGlobal.hpp" // Global variables

#include "Logging.h"
#include "DirectSoundLogging.hpp"
#include "..\XbDSoundLogging.hpp"


// TODO: Tasks need to do for DirectSound HLE
// * Ixxxxx_ patch might not be a requirement.
//   * IDirectSound3DCalculator_GetMixBinVolumes
//   * IDirectSound3DCalculator_GetPanData

#include "DirectSoundInline.hpp"

/* ------------- Sorted relative functions begin ------------------*/

// ******************************************************************
// * patch:  CDirectSound3DCalculator_Calculate3D
// ******************************************************************
VOID WINAPI xbox::EMUPATCH(CDirectSound3DCalculator_Calculate3D)
(
    DWORD a1,
    DWORD a2
)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(a1)
        LOG_FUNC_ARG(a2)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch:  CDirectSound3DCalculator_GetVoiceData
// ******************************************************************
VOID WINAPI xbox::EMUPATCH(CDirectSound3DCalculator_GetVoiceData)
(
    DWORD a1,
    DWORD a2,
    DWORD a3,
    DWORD a4,
    DWORD a5
)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(a1)
        LOG_FUNC_ARG(a2)
        LOG_FUNC_ARG(a3)
        LOG_FUNC_ARG(a4)
        LOG_FUNC_ARG(a5)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();
}
