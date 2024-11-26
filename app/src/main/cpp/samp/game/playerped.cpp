#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../vendor/armhook/patch.h"
#include <cmath>
// 0.3.7
#include "Entity/CPedGTA.h"
#include "Streaming.h"
#include "World.h"
#include "game/Models/ModelInfo.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

bool bIgnoreNextEntry = false;

CPlayerPed::CPlayerPed()
{
	m_dwGTAId = 1;
	m_pPed = GamePool_FindPlayerPed();
	m_bytePlayerNumber = 0;
	SetPlayerPedPtrRecord(m_bytePlayerNumber, reinterpret_cast<CPedGTA *>(m_pPed));
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);

	m_iCuffedState = 0;
	m_iCarryState = 0;

	m_dwArrow = 0;
	m_dwParachuteObject = 0;

	iSpecialAction = 0;

	m_iDanceStyle = -1;
	m_iCellPhoneEnabled = 0;
	m_bPissingState = false;

	m_stuffData.dwDrunkLevel = 0;
	m_stuffData.dwObject = 0;
	m_stuffData.type = eStuffType::STUFF_TYPE_NONE;
	m_stuffData.dwLastUpdateTick = 0;

	for (int i = 0; i < 10; i++)
	{
		m_bObjectSlotUsed[i] = false;
		memset(&m_attachedObjectInfo[i], 0, sizeof(NEW_ATTACHED_OBJECT));
		m_pAttachedObjects[i] = nullptr;
	}

	m_bHaveBulletData = false;
}

CPlayerPed::CPlayerPed(int iNum, int iSkin, float fX, float fY, float fZ, float fRotation)
{
	FLog("Creating PlayerPed(%d)", iNum);
    FLog("Attempting to create player: iPlayerNum=%d, fX=%f, fY=%f, fZ=%f, m_dwGTAId=%p", iNum, fX, fY, fZ, &m_dwGTAId);
	uint32_t dwPlayerActorID;
    static int iPlayerNum;
    iPlayerNum = iNum;

    m_pPed = nullptr;
    m_dwGTAId = 0;
    m_dwArrow = 0;
    m_bHaveBulletData = false;

    /*if (!ScriptCommand(&create_player, &iPlayerNum, fX, fY, fZ, &m_dwGTAId)) {
        FLog("Error: Failed to create player!");
        return;
    }*/

    //458FC0 ; CPlayerPed::SetupPlayerPed(int)
    ((void (*)(int))(g_libGTASA + (VER_x32 ? 0x4C39A4 + 1:0x5C0FD4)))(iPlayerNum); // CPlayerPed::SetupPlayerPed
    //4543D4 ; CPlayerPed::DeactivatePlayerPed
    ((void (*)(int))(g_libGTASA + (VER_x32 ? 0x4C3AD4+1: 0x5C1140)))(iPlayerNum); // CPlayerPed::DeactivatePlayerPed
    //3AC5DC ; FindPlayerPed(int)
    m_pPed = FindPlayerPed(iNum); // CPlayerPed::FindPlayerPed

    CVector posn = {fX, fY, fZ};
    ((void (*)(const CVector&, CEntityGTA*))(g_libGTASA + (VER_x32 ? 0x34DA34+1: 0x419BE0)))(posn, m_pPed); // CTheScripts::ClearSpaceForMissionEntity
    //4543F8 ; CPlayerPed::ReactivatePlayerPed
    ((void (*)(int))(g_libGTASA+(VER_x32 ? 0x4C3AEC + 1:0x5C1158)))(iPlayerNum); // CPlayerPed::ReactivatePlayerPed
    //((void (*)(ENTITY_TYPE*))(SA_ADDR(0x3C14B0 + 1)))((ENTITY_TYPE*)m_pPed); // CWorld::Add
    CWorld::Add(m_pPed);

	//ScriptCommand(&create_actor_from_player, &iPlayerNum, &m_dwGTAId);

    m_dwGTAId = GamePool_Ped_GetIndex(m_pPed);
    FLog("m_dwGTAId=%p",m_dwGTAId);

	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
    if (!m_pPed) {
        FLog("Error: Invalid m_pPed after GamePool_Ped_GetAt!");
        return;
    }
	m_bytePlayerNumber = iPlayerNum;

	SetPlayerPedPtrRecord(iPlayerNum, m_pPed);

    m_pPed->bDoesntDropWeaponsWhenDead = true;
	ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 1, 0, 0);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);
    m_pPed->bNeverEverTargetThisPed = true;

	if (pNetGame)
	{
		ScriptCommand(&set_actor_money, m_dwGTAId, 0);
		ScriptCommand(&set_actor_money, m_dwGTAId, pNetGame->m_pNetSet->iDeathDropMoney);
	}

	m_iCuffedState = 0;
	m_iCarryState = 0;

	m_dwArrow = 0;
	m_dwParachuteObject = 0;

	m_iDanceStyle = -1;
	m_iCellPhoneEnabled = 0;
	m_bPissingState = false;

	iSpecialAction = 0;

	SetModelIndex(iSkin);

    FLog("CPlayerPed1");

	// GameResetPlayerKeys
    memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));

    FLog("CPlayerPed2");
    SetTargetRotation(fRotation);
    RwMatrix mat = m_pPed->GetMatrix().ToRwMatrix();
    mat.pos.x = fX;
    mat.pos.y = fY;
    mat.pos.z = fZ+ 0.15f;
    m_pPed->SetMatrix((CMatrix&)mat);

	m_stuffData.dwDrunkLevel = 0;
	m_stuffData.dwObject = 0;
	m_stuffData.type = eStuffType::STUFF_TYPE_NONE;
	m_stuffData.dwLastUpdateTick = 0;

	for (int i = 0; i < 10; i++)
	{
		m_bObjectSlotUsed[i] = false;
		memset(&m_attachedObjectInfo[i], 0, sizeof(NEW_ATTACHED_OBJECT));
		m_pAttachedObjects[i] = nullptr;
	}

	m_bHaveBulletData = false;
    FLog("CPlayerPed3");
    FLog("m_pPed: %p, m_dwGTAId: %d", m_pPed, m_dwGTAId);
}

CPlayerPed::~CPlayerPed()
{
    auto modelId = m_pPed->m_nModelIndex;
	FLog("Destroying PlayerPed(%d)", m_bytePlayerNumber);

	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));
	// GameResetPlayerKeys
	SetPlayerPedPtrRecord(m_bytePlayerNumber, 0);

	if (m_pPed && (GamePool_Ped_GetAt(m_dwGTAId) != 0) && IsValidGamePed(m_pPed) /* CPlaceable */)
	{
		if (m_dwParachuteObject)
		{
			ScriptCommand(&disassociate_object, m_dwParachuteObject, 0.0, 0.0, 0.0, 0);
			ScriptCommand(&destroy_object_with_fade, m_dwParachuteObject);
			m_dwParachuteObject = 0;
		}

		if (IsHaveAttachedObject())
			RemoveAllAttachedObjects();

		// if(field_2C1)

		if (IsInVehicle()) {
			RemoveFromVehicleAndPutAt(100.0f, 100.0f, 20.0f);
		}

		uintptr_t dwPedPtr = (uintptr_t)m_pPed;
        m_pPed->m_pPlayerData->m_nPlayerGroup = 0;
		// CPlayerPed::Destructor
		//((void (*)(CPedGTA*))(*(void**)(m_pPed->entity.vtable + 0x4)))(m_pPed);

		// CPopulation::RemovePed
        ((void (*)(uintptr_t))(g_libGTASA + (VER_x32 ? 0x004CE6A0 + 1 : 0x5CDC64)))((uintptr_t)m_pPed);

		m_pPed = nullptr;
	}
	else
	{
		m_pPed = nullptr;
		m_dwGTAId = 0;

	}

    CStreaming::RemoveModelIfNoRefs(modelId);
}


bool CPlayerPed::IsInVehicle()
{
	if (!m_pPed) return false;

	return m_pPed->IsInVehicle();
}
// 0.3.7
bool CPlayerPed::IsAPassenger()
{
    if (!m_pPed) return false;

    return m_pPed->IsAPassenger();
}
// 0.3.7
void CPlayerPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
    if(!m_pPed || !m_pPed->pVehicle) return;

    m_pPed->m_pIntelligence->m_TaskMgr.FlushImmediately();

    CVector p = CVector(fX, fY, fZ);
    m_pPed->SetPosn(p);

    m_pPed->UpdateRW();
    m_pPed->UpdateRwFrame();//? ���� ��
}
// 0.3.7
uint8_t CPlayerPed::GetActionTrigger()
{
    return (uint8_t)m_pPed->m_nPedState;
}

void CPlayerPed::SetActionTrigger(uint8_t action)
{
	m_pPed->m_nPedState = (ePedState)action;
}

void CPlayerPed::SetDead()
{
	RwMatrix mat;

	if (m_dwGTAId && m_pPed) {
		if (!m_pPed->IsInVehicle())
		{

		}

		ExtinguishFire();
        mat = m_pPed->GetMatrix().ToRwMatrix();
        m_pPed->SetPosn(mat.pos.x, mat.pos.y, mat.pos.z);
		m_pPed->m_fHealth = 0.0f;
        uint8_t old = CWorld::PlayerInFocus;
        CWorld::PlayerInFocus = m_bytePlayerNumber;
        ScriptCommand(&kill_actor, m_dwGTAId);
        CWorld::PlayerInFocus = 0;
	}
}
// 0.3.7
bool CPlayerPed::IsDead()
{
	if (!m_pPed) return true;
	if (m_pPed->m_fHealth > 0.0f) return false;
	return true;
}
// 0.3.7
void CPlayerPed::TogglePlayerControllable(bool bControllable)
{
    FLog("TogglePlayerControllable");
    if (!m_pPed) return;
    FLog("TogglePlayerControllable2");
    if(!m_dwGTAId)return;
    FLog("TogglePlayerControllable3");
    if (!IsValidGamePed(m_pPed) || !GamePool_Ped_GetAt(m_dwGTAId)) {
        return;
    }
    FLog("TogglePlayerControllable4");
    //CHUD::bIsDisableControll = !bToggle;
    if(!bControllable)
    {
        FLog("TogglePlayerControllable5");
        ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 0);
        ScriptCommand(&lock_actor, m_dwGTAId, 1);
    }
    else if(bControllable)
    {
        FLog("TogglePlayerControllable6");
        ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 1);
        ScriptCommand(&lock_actor, m_dwGTAId, 0);
    }
}
// 0.3.7
float CPlayerPed::GetHealth()
{
	if (!m_pPed) return 0.0f;
	return m_pPed->m_fHealth;
}
// 0.3.7
void CPlayerPed::SetHealth(float fHealth)
{
	if (m_pPed) {
		m_pPed->m_fHealth = fHealth;
	}
}
// 0.3.7
float CPlayerPed::GetArmour()
{
	if (!m_pPed) return 0.0f;
	return m_pPed->m_fArmour;
}
// 0.3.7
void CPlayerPed::SetArmour(float fArmour)
{
	if (!m_pPed) return;
	m_pPed->m_fArmour = fArmour;
}
// 0.3.7
CVehicleGTA* CPlayerPed::GetGtaVehicle()
{
    return (CVehicleGTA*)m_pPed->pVehicle;
}

