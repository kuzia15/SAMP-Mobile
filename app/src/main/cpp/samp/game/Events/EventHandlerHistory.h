#pragma once

#include "../Tasks/TaskTimer.h"
#include "game/common.h"
#include "game/Enums/eEventType.h"

class CEvent;
class CTask;
struct CPedGTA;

class CEventHandlerHistory {
public:
    CTask* m_task = nullptr;
    CEvent* m_nonTempEvent = nullptr;
    CEvent* m_tempEvent = nullptr;
    CEvent* m_storedActiveEvent = nullptr;
    CTaskTimer m_storedActiveEventTimer;

public:
    static void InjectHooks();

    CEventHandlerHistory() = default;
    ~CEventHandlerHistory();

    void ClearAllEvents();
    void ClearNonTempEvent();
    void ClearTempEvent();
    void ClearStoredActiveEvent();
    void Flush();
    CEvent* GetCurrentEvent() { return m_tempEvent ? m_tempEvent : m_nonTempEvent; }
    int32 GetCurrentEventPriority();
    CEvent* GetStoredActiveEvent() { return m_storedActiveEvent; }
    bool IsRespondingToEvent(eEventType eventType);
    void RecordCurrentEvent(CPedGTA* ped, CEvent& event);
    void StoreActiveEvent();
    bool TakesPriorityOverCurrentEvent(CEvent& event);
    void TickStoredEvent(CPedGTA* ped);
};

VALIDATE_SIZE(CEventHandlerHistory, (VER_x32 ? 0x1C : 0x30));
