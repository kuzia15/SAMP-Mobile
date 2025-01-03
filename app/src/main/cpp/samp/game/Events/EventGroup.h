//
// Created by x1y2z on 26.01.2024.
//

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Event.h"
#include "../common.h"

struct CPedGTA;

const int32 TOTAL_EVENTS_PER_EVENTGROUP = 16;

class CEventGroup {
public:
    CPedGTA*    m_pPed;
    int32       m_count;
    CEvent*     m_events[TOTAL_EVENTS_PER_EVENTGROUP];
};