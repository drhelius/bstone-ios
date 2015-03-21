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
#ifndef _TEXTUREMANAGER_H
#define	_TEXTUREMANAGER_H

#include <map>
#include <string>
#import <GLKit/GLKit.h>
#include "bstone_singleton.h"
#include "bstone_ogl_texture.h"

class TextureManager : public Singleton<TextureManager>
{

    friend class Singleton<TextureManager>;

private:

    typedef std::map<std::string, Texture*> TTextureMap;
    typedef std::pair<std::string, Texture*> TTextureMapPair;
    typedef TTextureMap::iterator TTextureMapIterator;
    typedef std::pair<TTextureMapIterator, bool> TTextureMapResultPair;

    TTextureMap m_TextureMap;

    

    bool LoadTexture(Texture* pTexture, bool mipmaps);
    GLboolean GenMipMap2D(GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight, int level);

public:

    TextureManager();
    ~TextureManager();

    Texture* GetTexture(const char* strTextureName, bool mipmaps = false);

    bool UnloadTexture(const char* strTextureName);
    bool UnloadTexture(Texture* pTexture);
    int UnloadAll(void);
};

#endif	/* _TEXTUREMANAGER_H */

