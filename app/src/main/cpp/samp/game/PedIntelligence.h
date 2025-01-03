//
// Created by x1y2z on 07.04.2023.
//

#pragma once

#include "main.h"
#include "game/Tasks/TaskManager.h"
#include "game/Events/EventHandler.h"
#include "game/Events/EventGroup.h"

class CTaskSimpleJetPack;

class CPedIntelligence
{
public:
    class CPedGTA* m_pPed;
    CTaskManager   m_TaskMgr;
    CEventHandler  m_eventHandler;
    CEventGroup    m_eventGroup;

    int     m_iDecisionMakerType;
    int     m_iDecisionMakerTypeInGroup;
    float   m_fHearingRange;
    float   m_fSeeingRange;

    int     m_iMaxNumFriendsToInform;
    float   m_fMaxInformFriendDistance;
    float   m_fFollowNodeThresholdDistance;

    int8    m_iNextEventResponseSequence;
    uint8   m_iHighestPriorityEventType;
    uint8   m_iHighestPriorityEventPriority;
    uint8   pad0;

#if VER_x32
    uint8   m_vehicleScanner[0x1C8];
#else
    uint8   pad1[0x2C8];
#endif

public:
    static void InjectHooks();

    CPedIntelligence(CPedGTA* ped);
    ~CPedIntelligence();

    void Process();
    void FlushImmediately(bool bSetPrimaryDefaultTask);

    void FlushIntelligence();
    void ProcessAfterProcCol();
    void ProcessAfterPreRender();

    CTaskSimpleJetPack* GetTaskJetPack();
};
VALIDATE_SIZE(CPedIntelligence, (VER_x32 ? 0x298 : 0x440));

