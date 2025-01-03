//
// Created by x1y2z on 04.05.2023.
//

#pragma once

#include "common.h"
#include "game/Core/Rect.h"
#include "CPlayerInfoGta.h"
#include "RepeatSector.h"
#include "Sector.h"

constexpr int32 MAX_WORLD_UNITS = 6000;

constexpr int32 MAX_SECTORS_X = 120;
constexpr int32 MAX_SECTORS_Y = 120;
constexpr int32 MAX_SECTORS = MAX_SECTORS_X * MAX_SECTORS_Y;

constexpr int32 MAX_REPEAT_SECTORS_X = 16;
constexpr int32 MAX_REPEAT_SECTORS_Y = 16;
constexpr int32 MAX_REPEAT_SECTORS = MAX_REPEAT_SECTORS_X * MAX_REPEAT_SECTORS_Y;

constexpr int32 MAX_LOD_PTR_LISTS_X = 30;
constexpr int32 MAX_LOD_PTR_LISTS_Y = 30;
constexpr int32 MAX_LOD_PTR_LISTS = MAX_LOD_PTR_LISTS_X * MAX_LOD_PTR_LISTS_Y;

constexpr inline CRect WORLD_BOUNDS{-3000.0F, -3000.0F, 3000.0F, 3000.0F};
constexpr float MAP_Z_LOW_LIMIT = -100.0f;

class CWorld {
public:
    static inline CPlayerInfoGta Players[1004];
    static inline int PlayerInFocus;

    // Use GetRepeatSector() to access this array
    // static inline CRepeatSector ms_aRepeatSectors[MAX_REPEAT_SECTORS_Y][MAX_REPEAT_SECTORS_X];

    // Use GetSector() to access this array
    //static inline CSector ms_aSectors[MAX_SECTORS_Y][MAX_SECTORS_X];

 //   static CPtrListDoubleLink ms_listMovingEntityPtrs;

public:
    static void InjectHooks();

    static void Add(CEntityGTA* entity);
    static void Remove(CEntityGTA* entity);
    static void ProcessPedsAfterPreRender();

    static bool ProcessLineOfSight(const CVector* origin, const CVector* target, CColPoint* outColPoint, CEntityGTA** outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck);

    static void ClearScanCodes();
    static float FindGroundZForCoord(float x, float y);
    // Returns sector index in range -60 to 60 (Example: -3000 => -60, 3000 => 60)
    static float GetHalfMapSectorX(float x) { return x / static_cast<float>(MAX_WORLD_UNITS / MAX_SECTORS_X); }
    static float GetHalfMapSectorY(float y) { return y / static_cast<float>(MAX_WORLD_UNITS / MAX_SECTORS_Y); }

    // Returns sector index in range 0 to 120 (Example: -3000 => 0, 3000 => 120)
    static float GetSectorfX(float x) { return GetHalfMapSectorX(x) + static_cast<float>(MAX_SECTORS_X / 2); }
    static float GetSectorfY(float y) { return GetHalfMapSectorY(y) + static_cast<float>(MAX_SECTORS_Y / 2); }

    // returns sector index in range 0 to 120 (covers full map)
    static int32 GetSectorX(float x) { return static_cast<int32>(std::floor(GetSectorfX(x))); }
    static int32 GetSectorY(float y) { return static_cast<int32>(std::floor(GetSectorfY(y))); }

    static float GetSectorPosX(int32 sector)
    {
        constexpr auto HalfOfTotalSectorsX = MAX_SECTORS_X / 2;
        constexpr auto fTotalMapUnitsX = MAX_WORLD_UNITS / MAX_SECTORS_X;
        return static_cast<float>((sector - HalfOfTotalSectorsX) * fTotalMapUnitsX + (fTotalMapUnitsX / 2));
    }
    static float GetSectorPosY(int32 sector)
    {
        constexpr auto HalfOfTotalSectorsY = MAX_SECTORS_Y / 2;
        constexpr auto fTotalMapUnitsY = MAX_WORLD_UNITS / MAX_SECTORS_Y;
        return static_cast<float>((sector - HalfOfTotalSectorsY) * fTotalMapUnitsY + (fTotalMapUnitsY / 2));
    }

    static CVector2D GetSectorPos(int32 sector) { return { GetSectorPosX(sector), GetSectorPosY(sector) }; }

    static bool GetIsLineOfSightClear(const CVector& origin, const CVector& target, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck);
    // returns sector index in range 0 to 15 (covers half of the map)
    static float GetHalfMapLodSectorX(float sector) { return sector / static_cast<float>(MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_X); }
    static float GetHalfMapLodSectorY(float sector) { return sector / static_cast<float>(MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_Y); }
    static float GetLodSectorfX(float sector) { return GetHalfMapLodSectorX(sector) + static_cast<float>(MAX_LOD_PTR_LISTS_X / 2); }
    static float GetLodSectorfY(float sector) { return GetHalfMapLodSectorY(sector) + static_cast<float>(MAX_LOD_PTR_LISTS_Y / 2); }
    // returns sector index in range 0 to 30 (covers full map)
    static int32 GetLodSectorX(float fSector) { return static_cast<int32>(std::floor(GetLodSectorfX(fSector))); }
    static int32 GetLodSectorY(float fSector) { return static_cast<int32>(std::floor(GetLodSectorfY(fSector))); }
    static float GetLodSectorPosX(int32 sector)
    {
        const int32 HalfOfTotalSectorsX = MAX_LOD_PTR_LISTS_X / 2;
        const float fTotalMapUnitsX = static_cast<float>(MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_X);
        return (sector - HalfOfTotalSectorsX) * fTotalMapUnitsX + (fTotalMapUnitsX / 2);
    }
    static float GetLodSectorPosY(int32 sector)
    {
        const int32 HalfOfTotalSectorsY = MAX_LOD_PTR_LISTS_Y / 2;
        const float fTotalMapUnitsY = static_cast<float>(MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_Y);
        return (sector - HalfOfTotalSectorsY) * fTotalMapUnitsY + (fTotalMapUnitsY / 2);
    }
    static bool IsInWorldBounds(CVector2D pos) { // NOTSA
        return pos.x > -3000.0f && pos.x < 3000.0f
               && pos.y > -3000.0f && pos.y < 3000.0f;
    }

};

CSector* GetSector(int32 x, int32 y);
CRepeatSector* GetRepeatSector(int32 x, int32 y);


CPlayerInfoGta&   FindPlayerInfo(int32 playerId = -1);
CPlayerPedGta*    FindPlayerPed(int32 playerId = -1);
CVehicleGTA*      FindPlayerVehicle(int32 playerId = -1, bool bIncludeRemote = false);
bool              PlayerIsEnteringCar();