/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "EventHandlerHistory.h"
#include "../Enums/eEventType.h"
#include "../Enums/eWeaponType.h"

class CTask;
class CEvent;
class CEventDamage;
struct CPedGTA;
struct CEntityGTA;
class CEventHandler {
public:
    CPedGTA*                m_ped;
    CEventHandlerHistory    m_history;
    CTask*                  m_physicalResponseTask;
    CTask*                  m_eventResponseTask;
    CTask*                  m_attackTask;
    CTask*                  m_sayTask;
    CTask*                  m_partialAnimTask;

public:
    static void InjectHooks();

    explicit CEventHandler(CPedGTA* ped);
    CEventHandler* Constructor(CPedGTA* ped);
    ~CEventHandler() = default;

    void Flush();
    void FlushImmediately();

    eEventType GetCurrentEventType();
    void HandleEvents();
    bool IsKillTaskAppropriate(CPedGTA* ped1, CPedGTA* ped2, const CEvent& event);
    static bool IsTemporaryEvent(const CEvent& event);
    void RecordActiveEvent(const CEvent& event);
    void RecordPassiveEvent(const CEvent& event);
    static void RegisterKill(const CPedGTA* ped, const CEntityGTA* entity, eWeaponType weaponType, bool a4);
    void SetEventResponseTask(const CEvent& event);

    void ComputeAreaCodesResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeAttractorResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeBuildingCollisionPassiveResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeBuildingCollisionResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeCarUpsideDownResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeChatPartnerResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeCopCarBeingStolenResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeCreatePartnerTaskResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeDamageResponse(CEvent* event, CTask* task1, CTask* task2, CTask* task3);
    void ComputeDangerResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeDeadPedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeDeathResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeDontJoinGroupResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeDraggedOutCarResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeFireNearbyResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeGotKnockedOverByCarResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeGunAimedAtResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeHighAngerAtPlayerResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeInWaterResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeInteriorUseInfoResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeKnockOffBikeResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeLowAngerAtPlayerResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeLowHealthResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeObjectCollisionPassiveResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeObjectCollisionResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeOnEscalatorResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeOnFireResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePassObjectResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedCollisionWithPedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedCollisionWithPlayerResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedEnteredVehicleResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedFriendResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedSoundQuietResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedThreatBadlyLitResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedThreatResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedToChaseResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePedToFleeResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePersonalityResponseToDamage(CEventDamage* damageEvent, CEntityGTA* entity);
    void ComputePlayerCollisionWithPedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePlayerWantedLevelResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePotentialPedCollideResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputePotentialWalkIntoFireResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeReallyLowHealthResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeReviveResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeScriptCommandResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeSeenCopResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeSeenPanickedPedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeSexyPedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeSexyVehicleResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeShotFiredResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeShotFiredWhizzedByResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeSignalAtPedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeSpecialResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehicleCollisionResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehicleDamageResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehicleDiedResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehicleHitAndRunResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehicleOnFireResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehiclePotentialCollisionResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehiclePotentialPassiveCollisionResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeVehicleToStealResponse(CEvent* event, CTask* task1, CTask* task2);
    void ComputeWaterCannonResponse(CEvent* event, CTask* task1, CTask* task2);

    void ComputeEventResponseTask(CEvent* event, CTask* task);
    CTask* ComputeEventResponseTask(CPedGTA* ped, CEvent* event);

    auto& GetHistory() { return m_history; }
};

VALIDATE_SIZE(CEventHandler, (VER_x32 ? 0x34 : 0x60));
