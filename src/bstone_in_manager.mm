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


#include "bstone_in_manager.h"

InputManager::InputManager()
{
    m_fInputRate = 0.15f;
    m_Timer.Start();
}

InputManager::~InputManager()
{
}

void InputManager::SetResolution(int w, int h)
{
    m_iWidth = w;
    m_iHeight = h;
}

void InputManager::Update(void)
{
    while (!m_RegionEventResponseQueue.empty())
    {
        stRegionEventResponse event = m_RegionEventResponseQueue.front();
        m_RegionEventResponseQueue.pop();

        event.pCallback->Execute(event.parameter, event.id);
    }
}

void InputManager::AddCircleRegionEvent(float x, float y, float radius, InputCallbackGeneric* pCallback, int id, bool receiveMoveEvent)
{
    stRegionEvent tmp;

    tmp.receiveMoveEvent = receiveMoveEvent;
    tmp.region = new CircleRegion(x, y, radius);
    tmp.regionType = REGION_CIRCLE;
    tmp.pCallback = pCallback;
    tmp.pressed = false;
    tmp.touch_id = 0;
    tmp.id = id;

    m_RegionEventVector.push_back(tmp);
}

void InputManager::AddRectRegionEvent(float x, float y, float width, float height, InputCallbackGeneric* pCallback, int id, bool receiveMoveEvent)
{
    stRegionEvent tmp;

    tmp.receiveMoveEvent = receiveMoveEvent;
    tmp.region = new RectRegion(x, y, width, height);
    tmp.regionType = REGION_RECT;
    tmp.pCallback = pCallback;
    tmp.pressed = false;
    tmp.touch_id = 0;
    tmp.id = id;

    m_RegionEventVector.push_back(tmp);
}

void InputManager::ClearRegionEvents(void)
{
    int size = static_cast<int>(m_RegionEventVector.size());

    for (int i = 0; i < size; i++)
    {
        SafeDelete(m_RegionEventVector[i].region);
    }

    m_RegionEventVector.clear();

    while (!m_RegionEventResponseQueue.empty())
    {
        m_RegionEventResponseQueue.pop();
    }
}

void InputManager::HandleTouch(SDL_TouchFingerEvent* touch)
{
    stPoint location;
    location.x = touch->x * m_iWidth;
    location.y = touch->y * m_iHeight;
    
    stPoint previousLocation;

    if (touch->type == SDL_FINGERMOTION)
    {
        previousLocation.x = (touch->x - touch->dx) * m_iWidth;
        previousLocation.y = (touch->y - touch->dy) * m_iHeight;
    }
    else
        previousLocation = location;

    TRegionEventVector::size_type size = m_RegionEventVector.size();

    for (uint32_t i = 0; i < size; i++)
    {
        stRegionEvent regionEvent = m_RegionEventVector[i];

        stRegionEventResponse event;
        event.pCallback = regionEvent.pCallback;
        event.id = regionEvent.id;

        bool sendEvent = false;
        
        switch (touch->type)
        {
            case SDL_FINGERDOWN:
            {
                if (!regionEvent.pressed && regionEvent.region->PointInRegion(location.x, location.y))
                {
                    event.parameter.type = PRESS_START;
                    if (regionEvent.regionType == REGION_RECT)
                    {
                        event.parameter.vector = Vec3(location.x, location.y, 0.0f);
                    }
                    else
                    {
                        Vec3 point = Vec3(location.x, location.y, 0.0f);
                        event.parameter.vector = point - regionEvent.region->GetPosition();
                    }
                    regionEvent.pressed = true;
                    m_RegionEventVector[i].touch_id = touch->fingerId;
                    sendEvent = true;
                }
                break;
            }
            case SDL_FINGERUP:
            {
                if (regionEvent.pressed && (regionEvent.touch_id == touch->fingerId))
                {
                    event.parameter.type = PRESS_END;
                    regionEvent.pressed = false;
                    sendEvent = true;
                }
                break;
            }
            case SDL_FINGERMOTION:
            {
                if (regionEvent.region->PointInRegion(previousLocation.x, previousLocation.y))
                {
                    if (regionEvent.pressed && !regionEvent.region->PointInRegion(location.x, location.y))
                    {
                        event.parameter.type = PRESS_END;
                        regionEvent.pressed = false;
                        sendEvent = true;
                    }
                    else
                    {
                        if (regionEvent.pressed && regionEvent.receiveMoveEvent && (m_Timer.GetActualTime() > m_fInputRate))
                        {
                            m_Timer.Start();
                            
                            event.parameter.type = PRESS_MOVE;
                            if (regionEvent.regionType == REGION_RECT)
                            {
                                event.parameter.vector = Vec3(location.x, location.y, 0.0f);
                            }
                            else
                            {
                                Vec3 point = Vec3(location.x, location.y, 0.0f);
                                event.parameter.vector = point - regionEvent.region->GetPosition();
                            }
                            m_RegionEventVector[i].touch_id = touch->fingerId;
                            sendEvent = true;
                        }
                        else if (!regionEvent.pressed)
                        {
                            event.parameter.type = PRESS_START;
                            if (regionEvent.regionType == REGION_RECT)
                            {
                                event.parameter.vector = Vec3(location.x, location.y, 0.0f);
                            }
                            else
                            {
                                Vec3 point = Vec3(location.x, location.y, 0.0f);
                                event.parameter.vector = point - regionEvent.region->GetPosition();
                            }
                            regionEvent.pressed = true;
                            m_RegionEventVector[i].touch_id = touch->fingerId;
                            sendEvent = true;
                        }
                    }
                }
                break;
            }
        }

        if (sendEvent)
        {
            m_RegionEventVector[i].pressed = regionEvent.pressed;
            m_RegionEventResponseQueue.push(event);
        }
    }
}


