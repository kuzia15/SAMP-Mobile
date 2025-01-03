//
// Created by x1y2z on 04.05.2023.
//

#include "PlayerPedGta.h"
#include "game/Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "game/Animation/AnimManager.h"
#include "../vendor/armhook/patch.h"

void CPlayerPedGta::ReApplyMoveAnims() {
   // Log("ReApplyMoveAnims");

    constexpr AnimationId anims[]{
            ANIM_ID_WALK,
            ANIM_ID_RUN,
            ANIM_ID_SPRINT,
            ANIM_ID_IDLE,
            ANIM_ID_WALK_START
    };
    for (auto & id : anims) {
        if (auto anim = RpAnimBlendClumpGetAssociation(this->m_pRwClump, id)) {
            if (anim->GetHashKey() != CAnimManager::GetAnimAssociation(m_nAnimGroup, id)->GetHashKey()) {
                CAnimBlendAssociation* addedAnim = CAnimManager::AddAnimation(m_pRwClump, m_nAnimGroup, id);
                addedAnim->m_fBlendDelta = anim->m_fBlendDelta;
                addedAnim->m_fBlendAmount = anim->m_fBlendAmount;

                anim->m_nFlags |= ANIMATION_FREEZE_LAST_FRAME;
                anim->m_fBlendDelta = -1000.0f;
            }
        }
    }
}


// --- hooks

void ReApplyMoveAnims_hook(CPlayerPedGta* thiz) {
    thiz->ReApplyMoveAnims();
}

void CPlayerPedGta::InjectHooks() {
    CHook::Redirect("_ZN10CPlayerPed16ReApplyMoveAnimsEv", &ReApplyMoveAnims_hook);
}
