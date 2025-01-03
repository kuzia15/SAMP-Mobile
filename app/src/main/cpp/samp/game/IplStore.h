//
// Created by x1y2z on 15.11.2023.
//

#pragma once

#include "common.h"

class CIplStore {
public:

public:
    static void AddIplsNeededAtPosn(const CVector* posn);
    static void EnsureIplsAreInMemory(const CVector* posn);
    static void LoadIpls(CVector posn, bool bAvoidLoadInPlayerVehicleMovingDirection);

    static void RemoveIpl(int32 iplSlotIndex);
};

