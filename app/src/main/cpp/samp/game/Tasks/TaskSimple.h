/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Task.h"

struct CPedGTA;

class CTaskSimple : public CTask {
public:
    CTaskSimple();
    ~CTaskSimple() override = default;

    CTask* GetSubTask() override;
    bool IsSimple() override;
    virtual bool ProcessPed(CPedGTA* ped) = 0;
    virtual bool SetPedPosition(CPedGTA* ped);
};
static_assert(sizeof(CTaskSimple) == (VER_x32 ? 0x8 : 0x10));
