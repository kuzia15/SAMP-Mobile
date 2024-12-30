#include "../main.h"
#include "game.h"
#include "Streaming.h"

extern CGame* pGame;

// 0.3.7
CActor::CActor(int iSkin, float fX, float fY, float fZ, float fAngle)
{
    if (!CStreaming::TryLoadModel(iSkin))
        throw std::runtime_error("Model not loaded");

    if (!IsValidPedModel(iSkin))
    {
        iSkin = 0;
    }

    ScriptCommand(&create_actor, 5, iSkin, fX, fY, fZ, &m_dwGTAId);

    m_pPed = GamePool_Ped_GetAt(m_dwGTAId);

    ForceTargetRotation(fAngle);
    m_pPed->SetPosn(fX, fY, fZ);

    ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);
}
// 0.3.7
CActor::~CActor()
{
    auto modelId = m_pPed->m_nModelIndex;

    if (IsValidGamePed(m_pPed))
    {
        ((void (*)(CPedGTA*))(g_libGTASA + (VER_x32 ? 0x004CE6A0 + 1 : 0x5CDC64)))(m_pPed);
    }

    m_pPed = nullptr;
    m_dwGTAId = 0;

    CStreaming::RemoveModelIfNoRefs(modelId);
}
void CActor::ForceTargetRotation(float fRotation)
{

    if (!m_pPed) return;
    if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

    if (!IsValidGamePed(m_pPed))
    {
        return;
    }

    m_pPed->m_fCurrentRotation = DegToRad(fRotation);
    m_pPed->m_fAimingRotation = DegToRad(fRotation);

    ScriptCommand(&set_actor_z_angle, m_dwGTAId, fRotation);
}
// 0.3.7
void CActor::SetHealth(float fHealth)
{
	if (m_pPed) {
		m_pPed->m_fHealth = fHealth;
	}
}
// 0.3.7
void CActor::SetInvulnerable(bool bInvulnerable)
{
	m_bInvulnerable = bInvulnerable;

	if (bInvulnerable) {
		ScriptCommand(&set_actor_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
	}
	else {
		ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
	}
}
// 0.3.7 (adapted)
void CActor::ApplyAnimation(const char* szAnimName, const char* szAnimLib, float fDelta,
	int bLoop, int bLockX, int bLockY, int bFreeze, int iTime)
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if (!strcasecmp(szAnimLib, "SEX")) return;

	if (!pGame->IsAnimationLoaded(szAnimLib)) {
		pGame->RequestAnimation(szAnimLib);

        ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimLib, fDelta, bLoop, bLockX, bLockY, bFreeze, iTime);
		return;
	}

	ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimLib, fDelta, bLoop, bLockX, bLockY, bFreeze, iTime);
}
// 0.3.7
void CActor::ClearAnimation()
{
	if (m_pPed) {
		// CPedIntelligence::FlushImmediately
		//((void (*)(PED_TASKS_TYPE*, bool))(g_libGTASA + 0x4C0A44 + 1))(m_pPed->Tasks, true);
	}
}
// 0.3.7
void CActor::SetFacingAngle(float fAngle)
{
	if (m_pPed && GamePool_Ped_GetAt(m_dwGTAId)) {
		m_pPed->m_fAimingRotation = DegToRad(fAngle);
	}
}