void CPlayerPed::ExtinguishFire()
{

}

void CPlayerPed::DrunkProcess()
{

}

void CPlayerPed::SatisfyHunger()
{

}

bool IsTaskRunNamedOrSlideToCoord(void* pTask)
{

	uintptr_t dwVTable = *(uintptr_t*)(pTask);
	if (dwVTable == (g_libGTASA + 0x66C4E0) || dwVTable == (g_libGTASA + 0x6694F0)) // CTaskSimpleSlideToCoord CTaskSimpleRunNamedAnim
	{
		return true;
	}
	return false;
}

void* GetSubTaskFromTask(CPedIntelligence* pTask)
{
	uintptr_t pVTableTask = *((uintptr_t*)pTask);
	return ((void* (*)(void*))(*(void**)(pVTableTask + 12)))(pTask);
}

uint32_t CPlayerPed::GetCurrentAnimationIndex()
{
	/*int blendData = 4.0f;

	if (!m_pPed || !m_dwGTAId)
	{
		return 0;
	}

	if (!GamePool_Ped_GetAt(m_dwGTAId))
	{
		return 0;
	}

	if (!m_pPed->m_pRwObject)
	{
		return 0;
	}
	sizeof(CPedGTA);
	CPedIntelligence* pIntelligence = m_pPed->m_pIntelligence;

	if (pIntelligence)
	{
		void* pTask = pIntelligence->m_TaskMgr.m_aPrimaryTasks[3];

		if (pTask)
		{
			while (!IsTaskRunNamedOrSlideToCoord(pTask))
			{
				pTask = GetSubTaskFromTask(pTask);
				if (!pTask)
				{
					return 0;
				}
			}

			const char* szName = (const char*)((uintptr_t)pTask + 13);
			const char* szGroupName = (const char*)((uintptr_t)pTask + 37);

			std::string szStr = std::string(szGroupName);
			szStr += ":";
			szStr += szName;

			int idx = GetAnimIdxByName(szStr.c_str());
			if (idx == -1)
			{
				return 0;
			}
			else
			{
				return idx + 1;
			}
		}
	}*/
	return 0;
}

bool CPlayerPed::IsPlayingAnimation(int iIndex)
{
	if (!m_pPed || !m_dwGTAId || (iIndex == 0) )
	{
		return 0;
	}
	if (!GamePool_Ped_GetAt(m_dwGTAId))
	{
		return 0;
	}
	if (!m_pPed->m_pRwObject)
	{
		return 0;
	}

	const char* pAnim = GetAnimByIdx(iIndex - 1);
	if (!pAnim)
	{
		return false;
	}
	const char* pNameAnim = strchr(pAnim, ':') + 1;

    //RpAnimBlendClumpGetAssociation(RpClump *,char const*)	000000000046AAF4
	uintptr_t blendAssoc = ((uintptr_t(*)(RpClump* clump, const char* szName))(g_libGTASA + (VER_x32 ? 0x00390A24 + 1:0x46AAF4)))
			(m_pPed->m_pRwClump, pNameAnim);	// RpAnimBlendClumpGetAssociation

	if (blendAssoc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsBlendAssocGroupLoaded(int iGroup)
{
	uintptr_t* pBlendAssocGroup = *(uintptr_t * *)(g_libGTASA + (VER_x32 ? 0x00942184 : 0xBA88A8)); // CAnimManager::ms_aAnimAssocGroups
	uintptr_t blendAssoc = (uintptr_t)pBlendAssocGroup;
	blendAssoc += (iGroup * 20);
	pBlendAssocGroup = (uintptr_t*)blendAssoc;
	return *(pBlendAssocGroup) != NULL;
}

void CPlayerPed::PlayAnimationFromIndex(int iIndex, float fDelta)
{
	if (!iIndex)
	{
		RwMatrix mat;
        mat = m_pPed->GetMatrix().ToRwMatrix();
        m_pPed->SetPosn(mat.pos.x, mat.pos.y, mat.pos.z);
		return;
	}
	std::string szAnim;
	std::string szBlock;

	char pszAnim[40];
	char pszBlock[40];

	memset(&pszAnim[0], 0, 40);
	memset(&pszBlock[0], 0, 40);

	bool bTest = false;
	const char* pBegin = GetAnimByIdx(iIndex - 1);
	if (!pBegin)
	{
		return;
	}
	while (*pBegin)
	{
		if (*pBegin == ':')
		{
			pBegin++;
			bTest = true;
			continue;
		}
		if (!bTest)
		{
			szBlock += *pBegin;
		}
		if (bTest)
		{
			szAnim += *pBegin;
		}
		pBegin++;
	}

	strcpy(&pszAnim[0], szAnim.c_str());
	strcpy(&pszBlock[0], szBlock.c_str());
	ApplyAnimation(&pszAnim[0], &pszBlock[0], fDelta, 0, 1, 1, 0, 0);
}

// 0.3.7
uint8_t CPlayerPed::GetCurrentWeapon()
{
    return GetCurrentWeaponSlot()->dwType;
}
// 0.3.7
void CPlayerPed::RemoveWeaponWhenEnteringVehicle()
{
	if (m_pPed) {
		// CPed::RemoveWeaponWhenEnteringVehicle
		//((void(*)(CPedGTA*, int))(g_libGTASA + 0x4A52FC + 1))(m_pPed, 0);
	}
}
// 0.3.7
void CPlayerPed::SetInitialState()
{
	// CPlayerPed::SetInitialState
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x004C37B4 + 1 : 0x5C0D50), m_pPed);
}
// 0.3.7
void CPlayerPed::RestartIfWastedAt(CVector *vecRestart, float fRotation)
{
	ScriptCommand(&restart_if_wasted_at, vecRestart->x, vecRestart->y, vecRestart->z, fRotation, 0);
}

bool IsPedModel(unsigned int iModelID)
{
    if(iModelID < 0 || iModelID > 20000) return false;
    auto dwModelArray = CModelInfo::ms_modelInfoPtrs;

    uintptr_t ModelInfo = reinterpret_cast<uintptr_t>(dwModelArray[iModelID]);
    if(ModelInfo && *(uintptr_t*)ModelInfo == (uintptr_t)g_libGTASA + (VER_x32 ? 0x00667658 : 0x82F310))
        return true;

    return false;
}
// 0.3.7
void CPlayerPed::SetModelIndex(uint uiModel)
{
    if(!GamePool_Ped_GetAt(m_dwGTAId)) return;
    if(!IsPedModel(uiModel))
        uiModel = 0;

    if(m_pPed)
    {
        auto oldModelId = m_pPed->m_nModelIndex;

        if (!CStreaming::TryLoadModel(uiModel))
            return;

        // CEntity::DeleteRwObject();
        m_pPed->m_nModelIndex = uiModel;

        m_pPed->SetModelIndex(uiModel);

        CStreaming::RemoveModelIfNoRefs(oldModelId);
    }
}

void CPlayerPed::ClearWeapons()
{
	if (m_pPed == nullptr) return;

	CWorld::PlayerInFocus = m_bytePlayerNumber; // CWorld::PlayerInFocus
	((void (*)(CPedGTA*))(g_libGTASA + (VER_x32 ? 0x0049F836 + 1 : 0x595604)))(m_pPed); // CPed::ClearWeapons
    CWorld::PlayerInFocus = 0;
}

void CPlayerPed::ResetDamageEntity()
{
	if(!m_pPed) return;

	m_pPed->m_pLastEntityDamage = nullptr;
}

void CPlayerPed::GiveWeapon(int iWeaponId, int iAmmo)
{
	if(m_pPed)
        m_pPed->GiveWeapon(iWeaponId, iAmmo);
}

void CPlayerPed::SetArmedWeapon(uint8_t weapon, bool unk)
{
}
// 0.3.7
void CPlayerPed::SetTargetRotation(float fRotation)
{
	if (m_pPed && GamePool_Ped_GetAt(m_dwGTAId))
	{
		m_pPed->m_fAimingRotation = DegToRad(fRotation);
		m_pPed->m_fCurrentRotation = DegToRad(fRotation);
		ScriptCommand(&set_actor_z_angle, m_dwGTAId, fRotation);
	}
}
// 0.3.7
void CPlayerPed::SetImmunities(int BP, int FP, int EP, int CP, int MP)
{
	if (m_pPed) {
		if (GamePool_Ped_GetAt(m_dwGTAId)) {
			ScriptCommand(&set_actor_immunities, m_dwGTAId, BP, FP, EP, CP, MP);
		}
	}
}
// 0.3.7
void CPlayerPed::ShowMarker(int nIndex)
{
	if (m_dwArrow) {
		ScriptCommand(&disable_marker, m_dwArrow);
		m_dwArrow = 0;
	}

	ScriptCommand(&create_arrow_above_actor, m_dwGTAId, &m_dwArrow);
	ScriptCommand(&set_marker_color, m_dwArrow, nIndex);
	ScriptCommand(&show_on_radar2, m_dwArrow, 2);
}

