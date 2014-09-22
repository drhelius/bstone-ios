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


#ifndef _AN_CODES_H_
#define _AN_CODES_H_

//--------------------------------------------------------------------------
//
// 										ANIM CODES - DOCS
//
//--------------------------------------------------------------------------
//
//   FI - Fade In the current frame (Last Frame grabbed)
//   FO - Fade Out the current frame (Last Frame grabbed)
//   FB - Fade In with rate (a numeral value should follow in the script)
//        ** MUST be a divisor of 64
//   FE - Fade Out with rate (a numeral value should follow in the script)
//        ** MUST be a divisor of 64
//   SD - Play sounds (a numeral value should follow in the script)
//   GR - Graphic Page (full screen)
//
//   PA - Pause/Delay 'xxxxxx' number of VBLs
//
//
//
//
//

//--------------------------------------------------------------------------
//
// 										MACROS
//
//--------------------------------------------------------------------------


#define MV_CNVT_CODE(c1,c2)		((Uint16)((c1)|(c2<<8)))

#define AN_PAUSE						MV_CNVT_CODE('P','A')
#define AN_SOUND						MV_CNVT_CODE('S','D')
#define AN_MUSIC						MV_CNVT_CODE('M','U')
#define AN_PAGE 						MV_CNVT_CODE('G','R')
#define AN_FADE_IN_FRAME			MV_CNVT_CODE('F','I')
#define AN_FADE_OUT_FRAME			MV_CNVT_CODE('F','O')
#define AN_FADE_IN					MV_CNVT_CODE('F','B')
#define AN_FADE_OUT					MV_CNVT_CODE('F','E')
#define AN_PALETTE					MV_CNVT_CODE('P','L')

#define AN_PRELOAD_BEGIN		 	MV_CNVT_CODE('L','B')
#define AN_PRELOAD_END				MV_CNVT_CODE('L','E')

#define AN_END_OF_ANIM			 	MV_CNVT_CODE('X','X')

#endif
