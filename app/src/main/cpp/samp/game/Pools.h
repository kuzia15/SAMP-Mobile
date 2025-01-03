//
// Created by x1y2z on 17.04.2023.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include "game/Core/Pool.h"
#include "game/Entity/Object.h"
#include "game/Core/PtrNodeSingleLink.h"
#include "game/Core/PtrNodeDoubleLink.h"
#include "game/Tasks/Task.h"
#include "game/Core/EntryInfoNode.h"
#include "game/Entity/Dummy.h"
#include "game/Entity/Building.h"
#include "game/Entity/CopPed.h"
#include "game/Entity/Heli.h"
#include "game/Entity/CutsceneObject.h"
#include "game/Tasks/TaskTypes/TaskSimpleSlideToCoord.h"
#include "game/Entity/Heli.h"


class CPools {
public:
    static inline CPool<CPtrNodeSingleLink>*            ms_pPtrNodeSingleLinkPool;
    static inline CPool<CPtrNodeDoubleLink>*            ms_pPtrNodeDoubleLinkPool;
    static inline CPool<CPedGTA, CCopPed>*              ms_pPedPool;
    static inline CPool<CVehicleGTA, CHeli>*            ms_pVehiclePool;
    static inline CPool<CObjectGta, CCutsceneObject>*   ms_pObjectPool;
    static inline CPool<CTask, CTaskSimpleSlideToCoord>*ms_pTaskPool;
    static inline CPool<CEntryInfoNode>*                ms_pEntryInfoNodePool;
    static inline CPool<CDummy>*                        ms_pDummyPool;
    static inline CPool<CBuilding>*                     ms_pBuildingPool;

    static PoolAllocator::Pool*         ms_pColModelPool;
    static PoolAllocator::Pool*         ms_pEventPool;
    static PoolAllocator::Pool*         ms_pPointRoutePool;
    static PoolAllocator::Pool*         ms_pPatrolRoutePool;
    static PoolAllocator::Pool*         ms_pNodeRoutePool;
    static PoolAllocator::Pool*         ms_pTaskAllocatorPool;
    static PoolAllocator::Pool*         ms_pPedIntelligencePool;
    static PoolAllocator::Pool*         ms_pPedAttractorPool;

public:
    static void InjectHooks();

    static void Initialise();
};

static auto& GetPedPoolGta()                { return CPools::ms_pPedPool; }
static auto& GetVehiclePoolGta()            { return CPools::ms_pVehiclePool; }
static auto& GetBuildingPool()              { return CPools::ms_pBuildingPool; }
static auto& GetObjectPoolGta()             { return CPools::ms_pObjectPool; }
static auto& GetDummyPool()                 { return CPools::ms_pDummyPool; }
static auto& GetColModelPoolGta()           { return CPools::ms_pColModelPool; }
static auto& GetTaskPool()                  { return CPools::ms_pTaskPool; }
static auto& GetPedIntelligencePoolGta()    { return CPools::ms_pPedIntelligencePool; }
static auto& GetPtrNodeSingleLinkPool()     { return CPools::ms_pPtrNodeSingleLinkPool; }
static auto& GetPtrNodeDoubleLinkPool()     { return CPools::ms_pPtrNodeDoubleLinkPool; }
static auto& GetEntryInfoNodePool()         { return CPools::ms_pEntryInfoNodePool; }
static auto& GetPointRoutePoolGta()         { return CPools::ms_pPointRoutePool; }
static auto& GetPatrolRoutePoolGta()        { return CPools::ms_pPatrolRoutePool; }
static auto& GetEventPoolGta()              { return CPools::ms_pEventPool; }
static auto& GetNodeRoutePoolGta()          { return CPools::ms_pNodeRoutePool; }
static auto& GetTaskAllocatorPoolGta()      { return CPools::ms_pTaskAllocatorPool; }
static auto& GetPedAttractorPoolGta()       { return CPools::ms_pPedAttractorPool; }