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

#import <Foundation/Foundation.h>
#include "bstone_resource_path.h"

std::string getMainBundleResourcePath()
{
    NSBundle* mainBundle = [NSBundle mainBundle];
    
    if (mainBundle != nil)
    {
        NSString* path = [mainBundle resourcePath];
        return [path UTF8String] + std::string("/");
    }
    
    return "";
}

std::string getDocumentsResourcePath()
{
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    return [documentsDirectory UTF8String] + std::string("/");
}

std::string getJailbrokenResourcePath()
{
    NSString *path = @"/var/mobile/Media/bstone";
    return [path UTF8String] + std::string("/");
}
