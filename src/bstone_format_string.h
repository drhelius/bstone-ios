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
// A wrapper for std::ostringstream to produce a formatted string.
//


#ifndef BSTONE_FORMAT_STRING_H
#define BSTONE_FORMAT_STRING_H


#include <iomanip>
#include <sstream>


namespace bstone {


// A wrapper for std::ostringstream to produce a formatted string.
class FormatString {
public:
    FormatString();

    operator std::string();

    std::string to_string() const;

    template<class T>
    bstone::FormatString& operator <<(
        const T& value)
    {
        stream_ << value;
        return *this;
    }

private:
    std::ostringstream stream_;

    FormatString(
        const FormatString& that);

    FormatString& operator=(
        const FormatString& that);
}; // class FormatString


} //namespace bstone


#endif // BSTONE_FORMAT_STRING_H
