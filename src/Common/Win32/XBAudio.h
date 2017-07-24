// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Common->Win32->Cxbx->XBAudio.h
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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef XBAUDIO_H
#define XBAUDIO_H

#include "Cxbx.h"
#include "Common/Error.h"
#include "Mutex.h"

// ******************************************************************
// * class: XBAudio
// ******************************************************************
class XBAudio : public Error
{
    public:
        // ******************************************************************
        // * Initialization
        // ******************************************************************
        XBAudio();
       ~XBAudio();

        // ******************************************************************
        // * Registry Load/Save
        // ******************************************************************
        void Load(const char *szRegistryKey);
        void Save(const char *szRegistryKey);

        // ******************************************************************
        // * SetAudioAdapter
        // ******************************************************************
        void  SetAudioAdapter(GUID binAudioAdapter) { m_binAudioAdapter = binAudioAdapter; }
        GUID GetAudioAdapter() const { return m_binAudioAdapter; }

        // ******************************************************************
        // * Audio Hack Toggling
        // ******************************************************************
        void SetLegacyAudioHack(BOOL bAudioHack) { m_bLegacyAudioHack = bAudioHack; }
        BOOL GetLegacyAudioHack() const { return m_bLegacyAudioHack; }

        // ******************************************************************
        // * PCM Toggling
        // ******************************************************************
        void SetPCM(BOOL bPCM) { m_bPCM = bPCM; }
        BOOL GetPCM() const { return m_bPCM; }

        // ******************************************************************
        // * XADPCM Toggling
        // ******************************************************************
        void SetXADPCM(BOOL bXADPCM) { m_bXADPCM = bXADPCM; }
        BOOL GetXADPCM() const { return m_bXADPCM; }

        // ******************************************************************
        // * Unknown Codec Toggling
        // ******************************************************************
        void SetUnknownCodec(BOOL bUnknownCodec) { m_bUnknownCodec = bUnknownCodec; }
        BOOL GetUnknownCodec() const { return m_bUnknownCodec; }

    private:
        // ******************************************************************
        // * Configuration
        // ******************************************************************
        GUID  m_binAudioAdapter;
        BOOL  m_bLegacyAudioHack;
        BOOL  m_bPCM;
        BOOL  m_bXADPCM;
        BOOL  m_bUnknownCodec;
};

#endif
