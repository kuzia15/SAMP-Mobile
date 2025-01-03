//
// Created by x1y2z on 22.06.2023.
//

#include "../common.h"
#include "Event.h"
#include "../vendor/armhook/patch.h"


CEvent::CEvent() {
    m_nTimeActive = 0;
    m_bValid = false;
}

CEvent::~CEvent() {
    // nothing here
}

// 0x4B2850
float CEvent::GetSoundLevel(const CEntityGTA* entity, CVector& position) {
    return CHook::CallFunction<float>(g_libGTASA + (VER_x32 ? 0x0036FC18 + 1 : 0x441438), this, entity, position);
//    if ((entity && entity != GetSourceEntity()) || GetLocalSoundLevel() == 0.0f)
//        return 0.0f;
//
//    float level = 1000.0f;
//    if (GetEventType() == EVENT_SOUND_QUIET) {
//        CVector& sourcePos = static_cast<CEventSoundQuiet*>(this)->m_position;
//        level = std::min((position - sourcePos).Magnitude(), 1.0f);
//    } else if (GetSourceEntity()) {
//        CVector& sourcePos = GetSourceEntity()->GetPosition();
//        level = std::min((position - sourcePos).Magnitude(), 1.0f);
//    }
//    return (GetLocalSoundLevel() - (std::log10f(level * level) * 10.0f)) - (level / 100.0f);
}

// 0x4AC050
float CEvent::CalcSoundLevelIncrement(float level1, float level2) {
    if (level2 == 0.0f)
        return 0.0f;

    if (level1 == 0.0f)
        return level2;

    level1 = std::powf(10.0f, level1 * 0.1f);
    level2 = std::powf(10.0f, level2 * 0.1f);
    return std::log10f((level1 + level2) / level1) * 10.0f;
}
