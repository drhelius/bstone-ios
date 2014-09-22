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
// A binary reader for a block of memory.
//


#ifndef BSTONE_MEMORY_BINARY_READER_H
#define BSTONE_MEMORY_BINARY_READER_H


#include <cstddef>

#include "SDL.h"


namespace bstone {


// A binary reader for a block of memory.
class MemoryBinaryReader {
public:
    MemoryBinaryReader();

    MemoryBinaryReader(
        const void* data,
        Sint64 data_size);

    // Initializes the reader.
    bool initialize(
        const void* data,
        Sint64 data_size);

    // Uninitializes the reader.
    void uninitialize();

    // Returns true if the reader is initialized or
    // false otherwise.
    bool is_initialized() const;

    // Reads a signed 8-bit integer value.
    Sint8 read_s8();

    // Reads an unsigned 8-bit integer value.
    Uint8 read_u8();

    // Reads a signed 16-bit integer value.
    Sint16 read_s16();

    // Reads an unsigned 16-bit integer value.
    Uint16 read_u16();

    // Reads a signed 32-bit integer value.
    Sint32 read_s32();

    // Reads an unsigned 32-bit integer value.
    Uint32 read_u32();

    // Reads a signed 64-bit integer value.
    Sint64 read_s64();

    // Reads an unsigned 64-bit integer value.
    Uint64 read_u64();

    // Reads a 32-bit float-point value.
    float read_r32();

    // Reads a 64-bit float-point value.
    double read_r64();

    // Skips a specified number of octets forward if count positive
    // or backward otherwise.
    bool skip(
        Sint64 count);

    // Returns a current position.
    Sint64 get_position() const;

    // Sets a current position to a specified one.
    bool set_position(
        Sint64 position);

    // Returns a pointer to data for a current position.
    template<class T>
    const T* get_data() const
    {
        return reinterpret_cast<const T*>(&data_[data_offset_]);
    }

private:
    const Uint8* data_;
    Sint64 data_size_;
    Sint64 data_offset_;

    template<class T>
    T read()
    {
        if (!is_initialized())
            return 0;

        if (data_offset_ < 0)
            return 0;

        if ((data_offset_ + static_cast<Sint64>(sizeof(T))) >= data_size_)
            return 0;

        T result = *reinterpret_cast<const T*>(&data_[data_offset_]);
        data_offset_ += sizeof(T);
        return result;
    }
}; // class MemoryBinaryReader


} // namespace bstone


#endif // BSTONE_MEMORY_BINARY_READER_H
