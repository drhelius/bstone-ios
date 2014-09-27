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


#ifndef _TIMER_H
#define	_TIMER_H

#include <mach/mach.h>
#include <mach/mach_time.h>
#include "bstone_definitions.h"

#define FPS_REFRESH_TIME	0.5f

#define FRAME_RATE_AVERAGE 5

class Timer
{
public:
    Timer(void);

    Timer(bool calculateFPS)
    {
        m_bCalculateFPS = calculateFPS;

        Reset();
    };

    ~Timer(void);
    void Start(void);
    void Stop(void);
    void Continue(void);

    float GetActualTime(void) const;
    float GetFrameTime(void) const;
    float GetDeltaTime(void) const;

    void Reset(void);
    void Update(void);
    float GetFPS(void) const;

    bool IsRunning(void) const;

    void SetOffset(const float offset)
    {
        m_fOffset = offset;
    };

private:
    bool m_bCalculateFPS;
    bool m_bIsRunning;

    float m_fOffset;

    uint64_t m_i64BaseTicks;
    uint64_t m_i64StopedTicks;

    float m_fResolution;

    float m_fFrameTime;
    float m_fDeltaTime;

    uint32_t m_iFrameCount;
    float m_fLastUpdate;
    float m_fFPS;

    float m_fDeltaTimeAccumulation[FRAME_RATE_AVERAGE];
    int m_iCurrentAccumulator;
};

#endif	/* _TIMER_H */
