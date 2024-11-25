/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Task.h"

struct CPedGTA;

class CTaskComplex : public CTask {
public:
    CTask* m_pSubTask;

public:
    CTaskComplex();
    ~CTaskComplex() override;

    CTask* GetSubTask() override;
    bool IsSimple() override;
    bool MakeAbortable(CPedGTA* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    virtual void SetSubTask(CTask* subTask);
    virtual CTask* CreateNextSubTask(CPedGTA* ped) = 0;
    virtual CTask* CreateFirstSubTask(CPedGTA* ped) = 0;
    virtual CTask* ControlSubTask(CPedGTA* ped) = 0;
    // #vtable: 11
};
static_assert(sizeof(CTaskComplex) == (VER_x32 ? 0xC : 0x18));
