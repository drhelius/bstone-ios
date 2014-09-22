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


// ID_HEAD.H


#ifndef ID_HEAD_H
#define ID_HEAD_H


#include <cassert>
#include <cstdlib>

#include <memory>

#include "SDL.h"


#define WOLF
#define FREE_FUNCTIONS   (true)

#define	TEXTGR	0
#define	CGAGR	1
#define	EGAGR	2
#define	VGAGR	3

#define GRMODE	VGAGR

#define false (0)
#define true (1)
typedef	Sint16	boolean;

typedef	struct
		{
			int	x,y;
		} BSPoint;

typedef	struct
		{
			BSPoint	ul,lr;
		} BSRect;


void	Quit (char *error,...);		// defined in user program


#endif // ID_HEAD_H
