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


#ifndef _INPUTCALLBACK_H
#define	_INPUTCALLBACK_H

#include "bstone_vector.h"

enum eInputCallbackType
{

    PRESS_START,
    PRESS_MOVE,
    PRESS_END
};

struct stInputCallbackParameter
{
    eInputCallbackType type;
    Vec3 vector;
};

//////////////////////////
//////////////////////////

class InputCallbackGeneric
{

public:

    virtual ~InputCallbackGeneric() { };
    virtual void Execute(stInputCallbackParameter parameter, int id) const = 0;
};

//////////////////////////
//////////////////////////

template <class Class>
class InputCallbackOO : public InputCallbackGeneric
{

public:

    typedef void (Class::*Method)(stInputCallbackParameter, int);

private:

    Class* m_pClassInstance;
    Method m_theMethod;

public:

    InputCallbackOO(Class* class_instance, Method method)
    {
        m_pClassInstance = class_instance;
        m_theMethod = method;
    };

    virtual ~InputCallbackOO() { };

    virtual void Execute(stInputCallbackParameter parameter, int id) const
    {
        (m_pClassInstance->*m_theMethod)(parameter, id);
    };
};

//////////////////////////
//////////////////////////

class InputCallback : public InputCallbackGeneric
{
    
public:
    
    typedef void (*Method)(stInputCallbackParameter, int);
    
private:
    
    Method m_theMethod;
    
public:
    
    InputCallback(Method method)
    {
        m_theMethod = method;
    };
    
    virtual ~InputCallback() { };
    
    virtual void Execute(stInputCallbackParameter parameter, int id) const
    {
        (*m_theMethod)(parameter, id);
    };
};

#endif	/* _INPUTCALLBACK_H */