void CPlayerPed::SetKeys(uint16_t lrAnalog, uint16_t udAnalog, uint16_t wKeys)
{
	PAD_KEYS *pad = &RemotePlayerKeys[m_bytePlayerNumber];

	// LEFT/RIGHT
	pad->wKeyLR = lrAnalog;
	// UP/DOWN
	pad->wKeyUD = udAnalog;

	// KEY_ACTION
	pad->bKeys[ePadKeys::KEY_ACTION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_CROUCH
	pad->bKeys[ePadKeys::KEY_CROUCH] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_FIRE
	pad->bKeys[ePadKeys::KEY_FIRE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SPRINT
	pad->bKeys[ePadKeys::KEY_SPRINT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SECONDARY_ATTACK
	pad->bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_JUMP
	pad->bKeys[ePadKeys::KEY_JUMP] = (wKeys & 1);
	if(!pad->bKeys[ePadKeys::KEY_JUMP]) pad->bIgnoreJump = false;
	wKeys >>= 1;
	// KEY_LOOK_RIGHT
	pad->bKeys[ePadKeys::KEY_LOOK_RIGHT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_HANDBRAKE
	pad->bKeys[ePadKeys::KEY_HANDBRAKE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_LOOK_LEFT
	pad->bKeys[ePadKeys::KEY_LOOK_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SUBMISSION
	pad->bKeys[ePadKeys::KEY_SUBMISSION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_WALK
	pad->bKeys[ePadKeys::KEY_WALK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_UP
	pad->bKeys[ePadKeys::KEY_ANALOG_UP] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_DOWN
	pad->bKeys[ePadKeys::KEY_ANALOG_DOWN] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_LEFT
	pad->bKeys[ePadKeys::KEY_ANALOG_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_RIGHT
	pad->bKeys[ePadKeys::KEY_ANALOG_RIGHT] = (wKeys & 1);

	return;
}

uint8_t CPlayerPed::GetAdditionalKeys()
{
	uint8_t byteRet = 0;

	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_YES])
	{
		byteRet = 1;
		LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = false;
	}

	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_NO])
	{
		byteRet = 2;
		LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = false;
	}

	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK])
	{
		byteRet = 3;
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = false;
	}

	return byteRet;
}

uint16_t CPlayerPed::GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog, bool clear)
{
	*lrAnalog = LocalPlayerKeys.wKeyLR;
	*udAnalog = LocalPlayerKeys.wKeyUD;

	uint16_t wRet = 0;

	// KEY_ANALOG_RIGHT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_LEFT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_DOWN
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_DOWN]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_UP
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_UP]) wRet |= 1;
	wRet <<= 1;
	// KEY_WALK
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SUBMISSION
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_LEFT
	if (IsInVehicle() && LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_HANDBRAKE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE]/*true*/) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_RIGHT
	if(IsInVehicle() && LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_JUMP
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) wRet |= 1;
	wRet <<= 1;
	// KEY_SECONDARY_ATTACK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SPRINT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT]) wRet |= 1;
	wRet <<= 1;
	// KEY_FIRE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]) wRet |= 1;
	wRet <<= 1;
	// KEY_CROUCH
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH]) wRet |= 1;
	wRet <<= 1;
	// KEY_ACTION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION]) wRet |= 1;

	if(clear) memset(LocalPlayerKeys.bKeys, 0, ePadKeys::SIZE);

	return wRet;
}

// 0.3.7
void CPlayerPed::SetFightingStyle(int iStyle)
{
	if (m_pPed) {
		ScriptCommand(&set_fighting_style, m_dwGTAId, iStyle, 6);
	}
}

// 0.3.7
void CPlayerPed::SetRotation(float fRotation)
{
	if (m_pPed)
	{
		if (GamePool_Ped_GetAt(m_dwGTAId))
		{
			m_pPed->m_fAimingRotation = DegToRad(fRotation);
			m_pPed->m_fCurrentRotation = DegToRad(fRotation);
		}
	}
}

void CPlayerPed::DestroyFollowPedTask()
{

}

// 0.3.7
void CPlayerPed::GetBonePosition(int iBoneID, CVector* vecOut)
{
	m_pPed->GetBonePosition(vecOut, iBoneID, 0);
}
// 0.3.7
void CPlayerPed::GetTransformedBonePosition(int iBoneID, CVector* vecOut)
{
	if (!m_pPed) return;

	// CPed::GetTransformedBonePosition
	((void (*)(CPedGTA*, CVector*, int, int))(g_libGTASA + (VER_x32 ? 0x4A24A8 + 1: 0x598670)))(m_pPed, vecOut, iBoneID, 0);
}

void CPlayerPed::ApplyAnimation(const char* szAnimName, const char* szAnimLib, float fT, int opt1, int opt2, int opt3, int opt4, int iTime)
{
	int iWaitAnimLoad = 0;

	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!strcasecmp(szAnimLib,"SEX")) return;
	if(!strcasecmp(szAnimLib,"SAMP")) return;

	if(!pGame->IsAnimationLoaded(szAnimLib))
	{
		pGame->RequestAnimation(szAnimLib);
        ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimLib, fT, opt1, opt2, opt3, opt4, iTime);
	}

	ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimLib, fT, opt1, opt2, opt3, opt4, iTime);
}

// 0.3.7
void CPlayerPed::SetInterior(uint8_t byteInteriorId, bool bRefresh)
{
	if (m_pPed && m_bytePlayerNumber != 0) {
		ScriptCommand(&link_actor_to_interior, m_dwGTAId, byteInteriorId);
	}
	else
	{
		ScriptCommand(&select_interior, byteInteriorId);
		ScriptCommand(&link_actor_to_interior, m_dwGTAId, byteInteriorId);
		if (bRefresh)
		{
			RwMatrix mat;
            mat = m_pPed->GetMatrix().ToRwMatrix();
			ScriptCommand(&refresh_streaming_at, mat.pos.x, mat.pos.y);
		}
	}
}

// 0.3.7
CAMERA_AIM* CPlayerPed::GetCurrentAim()
{
	return GameGetInternalAim();
}
// 0.3.7
void CPlayerPed::SetCurrentAim(CAMERA_AIM* pAim)
{
	GameStoreRemotePlayerAim(m_bytePlayerNumber, pAim);
}
//0.3.7
uint8_t CPlayerPed::GetCameraMode()
{
	if (m_bytePlayerNumber == 0) {
		return GameGetLocalPlayerCameraMode();
	}
	else {
		return GameGetPlayerCameraMode(m_bytePlayerNumber);
	}
}
// 0.3.7
float CPlayerPed::GetAimZ()
{
    if (!m_pPed)
    {
        return 0.0f;
    }
    return m_pPed->m_pPlayerData->m_fLookPitch;
}
// 0.3.7
void CPlayerPed::SetAimZ(float fAimZ)
{
    if (!m_pPed)
    {
        return;
    }
    m_pPed->m_pPlayerData->m_fLookPitch = fAimZ;
}
// 0.3.7
CWeapon* CPlayerPed::GetCurrentWeaponSlot()
{
    if (m_pPed)
    {
        return &m_pPed->m_aWeapons[m_pPed->m_nActiveWeaponSlot];
    }
    return NULL;
}
// 0.3.7
void CPlayerPed::SetCameraMode(uint8_t byteCameraMode)
{
	GameSetPlayerCameraMode(byteCameraMode, m_bytePlayerNumber);
}
// 0.3.7
void CPlayerPed::SetCameraZoomAndAspect(float fExtZoom, float fAspectRatio)
{
	GameSetPlayerCameraExtZoomAndAspect(m_bytePlayerNumber, fExtZoom, fAspectRatio);
}
// 0.3.7
void CPlayerPed::CheckVehicleParachute()
{
	if (m_dwParachuteObject)
	{
		ScriptCommand(&disassociate_object, m_dwParachuteObject, 0.0, 0.0, 0.0, 0);
		ScriptCommand(&destroy_object, m_dwParachuteObject);
		m_dwParachuteObject = 0;
	}
}

void CPlayerPed::ProcessVehicleHorn()
{

}
// 0.3.7
void CPlayerPed::PutDirectlyInVehicle(uint32_t dwVehicleGTAId, uint8_t byteSeatID)
{
	if (!m_pPed) return;
	if (!GamePool_Vehicle_GetAt(dwVehicleGTAId)) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(GetCurrentWeapon() == WEAPON_PARACHUTE)
	{
		CWorld::PlayerInFocus = m_bytePlayerNumber;
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(m_bytePlayerNumber);

		SetArmedWeapon(0, 0);

		GameSetLocalPlayerSkills();
		CWorld::PlayerInFocus = 0;
	}

    CVehicleGTA* pGtaVehicle = GamePool_Vehicle_GetAt(dwVehicleGTAId);

	if (pGtaVehicle->fHealth != 0.0f )
	{
		if (GetVehicleSubtype(pGtaVehicle) == VEHICLE_SUBTYPE_CAR ||
			GetVehicleSubtype(pGtaVehicle) == VEHICLE_SUBTYPE_BIKE)
		{
			if (byteSeatID > pGtaVehicle->m_nMaxPassengers)
				return;
		}

		if (byteSeatID == 0)
		{
			if (pGtaVehicle->pDriver && pGtaVehicle->pDriver->IsInVehicle()) {
				return;
			}
			ScriptCommand(&put_actor_in_car, m_dwGTAId, dwVehicleGTAId);
		}
		else
		{
			ScriptCommand(&put_actor_in_car2, m_dwGTAId, dwVehicleGTAId, byteSeatID - 1);
		}

		if (m_pPed == GamePool_FindPlayerPed())
		{
			if (m_pPed->IsInVehicle()) {
				CCamera::SetBehindPlayer();
			}
		}

		if (pNetGame)
		{
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
			if (VehicleID != INVALID_VEHICLE_ID && VehicleID <= MAX_VEHICLES) {
				CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
				if (pVehicle)
				{
					if (pVehicle->IsATrainPart())
					{
						if (m_pPed == GamePool_FindPlayerPed()) {
							ScriptCommand(&camera_on_vehicle, pVehicle->m_dwGTAId, 3, 2);
						}
					}
				}
			}
		}
	}
}
// 0.3.7
void CPlayerPed::EnterVehicle(uint32_t dwVehicleGTAId, bool bPassenger)
{
	if (!m_pPed) return;

    CVehicleGTA* pGtaVehicle = GamePool_Vehicle_GetAt(dwVehicleGTAId);
	if (!pGtaVehicle) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	bIgnoreNextEntry = true;
	if(GetCurrentWeapon() == WEAPON_PARACHUTE)
	{
		CWorld::PlayerInFocus = m_bytePlayerNumber;
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(m_bytePlayerNumber);

		SetArmedWeapon(0,0);

		GameSetLocalPlayerSkills();
		CWorld::PlayerInFocus = 0;
	}

	if (bPassenger)
	{
		if (pGtaVehicle->m_nModelIndex != TRAIN_PASSENGER || m_pPed != GamePool_FindPlayerPed()) {
			ScriptCommand(&send_actor_to_car_passenger, m_dwGTAId, dwVehicleGTAId, 3000, -1);
		}
		else {
			ScriptCommand(&put_actor_in_car2, m_dwGTAId, dwVehicleGTAId, -1);
		}
	}
	else
	{
		ScriptCommand(&send_actor_to_car_driverseat, m_dwGTAId, dwVehicleGTAId, 3000);
	}
}
// 0.3.7
const SCRIPT_COMMAND TASK_LEAVE_ANY_CAR = { 0x0633, "i" };
void CPlayerPed::ExitCurrentVehicle()
{
    FLog("ExitCurrentVehicle");

    if (!m_pPed) return;
    if(!m_dwGTAId)return;
    if (!IsValidGamePed(m_pPed) || !GamePool_Ped_GetAt(m_dwGTAId)) {
        return;
    }

    //CVehicleGta* ThisVehicleType = 0;

    if(m_pPed->bInVehicle)
    {
        ScriptCommand(&TASK_LEAVE_ANY_CAR, m_dwGTAId);

    }
}
// 0.3.7
int CPlayerPed::GetCurrentVehicleID()
{
	if(!m_pPed) {
		return 0;
	}

	return GamePool_Vehicle_GetIndex(reinterpret_cast<CVehicleGTA *>(m_pPed->pVehicle));
}

