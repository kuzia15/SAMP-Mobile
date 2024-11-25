/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "../TaskSimple.h"
class CAnimBlendAssociation;

class CTaskSimpleAnim : public CTaskSimple {
public:
    CAnimBlendAssociation *m_pAnim;
    union {
        struct {
            bool m_bIsFinished : 1;
            bool m_bDontInterrupt : 1;
            bool m_bHoldLastFrame : 1;

            // These flags are used in CTaskSimpleRunAnim only
            bool m_bDontBlendOut : 1;

            // These flags are used in 	CTaskSimpleRunNamedAnim only
            bool m_bRunInSequence : 1;
            bool m_bOffsetAtEnd : 1;
            bool m_bOffsetAvailable : 1;
        };
        uint8_t m_nFlags;
    };

public:
    explicit CTaskSimpleAnim(bool bHoldLastFrame);
    ~CTaskSimpleAnim() override;

    bool MakeAbortable(CPedGTA* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    static void FinishRunAnimCB(CAnimBlendAssociation* blendAssoc, void* data); //data is CTaskSimpleAnim

    static void InjectHooks();
    bool MakeAbortable_Reversed(CPedGTA* ped, eAbortPriority priority, const CEvent* event);
};
static_assert(sizeof(CTaskSimpleAnim) == (VER_x32 ? 0x10 : 0x20));