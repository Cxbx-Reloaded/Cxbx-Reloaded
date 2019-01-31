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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "common\Error.h"

const std::string Error::GetError()
{
    return std::string(m_szError);
}

bool Error::HasError() const
{
    return HasFatalError() || (m_szError[0] != '\0');
}

bool Error::HasFatalError() const
{
    return m_bFatal;
}

bool Error::ClearError()
{
    if (m_bFatal) { return false; }

	m_szError[0] = '\0';
    m_bFatal = false;

    return true;
}

void Error::SetError(const std::string& strError)
{
    // assert(strError.length()) < sizeof(m_szError));

    memset(m_szError, '\0', sizeof(m_szError));
    memcpy(m_szError, strError.c_str(), strError.length());
}

void Error::SetFatalError(const std::string& strError)
{
    // assert(strError.length()) < sizeof(m_szError));

    memset(m_szError, '\0', sizeof(m_szError));
    memcpy(m_szError, strError.c_str(), strError.length());
    m_bFatal = true;
}