void CPlayerPed::SetSkillLevel(int iSkillID, int iLevel)
{

}
// 0.3.7
void CPlayerPed::SetAmmo(uint8_t byteWeapon, uint16_t wAmmo)
{
	if (m_pPed)
	{
        CWeapon* WeaponSlot = FindWeaponSlot(byteWeapon);
		if (WeaponSlot) {
			WeaponSlot->dwAmmo = (uint32_t)wAmmo;
		}
	}
}
// 0.3.7
CWeapon* CPlayerPed::FindWeaponSlot(uint8_t byteWeapon)
{
	if (!m_pPed) return nullptr;

	for (int i = 0; i < 13; i++)
	{
		if (m_pPed->m_aWeapons[i].dwType == byteWeapon) {
			return &m_pPed->m_aWeapons[i];
		}
	}

	return nullptr;
}
// 0.3.7
int CPlayerPed::GetVehicleSeatID()
{
    if(!m_pPed->pVehicle)
        return -1;

    if( m_pPed->pVehicle->pDriver == m_pPed) return 0;

    if(m_pPed->pVehicle->m_apPassengers[0] == m_pPed) return 1;
    if(m_pPed->pVehicle->m_apPassengers[1] == m_pPed) return 2;
    if(m_pPed->pVehicle->m_apPassengers[2] == m_pPed) return 3;
    if(m_pPed->pVehicle->m_apPassengers[3] == m_pPed) return 4;
    if(m_pPed->pVehicle->m_apPassengers[4] == m_pPed) return 5;
    if(m_pPed->pVehicle->m_apPassengers[5] == m_pPed) return 6;
    if(m_pPed->pVehicle->m_apPassengers[6] == m_pPed) return 7;

    return (-1);
}
// 0.3.7 (comment matcolor)
void CPlayerPed::SetAttachedObject(int index, NEW_ATTACHED_OBJECT* pNewAttachedObject)
{
	FLog("CPlayerPed::SetAttachedObject BoneID: %d", pNewAttachedObject->iBoneID);
	if (m_pPed && IsValidGamePed(m_pPed))
	{
		if (m_pPed->m_pRwObject)
		{
			if (index >= 0 && index < 10)
			{
				int iBoneID = pNewAttachedObject->iBoneID;
				if (iBoneID > 0 && iBoneID <= 18)
				{
					if (GetObjectSlotState(index))
						RemoveAttachedObject(index);

					RwMatrix mat;
					mat = m_pPed->GetMatrix().ToRwMatrix();
					memcpy(&m_attachedObjectInfo[index], pNewAttachedObject, sizeof(NEW_ATTACHED_OBJECT));

					CObject* pNewObject = new CObject(pNewAttachedObject->iModel, mat.pos, pNewAttachedObject->vecRot, 200.0f, true);

					m_pAttachedObjects[index] = pNewObject;

					//if (pNewAttachedObject->dwMaterialColor1)
					//	pNewObject->SetMaterial(-1, 0, 0, 0, pNewAttachedObject->dwMaterialColor1);
					//if (pNewAttachedObject->dwMaterialColor2)
					//	pNewObject->SetMaterial(-1, 1, 0, 0, pNewAttachedObject->dwMaterialColor2);

					pNewObject->m_pEntity->SetCollisionChecking(false);
					m_bObjectSlotUsed[index] = true;
				}
			}
		}
	}
}
// 0.3.7
void CPlayerPed::RemoveAttachedObject(int index)
{
	if (GetObjectSlotState(index))
	{
		if (m_pAttachedObjects[index])
		{
			delete m_pAttachedObjects[index];
			m_pAttachedObjects[index] = nullptr;
		}

		memset(&m_attachedObjectInfo[index], 0, sizeof(NEW_ATTACHED_OBJECT));
		m_bObjectSlotUsed[index] = false;
	}
}
// 0.3.7
bool CPlayerPed::GetObjectSlotState(int index)
{
	if (index < 0 || index >= 10) return false;

	return m_bObjectSlotUsed[index];
}
// 0.3.7
bool CPlayerPed::IsHaveAttachedObject()
{
	for (int i = 0; i < 10; i++)
	{
		if (m_bObjectSlotUsed[i]) return true;
	}

	return false;
}
// 0.3.7
void CPlayerPed::RemoveAllAttachedObjects()
{
	for (int i = 0; i < 10; i++)
	{
		if (m_bObjectSlotUsed[i])
		{
			if (m_pAttachedObjects[i])
			{
				delete m_pAttachedObjects[i];
				m_pAttachedObjects[i] = nullptr;
			}

			memset(&m_attachedObjectInfo[i], 0, sizeof(NEW_ATTACHED_OBJECT));
			m_bObjectSlotUsed[i] = false;
		}
	}
}

// 0.3.7
void CPlayerPed::ProcessAttachedObjects()
{
	bool bAnimUpdated = false;

	for (int i = 0; i < 10; i++)
	{
		if (m_bObjectSlotUsed[i])
		{
			if(m_pPed->IsAdded()) {
                if (m_pAttachedObjects[i]) {
                    if (!bAnimUpdated) {
                        if (m_pPed) {
                            // CEntity::UpdateRpHAnim
                            //((void (*)(CPedGTA*))(g_libGTASA + 0x3EBFF6 + 1))(m_pPed);
                            m_pPed->UpdateRpHAnim();
                            bAnimUpdated = true;
                        }
                    }

                    int iBoneID = m_attachedObjectInfo[i].iBoneID;

                    int iBoneIndex = 0;
                    if (m_pPed->m_apBones[iBoneID] == nullptr) return;
                    iBoneIndex = m_pPed->m_apBones[iBoneID]->m_nNodeId;
                    // CPhysical::Remove
                    auto hierarchy = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);

                    int iID;
                    if (hierarchy)
                    {

                        iID = RpHAnimIDGetIndex(hierarchy, m_attachedObjectInfo[i].iBoneID);
                    }
                    else
                    {
                        continue;
                    }
                    if (iID == -1)
                    {
                        continue;
                    }
                    ((void (*)(CEntityGTA*))(*(void**)(*(uintptr*)m_pAttachedObjects[i]->m_pEntity + (VER_x32 ? 16:16*2))))(m_pAttachedObjects[i]->m_pEntity); // CPhysical::Remove

                    RwMatrix boneMatrix;
                    memcpy(&boneMatrix, &hierarchy->pMatrixArray[iID], sizeof(RwMatrix));

                    CVector vecOut;
                    ProjectMatrix(&vecOut, reinterpret_cast<CMatrix *>(&boneMatrix),
                                  &m_attachedObjectInfo[i].vecOffset);

                    boneMatrix.pos.x = vecOut.x;
                    boneMatrix.pos.y = vecOut.y;
                    boneMatrix.pos.z = vecOut.z;

                    if (m_attachedObjectInfo[i].vecRot.x != 0.0f)
                        RwMatrixRotate(&boneMatrix, 0, m_attachedObjectInfo[i].vecRot.x);
                    if (m_attachedObjectInfo[i].vecRot.y != 0.0f)
                        RwMatrixRotate(&boneMatrix, 1, m_attachedObjectInfo[i].vecRot.y);
                    if (m_attachedObjectInfo[i].vecRot.z != 0.0f)
                        RwMatrixRotate(&boneMatrix, 2, m_attachedObjectInfo[i].vecRot.z);

                    RwMatrixScale(&boneMatrix, &m_attachedObjectInfo[i].vecScale);

                    m_pAttachedObjects[i]->m_pEntity->SetMatrix((CMatrix&)boneMatrix);

                    m_pAttachedObjects[i]->m_pEntity->UpdateRW();
                    m_pAttachedObjects[i]->m_pEntity->UpdateRwFrame();

                    m_pAttachedObjects[i]->m_pEntity->Add();
                }
            }
		}
	}
}

// 0.3.7
void CPlayerPed::GetBoneMatrix(RwMatrix* matOut, int iBoneID)
{
	if (m_pPed && IsValidGamePed(m_pPed))
	{
		if (m_pPed->m_pRwObject)
		{
			// GetAnimHierarchyFromSkinClump
			uintptr_t pAnimHierarchy = ((uintptr_t(*)(RwObject*))(g_libGTASA + 0x5D1020 + 1))(m_pPed->m_pRwObject);

			// RpHAnimIDGetIndex
			int index = (( int (*)(uintptr_t, int))(g_libGTASA + 0x1C2C90 + 1))(pAnimHierarchy, iBoneID) << 6;
			RwMatrix* mat = (RwMatrix*)(index + *(uintptr_t*)(pAnimHierarchy + 8));

			memcpy(matOut, mat, sizeof(RwMatrix));
		}
	}
}
// 0.3.7
void CPlayerPed::ClumpUpdateAnimations(float step, int flag)
{
	uintptr_t pRwObj;

	if (m_pPed)
	{
		pRwObj = reinterpret_cast<uintptr_t>(m_pPed->m_pRwObject);
		if (pRwObj) {
			// RpAnimBlendClumpUpdateAnimations
			((void (*)(uintptr_t, float, int))(g_libGTASA + 0x38BF00 + 1))(pRwObj, step, flag);
		}
	}
}
bool g_customFire = false;
extern uint32_t (*CWeapon__FireInstantHit)(CWeapon* thiz, CPedGTA* pFiringEntity, CVector* vecOrigin, CVector* muzzlePosn, CEntityGTA* targetEntity, CVector *target, CVector* originForDriveBy, int arg6, int muzzle);
extern uint32_t (*CWeapon__FireSniper)(CWeapon *pWeaponSlot, CPedGTA *pFiringEntity, CEntityGTA *a3, CVector *vecOrigin);

