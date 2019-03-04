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
#ifndef XBE_PRINTER_H
#define XBE_PRINTER_H

#include "common\Error.h"
#include "common\xbe\Xbe.h"

#include <cstdio>

extern std::string DumpInformation(Xbe* Xbe_object);

class XbePrinter
{
    public:
        XbePrinter(Xbe*);
        std::string GenXbeInfo();

    private:
        Xbe *Xbe_to_print;
        Xbe::Header *Xbe_header;
        Xbe::Certificate *Xbe_certificate;

        std::string GenHexRow(uint08*, const uint08, const uint08);
        std::string utf8_to_ascii(const wchar_t*);
        std::string AllowedMediaToString();
        std::string GameRatingToString();

        std::string GenDumpHeader();

        std::string GenXbeHeaderInfo();
        std::string GenDigitalSignature();
        std::string GenGeneralHeaderInfo1();
        std::string GenInitFlags();
        std::string GenGeneralHeaderInfo2();

        std::string GenXbeCertificateInfo();
        std::string GenCertificateHeader();
        std::string GenAlternateTitleIDs();
        std::string GenMediaInfo();
        std::string GenLANKey();
        std::string GenSignatureKey();
        std::string GenAlternateSignatureKeys();
        std::string GenExtraInfo();

        std::string GenSectionInfo();
        std::string GenSectionHeaders();
        std::string GenSectionFlags(Xbe::SectionHeader);
        std::string GenSectionDigest(Xbe::SectionHeader);

        std::string GenLibraryVersions();
        std::string GenLibraryFlags(Xbe::LibraryVersion);

        std::string GenTLS();
};

#endif // XBE_PRINTER_H
