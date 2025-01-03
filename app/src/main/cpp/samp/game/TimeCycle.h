//
// Created by x1y2z on 20.09.2023.
//

#ifndef LIVERUSSIA_TIMECYCLE_H
#define LIVERUSSIA_TIMECYCLE_H


#include "ColourSet.h"

class CTimeCycle {

public:
    static inline CColourSet m_CurrentColours;
    static inline RwRGBA m_BelowHorizonGrey;

public:
    static void InjectHooks();
};


#endif //LIVERUSSIA_TIMECYCLE_H
