//
// Created by x1y2z on 13.05.2023.
//

#include "World.h"
#include "../vendor/armhook/patch.h"
#include "main.h"
#include "game/game.h"
#include "net/netgame.h"

//CRepeatSector CWorld::ms_aRepeatSectors[MAX_REPEAT_SECTORS_Y][MAX_REPEAT_SECTORS_X];
//CPtrListDoubleLink CWorld::ms_listMovingEntityPtrs;

CSector* GetSector(int32 x, int32 y) {
    static CSector(&ms_aSectors)[MAX_SECTORS_Y][MAX_SECTORS_X] = *(CSector(*)[MAX_SECTORS_Y][MAX_SECTORS_X])(g_libGTASA + (VER_x32 ? 0x96B9F4 : 0xBDCB20));

    const auto x1 = std::clamp<int32>(x, 0, MAX_SECTORS_X - 1);
    const auto y1 = std::clamp<int32>(y, 0, MAX_SECTORS_Y - 1);

    return &ms_aSectors[y1][x1];
}

CRepeatSector* GetRepeatSector(int32 x, int32 y) {
    static CRepeatSector(&ms_aRepeatSectors)[MAX_REPEAT_SECTORS_Y][MAX_REPEAT_SECTORS_X] = *(CRepeatSector(*)[MAX_REPEAT_SECTORS_Y][MAX_REPEAT_SECTORS_X])(g_libGTASA + (VER_x32 ? 0x987BF4 : 0xC14F20));

    return &ms_aRepeatSectors[y % MAX_REPEAT_SECTORS_Y][x % MAX_REPEAT_SECTORS_X];
}

void CWorld::InjectHooks() {

    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x675C58 : 0x849A20), &ProcessPedsAfterPreRender);
}

bool CWorld::ProcessLineOfSight(const CVector* origin, const CVector* target, CColPoint* outColPoint, CEntityGTA** outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
   // assert(!origin.HasNanOrInf() && !target.HasNanOrInf()); // We're getting random nan/inf's from somewhere, so let's try to root cause it...
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x424B94 + 1 : 0x508C7C), origin, target, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck, doShootThroughCheck);
}
float CWorld::FindGroundZForCoord(float x, float y) {
    return CHook::CallFunction<float>(g_libGTASA + (VER_x32 ? 0x0042A7C4 + 1 : 0x50F3A0), x, y);
}

bool CWorld::GetIsLineOfSightClear(const CVector& origin, const CVector& target, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x00423468 + 1 : 0x5075A4), &origin, &target, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck);
}

void CWorld::Add(CEntityGTA *entity) {
    ((void(*)(CEntityGTA*))(g_libGTASA + (VER_x32 ? 0x00423418 + 1 : 0x507518)))(entity);
}

void CWorld::Remove(CEntityGTA *entity) {
    ((void (*)(CEntityGTA*))(g_libGTASA + (VER_x32 ? 0x0042330C + 1 : 0x5073A0))) (entity);
}

extern CNetGame *pNetGame;
void CWorld::ProcessPedsAfterPreRender() {
    if (CTimer::bSkipProcessThisFrame)
        return;

    if (pNetGame)
    {
        CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
        if (pPlayerPool)
        {
            pPlayerPool->ProcessAttachedObjects();
        }
    }
}