CPlayerPed* g_pCurrentFiredPed = nullptr;
BULLET_DATA* g_pCurrentBulletData = nullptr;
void CPlayerPed::FireInstant()
{
	LOGI("CPlayerPed::FireInstant");

	uint8_t byteSavedCameraMode = 0;
	uint16_t wSavedCameraMode2 = 0;

	if (m_bytePlayerNumber != 0)
	{
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(m_bytePlayerNumber);
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(m_bytePlayerNumber);
		if (*wCameraMode2 == 4) {
			*wCameraMode2 = 0;
		}

		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(m_bytePlayerNumber);
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(m_bytePlayerNumber);
		//GameStoreLocalPlayerSkills();
		//GameSetRemotePlayerSkills(m_bytePlayerNumber);
	}

	g_pCurrentFiredPed = this;

	if (m_bHaveBulletData)
		g_pCurrentBulletData = &m_bulletData;
	else
		g_pCurrentBulletData = nullptr;

	CVector vecBonePos, vecOut;

	if (GetCurrentWeapon() == WEAPON_SNIPERRIFLE)
	{
        if(m_pPed)
            CWeapon__FireSniper(GetCurrentWeaponSlot(), m_pPed, nullptr, nullptr);
        else
            CWeapon__FireSniper(nullptr, m_pPed, nullptr, nullptr);
	}
	else
	{
		GetWeaponInfoForFire(0, &vecBonePos, &vecOut);

        CWeapon* pSlot = GetCurrentWeaponSlot();

        if(m_pPed)
            CWeapon__FireInstantHit(pSlot, m_pPed, &vecBonePos, &vecOut, nullptr, nullptr, nullptr, 0, 1);
        else
            CWeapon__FireInstantHit(nullptr, m_pPed, &vecBonePos, &vecOut, nullptr, nullptr, nullptr, 0, 1);
	}

	g_pCurrentFiredPed = nullptr;
	g_pCurrentBulletData = nullptr;

	if (m_bytePlayerNumber != 0)
	{
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;
		GameSetLocalPlayerCameraExtZoomAndAspect();
		GameSetLocalPlayerAim();
		//GameSetLocalPlayerSkills();
	}
}
// 0.3.7
void CPlayerPed::GetWeaponInfoForFire(bool bLeftWrist, CVector* vecBonePos, CVector* vecOut)
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId))
		return;

	CVector *pFireOffset = GetCurrentWeaponFireOffset();
	if(pFireOffset && vecBonePos && vecOut)
	{
		vecOut->x = pFireOffset->x;
		vecOut->y = pFireOffset->y;
		vecOut->z = pFireOffset->z;

		int iBoneId = 24;
		if(bLeftWrist) iBoneId = 34;

		GetBonePosition(iBoneId, vecBonePos);

		vecBonePos->z += pFireOffset->z + 0.15000001f;

		GetTransformedBonePosition(iBoneId, vecOut);
	}
}

uintptr_t GetWeaponInfo(int iWeapon, int iSkill)
{
    // CWeaponInfo::GetWeaponInfo
    return ((uintptr_t(*)(int, int))(g_libGTASA + (VER_x32 ? 0x005E42E8 + 1 : 0x709BA8)))(iWeapon, iSkill);
}
// 0.3.7
CVector* CPlayerPed::GetCurrentWeaponFireOffset()
{
    if (!IsValidGamePed(m_pPed) || !GamePool_Ped_GetAt(m_dwGTAId)) {
        return nullptr;
    }

    CWeapon* pSlot = GetCurrentWeaponSlot();
    if (pSlot) {
        return (CVector*)(GetWeaponInfo(pSlot->dwType, 1) + 0x24);
    }
    return nullptr;
}
// 0.3.7
void CPlayerPed::ProcessBulletData(BULLET_DATA *btData)
{
	if (btData == nullptr)
	{
		m_bHaveBulletData = false;
		memset(&m_bulletData, 0, sizeof(BULLET_DATA));
		return;
	}

	m_bHaveBulletData = true;
	m_bulletData.pEntity = btData->pEntity;

	m_bulletData.vecOrigin.x = btData->vecOrigin.x;
	m_bulletData.vecOrigin.y = btData->vecOrigin.y;
	m_bulletData.vecOrigin.z = btData->vecOrigin.z;

	m_bulletData.vecPos.x = btData->vecPos.x;
	m_bulletData.vecPos.y = btData->vecPos.y;
	m_bulletData.vecPos.z = btData->vecPos.z;

	m_bulletData.vecOffset.x = btData->vecOffset.x;
	m_bulletData.vecOffset.y = btData->vecOffset.y;
	m_bulletData.vecOffset.z = btData->vecOffset.z;

	if (m_bytePlayerNumber == 0)
	{
		if (pNetGame)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				CPlayerPed* pLocalPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
				if (pLocalPlayerPed)
				{
					BULLET_SYNC_DATA btSync;
					memset(&btSync, 0, sizeof(BULLET_SYNC_DATA));

					unsigned short InstanceID = 0xFFFF;
					uint8_t byteHitType = BULLET_HIT_TYPE_NONE;

					if (pLocalPlayerPed->GetCurrentWeapon() != WEAPON_SNIPERRIFLE || btData->pEntity != nullptr)
					{
						if (btData->pEntity)
						{
							CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
							CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
							CObjectPool* pObjectPool = pNetGame->GetObjectPool();
							if (pPlayerPool && pVehiclePool && pObjectPool)
							{
								PLAYERID PlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGTA*)btData->pEntity);
								if (PlayerID != INVALID_PLAYER_ID)
								{
									byteHitType = BULLET_HIT_TYPE_PLAYER;
									InstanceID = PlayerID;
								}
								else
								{
									VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((CVehicleGTA*)btData->pEntity);
									if (VehicleID != INVALID_VEHICLE_ID)
									{
										byteHitType = BULLET_HIT_TYPE_VEHICLE;
										InstanceID = VehicleID;
									}
									else
									{
										OBJECTID ObjectID = pObjectPool->FindIDFromGtaPtr(
                                                dynamic_cast<CPhysical *>(btData->pEntity));
										if (ObjectID != INVALID_OBJECT_ID)
										{
											byteHitType = BULLET_HIT_TYPE_OBJECT;
											InstanceID = ObjectID;
										}
										else
										{
											CVector vecOut = { 0.0f, 0.0f, 0.0f };
											if (btData->pEntity->m_matrix)
											{
												ProjectMatrix(&vecOut, &btData->pEntity->GetMatrix(), &btData->vecOffset);
												btData->vecOffset.x = vecOut.x;
												btData->vecOffset.y = vecOut.y;
												btData->vecOffset.z = vecOut.z;
											}
											else
											{
												btData->vecOffset.x += btData->pEntity->GetPosition().x;
												btData->vecOffset.y += btData->pEntity->GetPosition().y;
												btData->vecOffset.z += btData->pEntity->GetPosition().z;
											}
										}
									}
								}
							}
						}

						btSync.vecOrigin.x = btData->vecOrigin.x;
						btSync.vecOrigin.y = btData->vecOrigin.y;
						btSync.vecOrigin.z = btData->vecOrigin.z;

						btSync.vecPos.x = btData->vecPos.x;
						btSync.vecPos.y = btData->vecPos.y;
						btSync.vecPos.z = btData->vecPos.z;

						btSync.vecOffset.x = btData->vecOffset.x;
						btSync.vecOffset.y = btData->vecOffset.y;
						btSync.vecOffset.z = btData->vecOffset.z;

						btSync.byteHitType = byteHitType;
						btSync.PlayerID = InstanceID;
						btSync.byteWeaponID = pLocalPlayerPed->GetCurrentWeapon();

						RakNet::BitStream bsBullet;
						bsBullet.Write((char)ID_BULLET_SYNC);
						bsBullet.Write((char*)&btSync, sizeof(BULLET_SYNC_DATA));
						pNetGame->GetRakClient()->Send(&bsBullet, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0);
					}
				}
			}
		}
	}
}

// 0.3.7
uint8_t CPlayerPed::FindDeathReasonAndResponsiblePlayer(uint16_t *nPlayer)
{
	if(m_pPed)
	{
		if(pNetGame)
		{
			PLAYERID PlayerIDWhoKilled;

			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

			if(!pVehiclePool || !pPlayerPool)
			{
				*nPlayer = INVALID_PLAYER_ID;
				return 255;
			}

			uint8_t byteDeathReason = (uint8_t)m_pPed->m_nLastDamagedWeaponType;

			if(byteDeathReason == WEAPON_DROWNING)
			{
				// lol poor kid.
				*nPlayer = INVALID_PLAYER_ID;
				return WEAPON_DROWNING;
			}

			// check for a player pointer.
			if(m_pPed->m_pLastEntityDamage)
			{
				// It's a weapon of some sort.
				if(byteDeathReason < WEAPON_CAMERA || byteDeathReason == WEAPON_RUNOVERBYCAR || byteDeathReason == WEAPON_EXPLOSION)
				{
					PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGTA *)m_pPed->m_pLastEntityDamage);
					if(PlayerIDWhoKilled != INVALID_PLAYER_ID)
					{
						// killed by another player with a weapon, this is all easy.
						*nPlayer = PlayerIDWhoKilled;
						return byteDeathReason;
					}
					else
					{
						if(pVehiclePool->FindIDFromGtaPtr((CVehicleGTA *)m_pPed->m_pLastEntityDamage) != INVALID_VEHICLE_ID)
						{
                            CVehicleGTA *pGtaVehicle = (CVehicleGTA *)m_pPed->m_pLastEntityDamage;
							PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGTA *)pGtaVehicle->pDriver);
							if(PlayerIDWhoKilled != INVALID_PLAYER_ID)
							{
								// killed by another player in car with a weapon, this is all easy.
								*nPlayer = PlayerIDWhoKilled;
								return byteDeathReason;
							}
						}
					}
				}
				else if(byteDeathReason == WEAPON_RAMMEDBYCAR)
				{
					// now, if we can find the vehicle
					// we can probably derive the responsible player.
					// Look in the vehicle pool for this vehicle.
					if(pVehiclePool->FindIDFromGtaPtr((CVehicleGTA *)m_pPed->m_pLastEntityDamage) != INVALID_VEHICLE_ID)
					{
                        CVehicleGTA *pGtaVehicle = (CVehicleGTA *)m_pPed->m_pLastEntityDamage;
						PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGTA *)pGtaVehicle->pDriver);
						if(PlayerIDWhoKilled != INVALID_PLAYER_ID)
						{
							*nPlayer = PlayerIDWhoKilled;
							return WEAPON_RAMMEDBYCAR;
						}
					}
				}
				else if(byteDeathReason == WEAPON_FALL)
				{
					if(pVehiclePool->FindIDFromGtaPtr((CVehicleGTA *)m_pPed->m_pLastEntityDamage) != INVALID_VEHICLE_ID)
					{
                        CVehicleGTA *pGtaVehicle = (CVehicleGTA *)m_pPed->m_pLastEntityDamage;
						PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGTA *)pGtaVehicle->pDriver);
						if(PlayerIDWhoKilled != INVALID_PLAYER_ID)
						{
							*nPlayer = PlayerIDWhoKilled;
							return WEAPON_FALL;
						}
					}

					*nPlayer = INVALID_PLAYER_ID;
					return WEAPON_FALL;
				}
			}
		}
	}

	// Unhandled death type.
	*nPlayer = INVALID_PLAYER_ID;
	return 255;
}

