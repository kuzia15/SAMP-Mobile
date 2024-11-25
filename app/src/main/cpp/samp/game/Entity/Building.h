/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CEntityGTA.h"

class CBuilding : public CEntityGTA {
public:
    CBuilding();
    static void* operator new(size_t size);
    static void operator delete(void* data);

public:
    static void InjectHooks();

    void ReplaceWithNewModel(int32 newModelIndex);
};

VALIDATE_SIZE(CBuilding, (VER_x32 ? 0x3C : 0x60));

bool IsBuildingPointerValid(CBuilding* building);

extern int32 gBuildings;
