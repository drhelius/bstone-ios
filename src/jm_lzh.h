/* ==============================================================
bstone-ios: Blake Stone Planet Strike for iOS

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013 Boris Bendovsky (bibendovsky@hotmail.com)
Copyright (c) 2014 Ignacio Sanchez (ignacio.sanchez@geardome.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
============================================================== */


#ifndef _JM_LZH_H_
#define _JM_LZH_H_


#include "SDL.h"


extern void (*LZH_CompressDisplayVector)(Uint32, Uint32);
extern void (*LZH_DecompressDisplayVector)(Uint32, Uint32);



//===========================================================================
//
//											PROTOTYPES
//
//===========================================================================


bool LZH_Startup(void);
void LZH_Shutdown(void);

int LZH_Compress(
    const void* infile,
    void* outfile,
    Uint32 DataLength);

int LZH_Decompress(
    const void* infile,
    void* outfile,
    Uint32 OrginalLength,
    Uint32 CompressLength);

#endif
