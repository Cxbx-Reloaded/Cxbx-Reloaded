# ******************************************************************
# *
# *    .,-:::::    .,::      .::::::::.    .,::      .:
# *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
# *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
# *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
# *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
# *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
# *
# *   Cxbx->Makefile
# *
# *  This file is part of the Cxbx project.
# *
# *  Cxbx and Cxbe are free software; you can redistribute them
# *  and/or modify them under the terms of the GNU General Public
# *  License as published by the Free Software Foundation; either
# *  version 2 of the license, or (at your option) any later version.
# *
# *  This program is distributed in the hope that it will be useful,
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# *  GNU General Public License for more details.
# *
# *  You should have recieved a copy of the GNU General Public License
# *  along with this program; see the file COPYING.
# *  If not, write to the Free Software Foundation, Inc.,
# *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
# *
# *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
# *
# *  All rights reserved
# *
# ******************************************************************
# *  File Authors: Aaron Robinson, Edgar Hucek
# ******************************************************************
CC	= g++
CFLAGS  = -g -O2 -IInclude -IInclude/Core -IInclude/Standard -IInclude/Standard/Cxbe -IInclude/Linux/
OBJCOPY	= objcopy
GCC295 = cpp0-2.95
#LDFLAGS = -s -S -T ldscript.ld

OBJECTS	= Source/Core/Error.o \
	Source/Core/Exe.o \
	Source/Core/Xbe.o \
        Source/Core/OpenXDK.o \
	Source/Standard/Cxbe/Main.o

RESOURCES = 

# target:
all	: cxbe

clean	:
	rm -rf *.o *~ core *.core ${OBJECTS} ${RESOURCES}
	rm -rf *.o *~ core *.core ${OBJECTS} ${RESOURCES} Bin/cxbe Bin/cxbe.exe

cxbe : ${OBJECTS} ${RESOURCES}
	${CC} -o Bin/$@ ${OBJECTS} ${RESOURCES} ${LDFLAGS-XBE}

### rules:

%.o	: %.cpp
	${CC} ${CFLAGS} -o $@ -c $<

%.o	: %.c
	${CC} ${CFLAGS} -o $@ -c $<

%.o	: %.S
	${CC} -DASSEMBLER ${CFLAGS} -o $@ -c $<
