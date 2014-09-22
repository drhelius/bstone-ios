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


//
// A wrapper for C platform-dependent functions.
//


#include "bstone_c.h"

#include <algorithm>
#include <locale>


// FIXME
// Non-"C" locales in MinGW are not supported.


namespace bstone {


// (static)
int C::memicmp(
    const void* buf1,
    const void* buf2,
    size_t cnt)
{
    const char* a = static_cast<const char*>(buf1);
    const char* b = static_cast<const char*>(buf2);

    std::locale locale("");

    const std::ctype<char>& ctype =
        std::use_facet<std::ctype<char> >(locale);

    int diff = 0;

    for (size_t i = 0; diff == 0 && i < cnt; ++i) {
        diff = static_cast<unsigned char>(ctype.tolower(a[i])) -
            static_cast<unsigned char>(ctype.tolower(b[i]));
    }

    return diff;
}

// (static)
int C::stricmp(
    const char* str1,
    const char* str2)
{
    std::locale locale("");

    const std::ctype<char>& ctype =
        std::use_facet<std::ctype<char> >(locale);

    int diff = 0;

    for (size_t i = 0; diff == 0; ++i) {
        char a = str1[i];
        char b = str2[i];

        diff = static_cast<unsigned char>(ctype.tolower(a)) -
            static_cast<unsigned char>(ctype.tolower(b));

        if (a == '\0' || b == '\0')
            break;
    }

    return diff;
}

// (static)
double C::m_pi()
{
    return 3.14159265358979323846;
}


} // namespace bstone
