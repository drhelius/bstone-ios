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


#ifndef _INPUTMANAGER_H
#define	_INPUTMANAGER_H

#include <queue>
#include <vector>
#include "bstone_singleton.h"
#include "bstone_in_callback.h"
#include "bstone_in_regions.h"
#include "bstone_timer.h"

class InputManager : public Singleton<InputManager>
{
    friend class Singleton<InputManager>;

private:

    enum eRegionType
    {
        REGION_RECT,
        REGION_CIRCLE
    };

    struct stRegionEvent
    {
        int id;
        bool receiveMoveEvent;
        eRegionType regionType;
        bool pressed;
        Region* region;
        InputCallbackGeneric* pCallback;
        UITouch* pActualTouch;
    };

    struct stRegionEventResponse
    {
        stInputCallbackParameter parameter;
        InputCallbackGeneric* pCallback;
        int id;
    };

    typedef std::queue<stRegionEventResponse> TRegionEventResponseQueue;

    TRegionEventResponseQueue m_RegionEventResponseQueue;

    typedef std::vector<stRegionEvent> TRegionEventVector;

    TRegionEventVector m_RegionEventVector;

    Timer m_Timer;

    float m_fInputRate;
    
private:
    
    InputManager();

public:

    ~InputManager();

    void Update(void);

    void HandleTouch(UITouch* touch, UIView* view);

    void AddRectRegionEvent(float x, float y, float width, float height, InputCallbackGeneric* pCallback, int id = 0, bool receiveMoveEvent = false);
    void AddCircleRegionEvent(float x, float y, float radius, InputCallbackGeneric* pCallback, int id = 0, bool receiveMoveEvent = false);

    void ClearRegionEvents(void);
};

#endif	/* _INPUTMANAGER_H */

