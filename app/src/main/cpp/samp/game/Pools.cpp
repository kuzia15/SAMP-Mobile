//
// Created by plaka on 17.04.2023.
//

#include "Pools.h"
#include "IplDef.h"
#include "../vendor/armhook/patch.h"
#include "game/Entity/CVehicleGTA.h"
#include "game/Entity/Building.h"
#include "game/Entity/CopPed.h"
#include "game/Entity/Heli.h"
#include "game/Entity/Dummy.h"
#include "game/Entity/Object.h"
#include "game/Entity/CutsceneObject.h"

PoolAllocator::Pool*      CPools::ms_pColModelPool;
PoolAllocator::Pool*      CPools::ms_pEventPool;
PoolAllocator::Pool*      CPools::ms_pPointRoutePool;
PoolAllocator::Pool*      CPools::ms_pPatrolRoutePool;
PoolAllocator::Pool*      CPools::ms_pNodeRoutePool;
PoolAllocator::Pool*      CPools::ms_pTaskAllocatorPool;
PoolAllocator::Pool*      CPools::ms_pPedIntelligencePool;
PoolAllocator::Pool*      CPools::ms_pPedAttractorPool;


void CPools::Initialise()
{
    FLog("CPools::Initialise");

    CPools::ms_pPtrNodeSingleLinkPool   = new CPool<CPtrNodeSingleLink>(100000, "PtrNode Single");
    CPools::ms_pPtrNodeDoubleLinkPool   = new CPool<CPtrNodeDoubleLink>(60000, "PtrNode Double");
    CPools::ms_pPedPool                 = new CPool<CPedGTA, CCopPed>(240, "Peds");
    CPools::ms_pVehiclePool             = new CPool<CVehicleGTA, CHeli>(1000, "Vehicles");
    CPools::ms_pObjectPool              = new CPool<CObjectGta, CCutsceneObject>(3000, "Objects");
    CPools::ms_pTaskPool                = new CPool<CTask, CTaskSimpleSlideToCoord>(6000, "Task");
    CPools::ms_pEntryInfoNodePool       = new CPool<CEntryInfoNode>(20000, "EntryInfoNodePool");
    CPools::ms_pDummyPool               = new CPool<CDummy>(40000, "Dummies");
    CPools::ms_pBuildingPool            = new CPool<CBuilding>(20000, "Buildings");

    CPools::ms_pColModelPool = PoolAllocator::Allocate(50000, (VER_x32 ? 0x30 : 0x38));
    // 13600 / 200 = 68
    CPools::ms_pEventPool = PoolAllocator::Allocate(1000, (VER_x32 ? 0x44 : 0x58));
    // 6400 / 64 = 100
    CPools::ms_pPointRoutePool = PoolAllocator::Allocate(200, (VER_x32 ? 0x64 : 0x64));
    // 13440 / 32 = 420
    CPools::ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
    // 2304 / 64 = 36
    CPools::ms_pNodeRoutePool = PoolAllocator::Allocate(200, (VER_x32 ? 0x24 : 0x24));
    // 512 / 16 = 32
    CPools::ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
    // 92960 / 140 = 664
    CPools::ms_pPedIntelligencePool = PoolAllocator::Allocate(240, (VER_x32 ? 0x298 : 0x440));
    // 15104 / 64 = 236
    CPools::ms_pPedAttractorPool = PoolAllocator::Allocate(200, (VER_x32 ? 0xEC : 0xEC));

    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC58 : 0xBC3BE0)) = CPools::ms_pColModelPool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC60 : 0xBC3BF0)) = CPools::ms_pEventPool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC64 : 0xBC3BF8)) = CPools::ms_pPointRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC68 : 0xBC3C00)) = CPools::ms_pPatrolRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC6C : 0xBC3C08)) = CPools::ms_pNodeRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC70 : 0xBC3C10)) = CPools::ms_pTaskAllocatorPool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC74 : 0xBC3C18)) = CPools::ms_pPedIntelligencePool;
    *(PoolAllocator::Pool**)(g_libGTASA + (VER_x32 ? 0x0095AC78 : 0xBC3C20)) = CPools::ms_pPedAttractorPool;
}

void CPools::InjectHooks() {
    CHook::Redirect("_ZN6CPools10InitialiseEv", &CPools::Initialise);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678EC4 : 0x84FDB8), &CPools::ms_pBuildingPool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676B1C : 0x84B698), &CPools::ms_pDummyPool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679C1C : 0x851850), &CPools::ms_pEntryInfoNodePool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677F0C : 0x84DE48), &CPools::ms_pPtrNodeSingleLinkPool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678364 : 0x84E6F0), &CPools::ms_pPtrNodeDoubleLinkPool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676C84 : 0x84B968), &CPools::ms_pPedPool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678534 : 0x84EA90), &CPools::ms_pVehiclePool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676BB0 : 0x84B7C0), &CPools::ms_pObjectPool);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676158 : 0x84A330), &CPools::ms_pTaskPool);
}