// 0.3.7
void CPlayerPed::SetStateFlags(uint32_t dwState)
{
	if (!m_pPed) return;
	//m_pPed->dwStateFlags = dwState;
}
// 0.3.7
uint32_t CPlayerPed::GetStateFlags()
{
	if (!m_pPed) return 0;
	return 0;
}
// 0.3.7
bool CPlayerPed::IsOnGround()
{
	if (m_pPed) {
		if (!m_pPed->bIsInTheAir) {
			return true;
		}
	}

	return false;
}

extern uint32_t(*CWorld_ProcessLineOfSight)(CVector*, CVector*, CVector*, CEntityGTA**, bool, bool, bool, bool, bool, bool, bool, bool);
CEntityGTA* CPlayerPed::GetEntityUnderPlayer()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId))
		return nullptr;

    CEntityGTA* entity;
	CVector vecStart;
	CVector vecEnd;
	CVector vecPos;
	char buf[100];

	vecStart.x = m_pPed->m_matrix->m_pos.x;
	vecStart.y = m_pPed->m_matrix->m_pos.y;
	vecStart.z = m_pPed->m_matrix->m_pos.z - 0.25f;

	vecEnd.x = m_pPed->m_matrix->m_pos.x;
	vecEnd.y = m_pPed->m_matrix->m_pos.y;
	vecEnd.z = vecStart.z - 1.75f;

	LineOfSight(&vecStart, &vecEnd, (void*)buf, (uintptr_t)&entity, 0, 1, 0, 1, 0, 0, 0, 0);
	return (CEntityGTA*)entity;
}

bool CPlayerPed::IsCrouching()
{
    if (!m_pPed || !m_dwGTAId)
    {
        return false;
    }
    if (!GamePool_Ped_GetAt(m_dwGTAId))
    {
        return false;
    }
    return m_pPed->bIsDucking;
}

void CPlayerPed::ApplyCrouch()
{
    if (!m_pPed || !m_dwGTAId)
    {
        return;
    }
    if (!GamePool_Ped_GetAt(m_dwGTAId))
    {
        return;
    }

    if (!(m_pPed->bIsDucking))
    {
        if (!IsCrouching())
        {
            if (m_pPed->m_pIntelligence)
            {
                ((int (*)(CPedIntelligence*, uint16_t))(g_libGTASA + (VER_x32 ? 0x004C07B0 + 1 : 0x5BCE70)))(m_pPed->m_pIntelligence, 0);
            }
        }
    }
}

void CPlayerPed::ResetCrouch()
{
    if (!m_pPed || !m_dwGTAId)
    {
        return;
    }
    if (!GamePool_Ped_GetAt(m_dwGTAId))
    {
        return;
    }
    m_pPed->bIsDucking = false;
    if (m_pPed->m_pIntelligence)
    {
        ((int (*)(CPedIntelligence*))(g_libGTASA + (VER_x32 ? 0x004C08A8 + 1 : 0x5BCFF8)))(m_pPed->m_pIntelligence);
    }
}

bool CPlayerPed::IsInJetpackMode()
{
	/*if(m_pPed && IsAdded() && !IsInVehicle() && m_pPed->Tasks &&
	   m_pPed->Tasks->pdwJumpJetPack)
	{
		return GetTaskTypeFromTask(m_pPed->Tasks->pdwJumpJetPack) == 1303;
	}*/

	return false;
}

void CPlayerPed::StartJetpack()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || IsInVehicle() || !m_pPed->IsAdded())
		return;

	CWorld::PlayerInFocus = m_bytePlayerNumber;

	// reset CTasks so the CJetPack task priority can be enforced
	m_pPed->SetPosn(m_pPed->m_matrix->m_pos.x, m_pPed->m_matrix->m_pos.y, m_pPed->m_matrix->m_pos.z);

	// CCheat::JetpackCheat
	(( void (*)())(g_libGTASA+0x2FE1E8+1))();

	CWorld::PlayerInFocus = 0;
}

void CPlayerPed::StopJetpack()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || !m_pPed->IsAdded())
		return;

	if(IsInJetpackMode())
	{
		/*uintptr_t dwJetPackTask = reinterpret_cast<uintptr_t>(m_pPed->Tasks->pdwJumpJetPack);

		// CTaskSimpleJetPack::~CTaskSimpleJetPack
		(( void (*)(uintptr_t))(g_libGTASA+0x530C8C+1))(dwJetPackTask); // CTaskSimpleJetPack::~CTaskSimpleJetPack

		m_pPed->Tasks->pdwJumpJetPack = 0;*/
	}
}

int CPlayerPed::HasHandsUp()
{
	/*if(!m_pPed || !IsAdded())
		return false;

	// HandsUP not have function GetTaskType
	if(m_pPed->Tasks->pdwJumpJetPack == NULL) return false;
	uint32_t dwJmpVtbl = m_pPed->Tasks->pdwJumpJetPack[0];
	if(dwJmpVtbl == g_libGTASA+0x665800) return true;*/
	return false;
}

void CPlayerPed::HandsUp()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || IsInVehicle() || !m_pPed->IsAdded())
		return;

	ScriptCommand(&task_hands_up, m_dwGTAId, -1);
}

char DanceStyleLibs[4][16] = {"WOP","GFUNK","RUNNINGMAN","STRIP"};
char DanceIdleLoops[4][16] = {"DANCE_LOOP","DANCE_LOOP","DANCE_LOOP","STR_Loop_B"};

char szDanceAnimNamesFemale[16][16] = {
		"DANCE_G1","DANCE_G2","DANCE_G3","DANCE_G4",
		"DANCE_G5","DANCE_G6","DANCE_G7","DANCE_G8",
		"DANCE_G9","DANCE_G10","DANCE_G11","DANCE_G12",
		"DANCE_G13","DANCE_G14","DANCE_G15","DANCE_G16"
};

char szDanceAnimNamesMale[16][16] = {
		"DANCE_B1","DANCE_B2","DANCE_B3","DANCE_B4",
		"DANCE_B5","DANCE_B6","DANCE_B7","DANCE_B8",
		"DANCE_B9","DANCE_B10","DANCE_B11","DANCE_B12",
		"DANCE_B13","DANCE_B14","DANCE_B15","DANCE_B16"
};

char szStripAnims[16][16] = {
		"strip_A","strip_B","strip_C","strip_D",
		"strip_E","strip_F","strip_G","STR_A2B",
		"STR_B2A","STR_B2C","STR_C1","STR_C2",
		"STR_C2B","STR_A2B","STR_B2C","STR_C2"
};

void CPlayerPed::StartDancing(int danceId)
{
	if(!m_pPed || IsInVehicle() || !m_pPed->IsAdded())
		return;

	if(danceId < 0 || danceId > 3)
		return;

	if(danceId == 3 && GetPedStat() != 5 && GetPedStat() != 22) // male stripping looks disgusting
		return;

	ApplyAnimation(DanceIdleLoops[danceId], DanceStyleLibs[danceId], 16.0, 1, 0, 0, 0, -1);
	m_iDanceStyle = danceId;
}

void CPlayerPed::StopDancing()
{
	if(!m_pPed || !m_pPed->IsAdded())
		return;

	m_iDanceStyle = -1;

	RwMatrix mat = m_pPed->GetMatrix().ToRwMatrix();
	m_pPed->SetPosn(mat.pos.x, mat.pos.y, mat.pos.z);
}

