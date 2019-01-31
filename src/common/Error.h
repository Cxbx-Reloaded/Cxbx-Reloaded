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
#ifndef ERROR_H
#define ERROR_H

#include "Cxbx.h"

#include <string>

// inherit from this class for handy error reporting capability
class Error
{
public:
    const std::string GetError();

    bool HasError() const;
    bool HasFatalError() const;

    bool ClearError();

protected:
    // protected constructor so this class must be inherited from
    Error() : m_szError("\0"), m_bFatal(false) { }

    // protected so only derived class may set an error
    void SetError(const std::string& strStrError);
    void SetFatalError(const std::string& strError);

private:
    char m_szError[60]; // Cannot be std::string, which sizeof() differs between builds
    uint32_t m_bFatal; // Cannot be bool, to avoid bit packing altering memory layout
};

#endif
