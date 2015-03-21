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

#pragma once
#ifndef _TEXTURE_H
#define	_TEXTURE_H

#import <GLKit/GLKit.h>

class TextureManager;

class Texture
{

    friend class TextureManager;

private:
    GLuint m_theTexture;
    int m_iWidth;
    int m_iHeight;
    bool m_bIsCompressed;
    char m_strName[256];

public:
    Texture(void)
    {
        m_theTexture = 0;
        m_iWidth = 0;
        m_iHeight = 0;
        m_bIsCompressed = 0;
        m_strName[0] = 0;
    };

    GLuint GetID(void) const
    {
        return m_theTexture;
    };

    int GetWidth(void) const
    {
        return m_iWidth;
    };

    int GetHeight(void) const
    {
        return m_iHeight;
    };

    bool IsCompressed(void) const
    {
        return m_bIsCompressed;
    };

    const char* GetName(void) const
    {
        return m_strName;
    };
};

#endif	/* _TEXTURE_H */