void CPlayerPed::ProcessDancing()
{
	if(!m_pPed || !m_pPed->IsAdded() || IsInVehicle())
		return;

	if(m_iDanceStyle == -1) return;

	uint16_t wKeysUpDown, wKeysLeftRight, iExtra;
	if(!m_bytePlayerNumber)
	{
		wKeysUpDown = LocalPlayerKeys.wKeyUD;
		wKeysLeftRight = LocalPlayerKeys.wKeyLR;
		iExtra = LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		wKeysUpDown = RemotePlayerKeys[m_bytePlayerNumber].wKeyUD;
		wKeysLeftRight = RemotePlayerKeys[m_bytePlayerNumber].wKeyLR;
		iExtra = RemotePlayerKeys[m_bytePlayerNumber].bKeys[ePadKeys::KEY_SPRINT];
	}

	if(!IsPerformingCustomAnim())
		ApplyAnimation(DanceIdleLoops[m_iDanceStyle], DanceStyleLibs[m_iDanceStyle], 4.0, 1, 0, 0, 0, -1);

	int iNewMove = 0;
	if(wKeysUpDown > 128 && !wKeysLeftRight && !iExtra) iNewMove = 0; // UP
	else if(wKeysUpDown < 128 && !wKeysLeftRight && !iExtra) iNewMove = 1; // DOWN
	else if(!wKeysUpDown && wKeysLeftRight < 128 && !iExtra) iNewMove = 2; // LEFT
	else if(!wKeysUpDown && wKeysLeftRight > 128 && !iExtra) iNewMove = 3; // RIGHT
	else if(wKeysUpDown > 128 && wKeysLeftRight < 128 && !iExtra) iNewMove = 4; // UP-LEFT
	else if(wKeysUpDown > 128 && wKeysLeftRight > 128 && !iExtra) iNewMove = 5; // UP-RIGHT
	else if(wKeysUpDown < 128 && wKeysLeftRight < 128 && !iExtra) iNewMove = 6; // DOWN-LEFT
	else if(wKeysUpDown < 128 && wKeysLeftRight > 128 && !iExtra) iNewMove = 7; // DOWN-RIGHT
	else if(wKeysUpDown > 128 && !wKeysLeftRight && iExtra) iNewMove = 8; // UP/EX
	else if(wKeysUpDown < 128 && !wKeysLeftRight && iExtra) iNewMove = 9; // DOWN/EX
	else if(!wKeysUpDown && wKeysLeftRight < 128 && iExtra) iNewMove = 10; // LEFT/EX
	else if(!wKeysUpDown && wKeysLeftRight > 128 && iExtra) iNewMove = 11; // RIGHT/EX
	else if(wKeysUpDown > 128 && wKeysLeftRight < 128 && iExtra) iNewMove = 12; // UP-LEFT/EX
	else if(wKeysUpDown > 128 && wKeysLeftRight > 128 && iExtra) iNewMove = 13; // UP-RIGHT/EX
	else if(wKeysUpDown < 128 && wKeysLeftRight < 128 && iExtra) iNewMove = 14; // DOWN-LEFT/EX
	else if(wKeysUpDown < 128 && wKeysLeftRight > 128 && iExtra) iNewMove = 15; // DOWN-RIGHT/EX
	else return;

	if(iNewMove == m_iLastDanceMove) return; // don't allow the same move twice

	m_iLastDanceMove = iNewMove;

	char *szAnimName = GetDanceAnimForMove(iNewMove);
	if(!szAnimName && !strlen(szAnimName)) return;

	ApplyAnimation(szAnimName, DanceStyleLibs[m_iDanceStyle], 4.0, 0, 0, 0, 0, -1);
}

char *CPlayerPed::GetDanceAnimForMove(int iMove)
{
	if(!m_iDanceStyle == -1) return ""; // shouldn't ever happen but..

	// style 0-2 have the same anim names, but M/F versions
	if(m_iDanceStyle >= 0 && m_iDanceStyle <= 2)
	{
		if(GetPedStat() == 5 || GetPedStat() == 22) // female or pro (only female types)
			return szDanceAnimNamesFemale[iMove];

		return szDanceAnimNamesMale[iMove];
	}
	else if(m_iDanceStyle == 3)
		return szStripAnims[iMove];

	return "";
}

void CPlayerPed::StartPissing()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || IsInVehicle() || !m_pPed->IsAdded())
		return;

	if(!m_bPissingState)
	{
		ApplyAnimation("PISS_LOOP", "PAULNMAC", 4.0, 1, 0, 0, 0, -1);

		char *ahaha = "PETROLCAN";
		ScriptCommand(&attach_particle_to_actor2, ahaha, m_dwGTAId, 0.0f, 0.58f, -0.08f, 0.0f, 0.01f, 0.0f, 1, &m_dwPissParticlesHandle);
		ScriptCommand(&make_particle_visible, m_dwPissParticlesHandle);

		m_bPissingState = true;
	}
}

void CPlayerPed::StopPissing()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || !m_pPed->IsAdded())
		return;

	if(m_bPissingState)
	{
		if(m_dwPissParticlesHandle)
		{
			ScriptCommand(&destroy_particle, m_dwPissParticlesHandle);
			m_dwPissParticlesHandle = 0;
		}

		RwMatrix mat = m_pPed->GetMatrix().ToRwMatrix();
		m_pPed->SetPosn(mat.pos.x, mat.pos.y, mat.pos.z);

		m_bPissingState = false;
	}
}

bool CPlayerPed::IsPerformingCustomAnim()
{
	if(!m_pPed || !m_pPed->IsAdded())
		return false;

	//if(m_pPed->Tasks->pdwJumpJetPack) return true;
	return false;
}

bool CPlayerPed::IsAnimationPlaying(char* szAnimName)
{
	if (!m_pPed) return false;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return false;
	if (!szAnimName || !strlen(szAnimName)) return false;

	if (ScriptCommand(&is_char_playing_anim, m_dwGTAId, szAnimName)) {
		return true;
	}

	return false;
}

int CPlayerPed::GetPedStat()
{
	if(!m_pPed) return -1;

	return Game_PedStatPrim(m_pPed->m_nModelIndex);
}

eStuffType CPlayerPed::GetStuff()
{
	if(!m_pPed) return eStuffType::STUFF_TYPE_NONE;

	return m_stuffData.type;
}

bool CPlayerPed::ApplyStuff()
{
	if(m_pPed && m_pPed->IsAdded() && !IsPerformingCustomAnim() || IsInVehicle())
	{
		SetArmedWeapon(0, 0);

		eStuffType stuffType = GetStuff();
		switch(stuffType)
		{
			case eStuffType::STUFF_TYPE_BEER:
			case eStuffType::STUFF_TYPE_DYN_BEER:
			case eStuffType::STUFF_TYPE_PINT_GLASS:
				if(GetPedStat() == 5 || GetPedStat() == 22)
					ApplyAnimation("DNK_STNDF_LOOP", "BAR", 4.0, 0, 0, 0, 0, -1);
				else ApplyAnimation("DNK_STNDM_LOOP", "BAR", 4.0, 0, 0, 0, 0, -1);
				break;

			case eStuffType::STUFF_TYPE_CIGGI:
				ApplyAnimation("smkcig_prtl", "GANGS", 700.0, 0, 0, 0, 0, 2750);
				break;
		}

		if(stuffType == eStuffType::STUFF_TYPE_BEER ||
		   stuffType == eStuffType::STUFF_TYPE_DYN_BEER)
		{
			SetDrunkLevel(GetDrunkLevel() + 1250);
		}

		return true;
	}

	return false;
}

void CPlayerPed::GiveStuff(eStuffType type)
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || IsInVehicle())
		return;


	if(m_stuffData.dwObject != NULL)
		DropStuff();

	SetArmedWeapon(0, 0);

	RwMatrix matPlayer = m_pPed->GetMatrix().ToRwMatrix();
	switch(type)
	{
		case eStuffType::STUFF_TYPE_BEER:
			ScriptCommand(&create_object, OBJECT_CJ_BEER_B_2, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z, &m_stuffData.dwObject);
			if(GamePool_Object_GetAt(m_stuffData.dwObject))
				ScriptCommand(&task_pick_up_object, m_dwGTAId, m_stuffData.dwObject, 0.05000000074505806, 0.02999999932944775, -0.300000011920929, 6, 16, "NULL", "NULL", -1);
			break;

		case eStuffType::STUFF_TYPE_DYN_BEER:
			ScriptCommand(&create_object, OBJECT_DYN_BEER_1, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z, &m_stuffData.dwObject);
			if(GamePool_Object_GetAt(m_stuffData.dwObject))
				ScriptCommand(&task_pick_up_object, m_dwGTAId, m_stuffData.dwObject, 0.05000000074505806, 0.02999999932944775, -0.05000000074505806, 6, 16, "NULL", "NULL", -1);
			break;

		case eStuffType::STUFF_TYPE_PINT_GLASS:
			ScriptCommand(&create_object, OBJECT_CJ_PINT_GLASS, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z, &m_stuffData.dwObject);
			if(GamePool_Object_GetAt(m_stuffData.dwObject))
				ScriptCommand(&task_pick_up_object, m_dwGTAId, m_stuffData.dwObject, 0.03999999910593033, 0.1000000014901161, -0.01999999955296516, 6, 16, "NULL", "NULL", -1);
			break;

		case eStuffType::STUFF_TYPE_CIGGI:
			ScriptCommand(&create_object, OBJECT_CJ_CIGGY, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z, &m_stuffData.dwObject);
			if(GamePool_Object_GetAt(m_stuffData.dwObject))
				ScriptCommand(&task_pick_up_object, m_dwGTAId, m_stuffData.dwObject, 0.0, 0.0, 0.0, 6, 16, "NULL", "NULL", -1);
			break;
	}

	m_stuffData.type = type;
}

void CPlayerPed::DropStuff()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId))
		return;

	if(GamePool_Object_GetAt(m_stuffData.dwObject))
	{
		ScriptCommand(&task_pick_up_object, m_dwGTAId, m_stuffData.dwObject, 0.0, 0.0, 0.0, 6, 16, "NULL", "NULL", 0);
		m_stuffData.dwObject = 0;
	}

	RwMatrix matPlayer = m_pPed->GetMatrix().ToRwMatrix();
	m_pPed->SetPosn(matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);

	m_stuffData.type = eStuffType::STUFF_TYPE_NONE;
}

void CPlayerPed::SetDrunkLevel(uint32_t dwLevel)
{
	if(dwLevel > 50000) dwLevel = 50000;
	if(dwLevel < 0) dwLevel = 0;

	m_stuffData.dwDrunkLevel = dwLevel;
};

void CPlayerPed::ProcessDrunk()
{
	if(!m_pPed || GetDrunkLevel() == 0)
		return;

	int iDrunkLevel = GetDrunkLevel();
	if(!m_bytePlayerNumber)
	{
		if(iDrunkLevel > 0 && iDrunkLevel <= 2000)
		{
			SetDrunkLevel(iDrunkLevel - 1);
			ScriptCommand(&set_player_drunk_visuals, m_bytePlayerNumber, 0);
		}
		else if(iDrunkLevel > 2000 && iDrunkLevel <= 50000)
		{
			int iDrunkVisual = iDrunkLevel * 0.02;
			if(iDrunkVisual <= 250)
			{
				if(iDrunkVisual < 5)
					iDrunkVisual = 0;
			}
			else
			{
				iDrunkVisual = 250;
			}

			SetDrunkLevel(iDrunkLevel - 1);
			ScriptCommand(&set_player_drunk_visuals, m_bytePlayerNumber, iDrunkVisual);

			if(IsInVehicle() && !IsAPassenger())
			{
                CVehicleGTA *_pVehicle = GetGtaVehicle();
				if(_pVehicle)
				{
					if(!m_stuffData.dwLastUpdateTick || (GetTickCount() - m_stuffData.dwLastUpdateTick) > 200)
					{
						int iRandNumber = rand() % 40;
						float fRotation = 0.0;
						if(iRandNumber >= 20)
						{
							fRotation = 0.012;
							if(iDrunkLevel >= 5000) fRotation = 0.015;

							if(iRandNumber <= 30)
							{
								fRotation = -0.012;
								if(iDrunkLevel >= 5000) fRotation = -0.015;
							}
						}

						if(FloatOffset(_pVehicle->GetMoveSpeed().x, 0.0) > 0.050000001f ||
						   FloatOffset(_pVehicle->GetMoveSpeed().y, 0.0) > 0.050000001f)
						{
							_pVehicle->GetTurnSpeed().z = fRotation + _pVehicle->GetTurnSpeed().z;
						}

						m_stuffData.dwLastUpdateTick = GetTickCount();
					}
				}
			}
		}
	}
}

