//
// Created by x1y2z on 22.06.2023.
//

#include "EventHandler.h"
#include "Event.h"
#include "../vendor/armhook/patch.h"

CEventHandler::CEventHandler(CPedGTA* ped) {
    m_ped                  = ped;
    m_physicalResponseTask = nullptr;
    m_eventResponseTask    = nullptr;
    m_attackTask           = nullptr;
    m_sayTask              = nullptr;
    m_partialAnimTask      = nullptr;
    m_history.ClearAllEvents();
}

void CEventHandler::FlushImmediately() {
    m_history.Flush();
}

bool CEventHandler::IsTemporaryEvent(const CEvent& event) {
    auto eventType = event.GetEventType();
    switch (eventType) {
        case EVENT_VEHICLE_COLLISION:
        case EVENT_PED_COLLISION_WITH_PED:
        case EVENT_PED_COLLISION_WITH_PLAYER:
        case EVENT_PLAYER_COLLISION_WITH_PED:
        case EVENT_OBJECT_COLLISION:
        case EVENT_BUILDING_COLLISION:
        case EVENT_IN_AIR:
        case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
        case EVENT_POTENTIAL_WALK_INTO_BUILDING:
        case EVENT_POTENTIAL_WALK_INTO_OBJECT:
        case EVENT_POTENTIAL_WALK_INTO_PED:
        case EVENT_POTENTIAL_WALK_INTO_FIRE:
        case EVENT_AREA_CODES:
        case EVENT_STUCK_IN_AIR:
        case EVENT_POTENTIAL_GET_RUN_OVER:
        case EVENT_ON_ESCALATOR:
        case EVENT_FIRE_NEARBY:
            return true;
        default:
            return false;
    }
}

void CEventHandler::HandleEvents() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0037B818 + 1 : 0x45084C), this);
}
