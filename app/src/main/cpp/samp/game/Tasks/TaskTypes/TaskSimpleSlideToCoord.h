#pragma once

#include "Vector.h"
#include "TaskSimpleRunNamedAnim.h"

constexpr auto STAND_STILL_TIME = 20000; // 0x86DB24

class CTaskSimpleSlideToCoord : public CTaskSimpleRunNamedAnim {
public:
    CVector m_SlideToPos;
    float   m_fAimingRotation; // Heading
    float   m_fSpeed;
    bool    m_bFirstTime : 1;
    bool    m_bRunningAnim : 1;
    int32   m_Timer;

public:
    static constexpr auto Type = TASK_SIMPLE_SLIDE_TO_COORD;

    CTaskSimpleSlideToCoord(const CVector& slideToPos, float fAimingRotation, float speed);
    CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed, const char* animBlockName, const char* animGroupName, uint32 animFlags, float animBlendDelta, bool bRunInSequence, uint32 time);
    ~CTaskSimpleSlideToCoord() override = default;

    CTask* Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPedGTA* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPedGTA* ped) override;

private:
    static void InjectHooks();
};
VALIDATE_SIZE(CTaskSimpleSlideToCoord, (VER_x32 ? 0x80 : 0x90));