void CPlayerPed::ToggleCellphone(int iOn)
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || !m_pPed->IsAdded())
		return;

	ScriptCommand(&toggle_actor_cellphone, m_dwGTAId, iOn);
	m_iCellPhoneEnabled = iOn;
}

bool CPlayerPed::IsJumpTask()
{
	//if(m_pPed && !IsInVehicle() && m_pPed->Tasks && m_pPed->Tasks->pdwJumpJetPack)
		//return GetTaskTypeFromTask(m_pPed->Tasks->pdwJumpJetPack) == 211;

	return false;
}

CVehicleGTA* CPlayerPed::GetGtaContactVehicle()
{
	return (CVehicleGTA*)(m_pPed + 1388);
}

CEntityGTA* CPlayerPed::GetGtaContactEntity()
{
	return (CEntityGTA*)(m_pPed + 1416);
}

bool CPlayerPed::IsTakeDamageFallTask()
{
	//if(m_pPed && !IsInVehicle() && m_pPed->Tasks && m_pPed->Tasks->pdwDamage)
		//return GetTaskTypeFromTask(m_pPed->Tasks->pdwDamage) == 208;

	return false;
}

uint8_t CPlayerPed::IsEnteringVehicle()
{
	/*if(m_pPed && m_pPed->Tasks && m_pPed->Tasks->pdwJumpJetPack)
	{
		int iType = GetTaskTypeFromTask(m_pPed->Tasks->pdwJumpJetPack);
		if(iType == 700 || iType == 712)
			return 2;
		if(iType == 701 || iType == 713)
			return 1;
	}*/
	return m_pPed->IsEnteringCar();
}

bool CPlayerPed::IsExitingVehicle()
{
	//if(m_pPed && m_pPed->Tasks && m_pPed->Tasks->pdwJumpJetPack)
		//return GetTaskTypeFromTask(m_pPed->Tasks->pdwJumpJetPack) == 704;

	return m_pPed->IsExitingVehicle();
}

bool CPlayerPed::IsSitTask()
{
	/*if(m_pPed && m_pPed->Tasks && m_pPed->Tasks->pdwJumpJetPack)
	{
		return (GetTaskTypeFromTask(m_pPed->Tasks->pdwJumpJetPack) == 221 ||
				GetTaskTypeFromTask(m_pPed->Tasks->pdwJumpJetPack) == 220);
	}*/

	return false;
}

void CPlayerPed::ClearAllTasks()
{
	if (!GamePool_Ped_GetAt(m_dwGTAId) || !m_pPed) {
		return;
	}

	ScriptCommand(&clear_char_tasks, m_dwGTAId);
}

void CPlayerPed::ProcessSpecialAction(int iAction)
{
	if (iAction == SPECIAL_ACTION_CARRY && !IsAnimationPlaying("CRRY_PRTIAL"))
	{
		ApplyAnimation("CRRY_PRTIAL", "CARRY", 4.1, 0, 0, 0, 1, 1);

		m_iCarryState = 1;

		return;
	}

	if (iAction != SPECIAL_ACTION_CARRY && IsCarry())
	{
		ApplyAnimation("crry_prtial", "CARRY", 4.00, false, false, false, false, 1);

		m_iCarryState = 0;

		return;
	}

	if (iAction == SPECIAL_ACTION_CUFFED)
	{
		if(m_iCuffedState)
			ProcessCuffAndCarry();
		else
			m_iCuffedState = 1;

		return;
	}

	if (iAction != SPECIAL_ACTION_CUFFED && IsCuffed())
	{
		m_iCuffedState = 0;

		return;
	}
}

#include "RW/RenderWare.h"
void CPlayerPed::ProcessCuffAndCarry()
{

	LOGI("ProcessCuffAndCarry 1");

	//RpHAnimHierarchy* hierarchy = GetAnimHierarchyFromSkinClump();
	//if(!hierarchy) return;

	LOGI("ProcessCuffAndCarry 2");

	/*// left  full hand
	RtQuat* quat1 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*9)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 9 %f %f %f", quat1->imag.x, quat1->imag.y, quat1->imag.z);
	quat1->imag.x = -0.66828901;
	quat1->imag.z = -0.72157103;

	RtQuat* quat2 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*10)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 10 %f %f %f", quat2->imag.x, quat2->imag.y, quat2->imag.z);
	quat2->imag.x = 0.1;
	quat2->imag.z = 0.1;

	RtQuat* quat3 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*11)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 11 %f %f %f", quat3->imag.x, quat3->imag.y, quat3->imag.z);
	quat3->imag.x = -0.099771999;
	quat3->imag.z = 0.29214600;

	RtQuat* quat4 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*12)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 12 %f %f %f", quat4->imag.x, quat4->imag.y, quat4->imag.z);
	quat4->imag.x = -0.52213401;
	quat4->imag.z = -0.44046500;

	RtQuat* quat5 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*13)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 13 %f %f %f", quat5->imag.x, quat5->imag.y, quat5->imag.z);
	quat5->imag.x = -0.00024400000;
	quat5->imag.z = 0.30278999;

	RtQuat* quat11 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*15)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 15 %f %f %f", quat11->imag.x, quat11->imag.y, quat11->imag.z);
	quat11->imag.x = 0.64019400;
	quat11->imag.z = -0.75476903;

	RtQuat* quat22 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*16)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 16 %f %f %f", quat22->imag.x, quat22->imag.y, quat22->imag.z);
	quat22->imag.x = 0.014282000;
	quat22->imag.z = 0.11346700;

	RtQuat* quat33 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*17)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 17 %f %f %f", quat33->imag.x, quat33->imag.y, quat33->imag.z);
	quat33->imag.x = 0.12742200;
	quat33->imag.z = 0.33031401;

	// right wrist
	RtQuat* quat44 = (RtQuat*) ((uintptr_t)hierarchy->currentAnim+(hierarchy->currentAnim->currentInterpKeyFrameSize*18)+sizeof(RtAnimInterpolator));
	LOGI("ProcessCuffAndCarry 18 %f %f %f", quat44->imag.x, quat44->imag.y, quat44->imag.z);
	quat44->imag.x = 0.579847;
	quat44->imag.z = -0.24247;*/

	LOGI("ProcessCuffAndCarry 5");
	SetArmedWeapon(0,0);
	m_pPed->UpdateRpHAnim();
}

void CPlayerPed::SetCurrentWeapon(uint8_t weaponType)
{
	((int(*)(uintptr_t, uint8_t))(g_libGTASA + 0x4A51AC + 1))((uintptr_t)m_pPed, weaponType);
}

float CPlayerPed::GetDistanceFromVehicle(CVehicle *pVehicle)
{
	RwMatrix matFromPlayer, matThis;
	CVector vecDistance;

    matThis = m_pPed->GetMatrix().ToRwMatrix();
    matFromPlayer = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();

	vecDistance.x = matThis.pos.x - matFromPlayer.pos.x;
	vecDistance.y = matThis.pos.y - matFromPlayer.pos.y;
	vecDistance.z = matThis.pos.z - matFromPlayer.pos.z;

	return (float)sqrt(vecDistance.x * vecDistance.x + vecDistance.y * vecDistance.y + vecDistance.z * vecDistance.z);
}

CVehicle* CPlayerPed::GetCurrentVehicle()
{
	if(!m_pPed) return nullptr;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	for (size_t i = 0; i < MAX_VEHICLES; i++) {
		if (pVehiclePool->GetSlotState(i)) {
			CVehicle *pVehicle = pVehiclePool->GetAt(i);
			if (pVehicle && pVehicle->m_pVehicle->IsAdded()) {
				if (pVehicle->m_pVehicle == reinterpret_cast<CVehicleGTA*>(m_pPed->pVehicle)) {
					return pVehicle;
				}
			}
		}
	}
	return nullptr;
}

bool CPlayerPed::IsInPassengerDriveByMode()
{
	/*if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || !IsInVehicle() ||
	   !m_pPed->Tasks || !m_pPed->Tasks->pdwJumpJetPack)
	{
		return false;
	}*/

	return 0;
}

bool CPlayerPed::StartPassengerDriveByMode()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId))
		return false;

	int iWeapon = GetCurrentWeapon();
	if(iWeapon == WEAPON_PARACHUTE)
	{
		SetArmedWeapon(0, 0);
		return false;
	}

	if((iWeapon != WEAPON_MICRO_UZI) && (iWeapon != WEAPON_MP5) && (iWeapon != WEAPON_TEC9)) {
		return false;
	}

	SetArmedWeapon(iWeapon, 0);
	ScriptCommand(&enter_passenger_driveby, m_dwGTAId, -1, -1, 0.0f, 0.0f, 0.0f, 300.0f, 8, 1, 100);
	return true;
}

void CPlayerPed::StopPassengerDriveByMode()
{
	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || !IsInVehicle())
		return;

	if(IsInPassengerDriveByMode())
	{
        /*uintptr_t dwJetPackTask = (uintptr_t)m_pPed->Tasks->pdwJumpJetPack;

		// CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy
		(( void (*)(uintptr_t))(g_libGTASA+0x4E4458+1))(dwJetPackTask);

		m_pPed->Tasks->pdwJumpJetPack = 0;*/
	}
}

void CPlayerPed::SetWeaponSkill(uint32_t iWeaponType, uint16_t byteSkill)
{
	if(!m_pPed) return;

	LOGI("SetWeaponSkill: %d %d", iWeaponType, byteSkill);

	if(m_bytePlayerNumber == 0)
		GameUpdateLocalPlayerSkill(iWeaponType, byteSkill);
	else
		GameStoreRemotePlayerSkills(m_bytePlayerNumber, iWeaponType, byteSkill);
}