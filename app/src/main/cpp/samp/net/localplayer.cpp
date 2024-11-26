#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "localplayer.h"
#include "../gui/gui.h"

// voice
#include "../voice_new/MicroIcon.h"
#include "../voice_new/SpeakerList.h"

extern UI* pUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
//extern CVoice* pVoice;

extern int iNetModeNormalOnFootSendRate;
extern int iNetModeNormalInCarSendRate;
extern int iNetModeFiringSendRate;
extern int iNetModeSendMultiplier;

extern bool bUsedPlayerSlots[];

uint32_t dwEnterVehTimeElasped = -1;
bool bFirstSpawn = true;
bool g_bLockEnterVehicleWidget = false;

CLocalPlayer::CLocalPlayer()
{
    FLog("CLocalPlayer::CLocalPlayer()");
	ResetAllSyncAttributes();

	m_bInRCMode = false;

	m_surfData.bIsActive = false;
	m_surfData.pSurfInst = 0;
	m_surfData.bIsVehicle = false;
	m_surfData.vecOffsetPos = CVector {0.0f, 0.0f, 0.0f};

	m_statsData.dwLastMoney = 0;
	m_statsData.dwLastDrunkLevel = 0;

	m_pPlayerPed = pGame->FindPlayerPed();
	m_bIsActive = false;
	m_bIsWasted = false;
	
	m_iDisplayZoneTick = 0;
	m_dwLastSendTick = GetTickCount();
	m_dwLastSendSpecTick = GetTickCount();
	m_dwLastSendSyncTick = GetTickCount();
	m_dwLastUpdateInCarData = GetTickCount();
	m_dwLastSendAimSyncTick = GetTickCount();
	m_dwLastStatsUpdateTick = GetTickCount();
	m_dwLastPerformStuffAnimTick = GetTickCount();
	m_dwLastUpdateOnFootData = GetTickCount();
	m_bIsSpectating = false;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;
	m_bSpawnDialogShowed = false;

	for (int i = 0; i < 13; i++)
	{
		m_byteLastWeapon[i] = 0;
		m_dwLastAmmo[i] = 0;
	}

	m_byteTeam = NO_TEAM;
}

CLocalPlayer::~CLocalPlayer()
{

}
extern int g_iLagCompensationMode;
extern bool DriveBy;

bool CLocalPlayer::Process()
{
	//UpdateVoice();

	if (m_bIsActive && m_pPlayerPed != nullptr)
	{
		// local player is dead
		if (!m_bIsWasted && m_pPlayerPed->GetActionTrigger() == ACTION_DEATH || m_pPlayerPed->IsDead())
		{
			ToggleSpectating(false);

			if(m_pPlayerPed->GetDanceStyle() != -1) m_pPlayerPed->StopDancing();
			if(m_pPlayerPed->IsCellphoneEnabled()) m_pPlayerPed->ToggleCellphone(0);
			if(m_pPlayerPed->IsPissing()) m_pPlayerPed->StopPissing();
			if(m_pPlayerPed->GetStuff() != eStuffType::STUFF_TYPE_NONE) m_pPlayerPed->DropStuff();

			m_pPlayerPed->TogglePlayerControllable(true);

			if (m_bInRCMode)
			{
				m_bInRCMode = false;
				m_pPlayerPed->m_pPed->Add();
			}

			if (m_pPlayerPed->IsInVehicle() && !m_pPlayerPed->IsAPassenger())
			{
				SendInCarFullSyncData(); // for explosion
				m_LastVehicle = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
			}

			m_pPlayerPed->ExtinguishFire();
			SendWastedNotification();
			m_bIsActive = false;
			m_bIsWasted = true;
			pGame->EnableZoneNames(false);
			return true;
		}

		uint16_t wKeys, lrAnalog, udAnalog;
		wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog, false);

		uint32_t dwThisTick = GetTickCount();

		if(m_pPlayerPed->GetDanceStyle() != -1) 
		{
			m_pPlayerPed->ProcessDancing();
			if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()
				|| m_pPlayerPed->IsInJetpackMode())
			{ 
				m_pPlayerPed->StopDancing();
			}
		}

		// HANDLE I GOT MY HANDS UP BUT DON'T WANT TO ANYMORE
		if(m_pPlayerPed->HasHandsUp())
		{ 
			if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()
				|| m_pPlayerPed->IsInJetpackMode())
			{ 
				m_pPlayerPed->TogglePlayerControllable(true);
			}
		}

		// HANDLE PHONE LOCAL PED
		if(m_pPlayerPed->IsCellphoneEnabled()) 
		{
			if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()
				|| m_pPlayerPed->IsInJetpackMode())
			{ 
				m_pPlayerPed->ToggleCellphone(0);
			}
		}

		// HANDLE URINATING LOCAL PED
		if(m_pPlayerPed->IsPissing())
		{ 
			if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()
				|| m_pPlayerPed->IsInJetpackMode())
			{ 
				m_pPlayerPed->StopPissing();
			} 
		}

		// HANDLE STUFF LOCAL PED
		if(m_pPlayerPed->GetStuff() != eStuffType::STUFF_TYPE_NONE)
		{
			if((dwThisTick - m_dwLastPerformStuffAnimTick) > 3500)
				m_bPerformingStuffAnim = false;

			if(!m_bPerformingStuffAnim && LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]) 
			{
				if(m_pPlayerPed->ApplyStuff())
				{
					m_dwLastPerformStuffAnimTick = dwThisTick;
					m_bPerformingStuffAnim = true;
				}
			}

			if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()
				|| m_pPlayerPed->IsInJetpackMode())
			{
				m_pPlayerPed->DropStuff();
			}
		}

		// HANDLE DRUNK
		m_pPlayerPed->ProcessDrunk();

		if (dwEnterVehTimeElasped != -1 &&
			(dwThisTick - dwEnterVehTimeElasped) > 5000 &&
			!m_pPlayerPed->IsInVehicle())
		{
			CCamera::SetBehindPlayer();
			dwEnterVehTimeElasped = -1;
		}
		if (dwThisTick >= m_iDisplayZoneTick) {
			pGame->EnableZoneNames(pNetGame->m_pNetSet->bZoneNames);
		}
		pGame->UpdateCheckpoints();
		if ((dwThisTick - m_dwLastStatsUpdateTick) > 1000) {
			SendStatsUpdate();
			m_dwLastStatsUpdateTick = dwThisTick;
		}
		UpdateSurfing();
		CheckWeapons();
		uint8_t byteInterior = pGame->GetActiveInterior();
		if (byteInterior != m_byteCurInterior) {
			UpdateRemoteInterior(byteInterior);
		}
		UpdateCameraTarget();
		// PLAYER DATA UPDATES
		if (m_bIsSpectating) {
			ProcessSpectating();
			m_bPassengerDriveByMode = false;
		}
		// DRIVER CONDITIONS
		else if (m_pPlayerPed->IsInVehicle() && !m_pPlayerPed->IsAPassenger())
		{
			g_bLockEnterVehicleWidget = false;

            CVehicleGTA* pGtaVehicle = m_pPlayerPed->GetGtaVehicle();
			m_nLastVehicle = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pGtaVehicle);

			//m_pPlayerPed->RemoveWeaponWhenEnteringVehicle();
			MoveHeadWithCamera();
			ProcessInCarWorldBounds();

			if ((dwThisTick - m_dwLastSendAimSyncTick) > 500)
			{
				m_dwLastSendAimSyncTick = dwThisTick;
				SendAimSyncData();
			}

			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			CVehicle *pVehicle = nullptr;

			if (pVehiclePool) {
				m_CurrentVehicle = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
			}

			pVehicle = pVehiclePool->GetAt(m_CurrentVehicle);
			if (pVehicle && !m_bInRCMode && pVehicle->IsRCVehicle())
			{
				m_pPlayerPed->m_pPed->Remove();
				m_bInRCMode = true;
			}

			if (m_bInRCMode && !pVehicle)
			{
				m_pPlayerPed->SetHealth(0.0f);
				m_pPlayerPed->SetDead();
			}
			
			if (m_bInRCMode && pVehicle && pVehicle->GetHealth() == 0.0f)
			{
				m_pPlayerPed->SetHealth(0.0f);
				m_pPlayerPed->SetDead();
			}

			if ((dwThisTick - m_dwLastSendTick) > GetOptimumInCarSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendInCarFullSyncData();
				UpdateVehicleDamage(m_CurrentVehicle);
			}

			m_bPassengerDriveByMode = false;
		}
		// ONFOOT CONDITIONS
		else if (m_pPlayerPed->GetActionTrigger() == ACTION_NORMAL || m_pPlayerPed->GetActionTrigger() == ACTION_SCOPE)
		{
			g_bLockEnterVehicleWidget = true;
			if(m_bWasInCar)
			{
				m_bWasInCar = false;
			}

			//ProcessSurfing();
			MoveHeadWithCamera();

			if (m_bInRCMode)
			{
				m_bInRCMode = false;
				m_pPlayerPed->m_pPed->Add();
			}

			//HandlePassengerEntry();
			ProcessOnFootWorldBounds();

			if (m_CurrentVehicle != 0xFFFF)
			{
				m_LastVehicle = m_CurrentVehicle;
				m_CurrentVehicle = 0xFFFF;
			}

			if (CompareOnFootSyncKeys(wKeys, udAnalog, lrAnalog) 
				|| (dwThisTick - m_dwLastSendTick) > GetOptimumOnFootSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendOnFootFullSyncData();
			}

            if((dwThisTick - m_dwLastSendTick) < 1000)
            {
                if(IS_TARGETING(m_pPlayerPed->m_pPed) && IS_FIRING(m_pPlayerPed->m_pPed))
                {
                    if(g_iLagCompensationMode == 2)
                    {
                        if((dwThisTick - m_dwLastSendAimSyncTick) > iNetModeFiringSendRate)
                        {
                            SendAimSyncData();
                            m_dwLastSendAimSyncTick = dwThisTick;
                        }
                    }
                    else if((dwThisTick - m_dwLastSendAimSyncTick) > 500)
                    {
                        SendAimSyncData();
                        m_dwLastSendAimSyncTick = dwThisTick;
                    }
                }
                else
                {
                    if((dwThisTick - m_dwLastSendAimSyncTick) > 1000)
                    {
                        SendAimSyncData();
                        m_dwLastSendAimSyncTick = dwThisTick;
                    }
                }
            }

			m_bPassengerDriveByMode = false;
		}
		// PASSENGER CONDITIONS
		else if (m_pPlayerPed->GetActionTrigger() == ACTION_INCAR && m_pPlayerPed->IsAPassenger())
		{
			g_bLockEnterVehicleWidget = false;

            CVehicleGTA* pGtaVehicle = m_pPlayerPed->GetGtaVehicle();
			m_nLastVehicle = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pGtaVehicle);

			MoveHeadWithCamera();

			if (m_bInRCMode)
			{
				m_bInRCMode = false;
				m_pPlayerPed->m_pPed->Add();
			}

			//GTA_CONTROLSET *controls = GameGetInternalKeys();

			if(!m_bPassengerDriveByMode && LocalPlayerKeys.bKeys[KEY_CTRL_BACK]) {
				// NOT IN DRIVEBY MODE AND HORN HELD
				if(m_pPlayerPed->GetCurrentWeapon() == WEAPON_MICRO_UZI || m_pPlayerPed->GetCurrentWeapon() == WEAPON_MP5 || m_pPlayerPed->GetCurrentWeapon() == WEAPON_TEC9) {
					if(m_pPlayerPed->StartPassengerDriveByMode()) {
						m_bPassengerDriveByMode = true;
					}
				}
			}

			if (dwThisTick - m_dwLastSendTick > GetOptimumInCarSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendPassengerFullSyncData();
			}
		}
	}

	// HANDLE !IsActive spectating
	if (m_bIsSpectating && !m_bIsActive)
	{
		if (m_bSpawnDialogShowed)
		{
			m_bSpawnDialogShowed = false;
			if(pUI) pUI->spawn()->setVisible(false);
		}

		ProcessSpectating();
		return true;
	}

	// HANDLE NEEDS TO RESPAWN AFTER DEATH
	if (m_bIsWasted
		&& m_pPlayerPed->GetActionTrigger() != ACTION_WASTED
		&& m_pPlayerPed->GetActionTrigger() != ACTION_DEATH)
	{
		if (m_pPlayerPed->IsHaveAttachedObject())
			m_pPlayerPed->RemoveAllAttachedObjects();

		//if (sub_100AC660(_this->m_pPlayerPed))
		//	sub_100AC670(_this->m_pPlayerPed, 0);

		if (IsClearedToSpawn() && pNetGame->GetGameState() == GAMESTATE_CONNECTED)
		{
			if (m_pPlayerPed->GetHealth() > 0.0f) {
				Spawn();
			}
		}
		else
		{
			m_bIsWasted = false;
			HandleClassSelection();
		}

		return true;
	}

	if (m_pPlayerPed->GetActionTrigger() != ACTION_WASTED &&
		m_pPlayerPed->GetActionTrigger() != ACTION_DEATH &&
		pNetGame->GetGameState() == GAMESTATE_CONNECTED &&
		!m_bIsActive &&
		!m_bIsSpectating)
	{
		ProcessClassSelection();
	}

	return true;
}

void CLocalPlayer::ProcessClassSelection()
{ 
	if (!m_bSpawnDialogShowed)
	{
		if (pUI) pUI->spawn()->setVisible(true);
		RequestClass(m_iSelectedClass);
		m_bSpawnDialogShowed = true;
	}
}

void CLocalPlayer::ResetAllSyncAttributes()
{
	m_bHasSpawnInfo = false;
	m_bWaitingForSpawnRequestReply = false;
	m_iSelectedClass = 0;
	m_byteCurInterior = 0;
	m_LastVehicle = 0xFFFF;
	m_bInRCMode = false;

	memset(&m_SpawnInfo, 0, sizeof(PLAYER_SPAWN_INFO));
	memset(&m_ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	memset(&m_icSync, 0, sizeof(INCAR_SYNC_DATA));
	memset(&m_TrailerData, 0, sizeof(TRAILER_SYNC_DATA));
	memset(&m_psSync, 0, sizeof(PASSENGER_SYNC_DATA));
	memset(&m_aimSync, 0, sizeof(AIM_SYNC_DATA));

	m_dwAnimation = 0;
	m_dwLastWeaponsUpdateTick = GetTickCount();
	m_byteCurrentWeapon = 0;

	/* voice */
	m_iVCState = VOICE_CHANNEL_STATE_CLOSED;
	m_dwVCOpenRequestTick = GetTickCount();

	m_CurrentVehicle = INVALID_VEHICLE_ID;
	m_LastVehicle = INVALID_VEHICLE_ID;
	m_nLastVehicle = INVALID_VEHICLE_ID;
	m_bWasInCar = false;
}
// 0.3.7 ( ศใํ๎๐ F4-class-reselect)
void CLocalPlayer::ToggleSpectating(bool bToggle)
{
	if (m_bIsSpectating && !bToggle) {
		Spawn();
	}

	m_bIsSpectating = bToggle;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_bSpectateProcessed = false;
	m_SpectateID = 0xFFFFFFFF;
}

void CLocalPlayer::ProcessSpectating()
{
	RakNet::BitStream bsSpectatorSync;
	SPECTATOR_SYNC_DATA spSync;
	RwMatrix matPos;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
    matPos = TheCamera.GetMatrix().ToRwMatrix();

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if (!pPlayerPool || !pVehiclePool) return;

	spSync.vecPos.x = matPos.pos.x;
	spSync.vecPos.y = matPos.pos.y;
	spSync.vecPos.z = matPos.pos.z;
	spSync.lrAnalog = lrAnalog;
	spSync.udAnalog = udAnalog;
	spSync.wKeys = wKeys;

	if ((GetTickCount() - m_dwLastSendSpecTick) > GetOptimumOnFootSendRate())
	{
		m_dwLastSendSpecTick = GetTickCount();
		bsSpectatorSync.Write((uint8_t)ID_SPECTATOR_SYNC);
		bsSpectatorSync.Write((char*)&spSync, sizeof(SPECTATOR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsSpectatorSync, HIGH_PRIORITY, UNRELIABLE, 0);

		if ((GetTickCount() - m_dwLastSendAimSyncTick) > (GetOptimumOnFootSendRate() * 2))
		{
			m_dwLastSendAimSyncTick = GetTickCount();
			SendAimSyncData();
		}
	}

	pGame->DisplayHUD(false);

	m_pPlayerPed->SetHealth(100.0f);
	GetPlayerPed()->m_pPed->SetPosn(spSync.vecPos.x, spSync.vecPos.y, spSync.vecPos.z + 20.0f);

	// handle spectate player left the server
	if (m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
		!pPlayerPool->GetSlotState(m_SpectateID))
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	// handle spectate player is no longer active (ie Died)
	if (m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
		pPlayerPool->GetSlotState(m_SpectateID) &&
		(!pPlayerPool->GetAt(m_SpectateID)->IsActive() ||
			pPlayerPool->GetAt(m_SpectateID)->GetState() == PLAYER_STATE_WASTED))
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	if (m_bSpectateProcessed) return;

	if (m_byteSpectateType == SPECTATE_TYPE_NONE)
	{
		GetPlayerPed()->RemoveFromVehicleAndPutAt(0.0f, 0.0f, 10.0f);
        CCamera::SetPosition(50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);
        CCamera::LookAtPoint(60.0f, 60.0f, 50.0f, 2);
		m_bSpectateProcessed = true;
	}
	else if (m_byteSpectateType == SPECTATE_TYPE_PLAYER)
	{
		uint32_t dwGTAId = 0;
		CPlayerPed* pPlayerPed = 0;

		if (pPlayerPool->GetSlotState(m_SpectateID))
		{
			pPlayerPed = pPlayerPool->GetAt(m_SpectateID)->GetPlayerPed();
			if (pPlayerPed)
			{
				dwGTAId = pPlayerPed->m_dwGTAId;
				ScriptCommand(&camera_on_actor, dwGTAId, m_byteSpectateMode, 2);
				//pGame->GetCamera()->AttachToEntity(m_pPlayerPed);
				m_bSpectateProcessed = true;
			}
		}
	}
	else if (m_byteSpectateType == SPECTATE_TYPE_VEHICLE)
	{
		CVehicle* pVehicle = nullptr;
		uint32_t dwGTAId = 0;
		
		pVehicle = pVehiclePool->GetAt((VEHICLEID)m_SpectateID);
		if (pVehicle)
		{
			dwGTAId = pVehicle->m_dwGTAId;
			ScriptCommand(&camera_on_vehicle, dwGTAId, m_byteSpectateMode, 2);
			//pGame->GetCamera()->AttachToEntity(pVehicle);
			m_bSpectateProcessed = true;
		}
	}
}

bool CLocalPlayer::Spawn()
{
	if (!m_bHasSpawnInfo) {
		return false;
	}

	// voice
	SpeakerList::Show();
	MicroIcon::Show();

	if (m_bSpawnDialogShowed == true)
	{
		m_bSpawnDialogShowed = false;
		if (pUI) pUI->spawn()->setVisible(false);
	}


    CCamera::SetBehindPlayer();
	pGame->DisplayHUD(true);
	m_pPlayerPed->TogglePlayerControllable(true);

	if (!bFirstSpawn) {
		m_pPlayerPed->SetInitialState();
	}
	else {
		bFirstSpawn = false;
	}

	pGame->RefreshStreamingAt(m_SpawnInfo.vecPos.x, m_SpawnInfo.vecPos.y);

	if (m_pPlayerPed->IsCuffed()) {
	//	m_pPlayerPed->ToggleCuffed(false);
	}

	m_pPlayerPed->RestartIfWastedAt(&m_SpawnInfo.vecPos, m_SpawnInfo.fRotation);
	m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	m_pPlayerPed->ClearWeapons();
	m_pPlayerPed->ResetDamageEntity();

	//ApplySpecialAction(0);

	//_this->field_2DE = 0;

	if (m_SpawnInfo.iSpawnWeapons[2] != -1) {
		m_pPlayerPed->GiveWeapon(m_SpawnInfo.iSpawnWeapons[2],
			m_SpawnInfo.iSpawnWeaponsAmmo[2]);
	}

	if (m_SpawnInfo.iSpawnWeapons[1] != -1) {
		m_pPlayerPed->GiveWeapon(m_SpawnInfo.iSpawnWeapons[1],
			m_SpawnInfo.iSpawnWeaponsAmmo[1]);
	}

	if (m_SpawnInfo.iSpawnWeapons[0] != -1) {
		m_pPlayerPed->GiveWeapon(m_SpawnInfo.iSpawnWeapons[0],
			m_SpawnInfo.iSpawnWeaponsAmmo[0]);
	}

	pGame->DisableTrainTraffic();

	m_pPlayerPed->m_pPed->SetPosn(m_SpawnInfo.vecPos.x,
		m_SpawnInfo.vecPos.y, m_SpawnInfo.vecPos.z + 0.5f);

	m_pPlayerPed->SetTargetRotation(m_SpawnInfo.fRotation);

	m_bIsWasted = false;
	m_bIsActive = true;
	m_bWaitingForSpawnRequestReply = false;
	m_surfData.bIsActive = false;

	RakNet::BitStream bsSendSpawn;
	pNetGame->GetRakClient()->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

	m_iDisplayZoneTick = GetTickCount() + 1000;

	return true;
}
// 0.3.7
void CLocalPlayer::HandleClassSelection()
{
	m_bClearedToSpawn = false;
	if (m_pPlayerPed) {
		m_pPlayerPed->SetInitialState();
		m_pPlayerPed->SetHealth(100.0f);
		m_pPlayerPed->TogglePlayerControllable(false);
	}
}
// 0.3.7
void CLocalPlayer::SendWastedNotification()
{
	uint8_t byteDeathReason;
	PLAYERID WhoWasResponsible = INVALID_PLAYER_ID;
	RakNet::BitStream bsPlayerDeath;
	byteDeathReason = m_pPlayerPed->FindDeathReasonAndResponsiblePlayer(&WhoWasResponsible);
	bsPlayerDeath.Write(byteDeathReason);
	bsPlayerDeath.Write(WhoWasResponsible);
	pNetGame->GetRakClient()->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7 (ํๅ๒ extKeys ่ this->field_104 = 1)
void CLocalPlayer::SendOnFootFullSyncData()
{
	RakNet::BitStream bsPlayerSync;
	RwMatrix matPlayer;
	CVector vecMoveSpeed;
	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys = m_pPlayerPed->GetAdditionalKeys();
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

	ONFOOT_SYNC_DATA ofSync;

    matPlayer = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
    vecMoveSpeed = m_pPlayerPed->m_pPed->GetMoveSpeed();
	
	ofSync.lrAnalog = lrAnalog;
	ofSync.udAnalog = udAnalog;
	ofSync.wKeys = wKeys;
	ofSync.vecPos.x = matPlayer.pos.x;
	ofSync.vecPos.y = matPlayer.pos.y;
	ofSync.vecPos.z = matPlayer.pos.z;

	ofSync.quat.SetFromMatrix(&matPlayer);
	ofSync.quat.Normalize();

	if (FloatOffset(ofSync.quat.w, m_ofSync.quat.w) < 0.00001 &&
		FloatOffset(ofSync.quat.x, m_ofSync.quat.x) < 0.00001 &&
		FloatOffset(ofSync.quat.y, m_ofSync.quat.y) < 0.00001 &&
		FloatOffset(ofSync.quat.z, m_ofSync.quat.z) < 0.00001)
	{
		ofSync.quat.Set(m_ofSync.quat);
	}

	ofSync.byteHealth = (uint8_t)m_pPlayerPed->GetHealth();
	ofSync.byteArmour = (uint8_t)m_pPlayerPed->GetArmour();

	ofSync.byteCurrentWeapon = (exKeys << 6) | ofSync.byteCurrentWeapon & 0x3F;
	ofSync.byteCurrentWeapon ^= (ofSync.byteCurrentWeapon ^ m_pPlayerPed->GetCurrentWeapon()) & 0x3F;
	ofSync.byteSpecialAction = GetSpecialAction();
	ofSync.vecMoveSpeed.x = vecMoveSpeed.x;
	ofSync.vecMoveSpeed.y = vecMoveSpeed.y;
	ofSync.vecMoveSpeed.z = vecMoveSpeed.z;

    ofSync.vecSurfOffsets.x = 0.0f;
    ofSync.vecSurfOffsets.y = 0.0f;
    ofSync.vecSurfOffsets.z = 0.0f;
    ofSync.wSurfID = 0;
	if(m_surfData.bIsActive){
		if(m_surfData.bIsVehicle && m_surfData.dwSurfVehID != INVALID_VEHICLE_ID){
			CVehicle* pVeh = (CVehicle*)m_surfData.pSurfInst;
			ofSync.vecSurfOffsets.x = m_surfData.vecOffsetPos.x;
			ofSync.vecSurfOffsets.y = m_surfData.vecOffsetPos.y;
			ofSync.vecSurfOffsets.z = m_surfData.vecOffsetPos.z;
			ofSync.wSurfID = m_surfData.dwSurfVehID;
		}
	}

	ofSync.dwAnimation = 0;
	//_this->field_104 = 1;

	if ((GetTickCount() - m_dwLastUpdateOnFootData) > 500 || memcmp(&m_ofSync, &ofSync, sizeof(ONFOOT_SYNC_DATA)))
	{
		m_dwLastUpdateOnFootData = GetTickCount();

		bsPlayerSync.Write((uint8_t)ID_PLAYER_SYNC);
		bsPlayerSync.Write((char*)&ofSync, sizeof(ONFOOT_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1);
		memcpy(&m_ofSync, &ofSync, sizeof(ONFOOT_SYNC_DATA));
	}
}

void CLocalPlayer::SendInCarFullSyncData()
{
	CPlayerPed* pPlayerPed = m_pPlayerPed;

	if (!pPlayerPed) return;

	INCAR_SYNC_DATA icSync;
	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) return;

	icSync.VehicleID = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
	if(icSync.VehicleID == INVALID_VEHICLE_ID) return;

	CVehicle *pVehicle = m_pPlayerPed->GetCurrentVehicle();
	if(!pVehicle) return;

	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys = m_pPlayerPed->GetAdditionalKeys();
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

	icSync.lrAnalog = lrAnalog;
	icSync.udAnalog = udAnalog;
	icSync.wKeys = wKeys;

	RwMatrix mat;
	CVector vecMoveSpeed;
    mat = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
    vecMoveSpeed = pVehicle->m_pVehicle->GetMoveSpeed();

	icSync.quat.SetFromMatrix(&mat);
	icSync.quat.Normalize();

	if (	FloatOffset(icSync.quat.w, m_icSync.quat.w) < 0.00001f
			&&	FloatOffset(icSync.quat.x, m_icSync.quat.x) < 0.00001f
			&&	FloatOffset(icSync.quat.y, m_icSync.quat.y) < 0.00001f
			&&	FloatOffset(icSync.quat.z, m_icSync.quat.z) < 0.00001f)
	{
		icSync.quat.Set(m_icSync.quat);
	}

	icSync.vecPos.x = mat.pos.x;
	icSync.vecPos.y = mat.pos.y;
	icSync.vecPos.z = mat.pos.z;

	icSync.vecMoveSpeed.x = vecMoveSpeed.x;
	icSync.vecMoveSpeed.y = vecMoveSpeed.y;
	icSync.vecMoveSpeed.z = vecMoveSpeed.z;

	icSync.fCarHealth = pVehicle->GetHealth();
	icSync.bytePlayerHealth = m_pPlayerPed->GetHealth();
	icSync.bytePlayerArmour = m_pPlayerPed->GetArmour();

	icSync.byteCurrentWeapon = (exKeys << 6) | icSync.byteCurrentWeapon & 0x3F;
	icSync.byteCurrentWeapon ^= (icSync.byteCurrentWeapon ^ m_pPlayerPed->GetCurrentWeapon()) & 0x3F;

	icSync.TrailerID = INVALID_VEHICLE_ID;

	CVehicle *pTrailer = pVehicle->GetTrailer();
	if(pTrailer && pTrailer->m_pVehicle && pTrailer->GetTractor() == pVehicle)
	{
		pVehicle->SetTrailer(pTrailer);
		icSync.TrailerID = pVehiclePool->FindIDFromGtaPtr(pTrailer->m_pVehicle);
	}
	else pVehicle->SetTrailer(NULL);

	if (pVehicle->m_pVehicle->GetModelId() == TRAIN_PASSENGER_LOCO ||
		pVehicle->m_pVehicle->GetModelId() == TRAIN_FREIGHT_LOCO ||
		pVehicle->m_pVehicle->GetModelId() == TRAIN_TRAM)
	{
		icSync.fTrainSpeed = pVehicle->GetTrainSpeed();
	}
	else if(pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE ||
		pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PUSHBIKE)
	{
		icSync.fTrainSpeed = pVehicle->GetBikeLean();
	}
	else if (pVehicle->m_pVehicle->GetModelId() == HYDRA)
	{
		icSync.fTrainSpeed = pVehicle->GetHydraThrusters();
	}
	else
	{
		icSync.fTrainSpeed = 0.0f;
	}

	icSync.byteSirenOn = 0;
	if(pVehicle->SirenEnabled()) icSync.byteSirenOn = 1;

	if(pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE)
	{
		if(pVehicle->IsLandingGearNotUp()) icSync.byteLandingGearState = 1;
		else icSync.byteLandingGearState = 0;
	}

    if(icSync.TrailerID != INVALID_VEHICLE_ID)
        SendTrailerData(icSync.TrailerID);

	if (IsNeedSyncDataSend(&m_icSync, &icSync, sizeof(INCAR_SYNC_DATA)))
	//if( (GetTickCount() - m_dwLastUpdateInCarData) > 500 || memcmp(&m_icSync, &icSync, sizeof(INCAR_SYNC_DATA))) {
	{
		RakNet::BitStream bsVehicleSync;
		bsVehicleSync.Write((uint8_t) ID_VEHICLE_SYNC);
		bsVehicleSync.Write((char *) &icSync, sizeof(INCAR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_icSync, &icSync, sizeof(INCAR_SYNC_DATA));
	}

	//if (pVehicle->HasTurret() || GetTickCount() - m_dwLastSendAimSyncTick > 1000)
	//{
	//	m_dwLastSendAimSyncTick = GetTickCount();
	//	SendAimSyncData();
	//}

}

void CLocalPlayer::SendTrailerData(VEHICLEID vehicleId)
{
	TRAILER_SYNC_DATA trSync;
	memset(&trSync, 0, sizeof(TRAILER_SYNC_DATA));

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) return;
	
	CVehicle* pTrailer = pVehiclePool->GetAt(vehicleId);
	if(pTrailer)
	{
		RwMatrix matTrailer = pTrailer->m_pVehicle->GetMatrix().ToRwMatrix();
        CQuaternion syncQuat;
        syncQuat.SetFromMatrix(&matTrailer);
		syncQuat.Normalize();
        trSync.quat = syncQuat;
		
		trSync.trailerId = vehicleId;

		trSync.vecPos.x = matTrailer.pos.x;
		trSync.vecPos.y = matTrailer.pos.y;
		trSync.vecPos.z = matTrailer.pos.z;

        trSync.vecMoveSpeed = pTrailer->m_pVehicle->GetMoveSpeed();
        trSync.vecTurnSpeed = pTrailer->m_pVehicle->GetTurnSpeed();

		if(IsNeedSyncDataSend(&m_TrailerData, &trSync, sizeof(TRAILER_SYNC_DATA)))
		{
			RakNet::BitStream bsTrailerSync;
			bsTrailerSync.Write((uint8_t)ID_TRAILER_SYNC);
			bsTrailerSync.Write((char*)&trSync, sizeof (TRAILER_SYNC_DATA));
			pNetGame->GetRakClient()->Send(&bsTrailerSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0);

			memcpy(&m_TrailerData, &trSync, sizeof(TRAILER_SYNC_DATA));
		}
	}
}

void CLocalPlayer::SendPassengerFullSyncData()
{
	RakNet::BitStream bsData;
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys = m_pPlayerPed->GetAdditionalKeys();
	uint16_t wkeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

	PASSENGER_SYNC_DATA psSync;
	memset(&psSync, 0, sizeof(PASSENGER_SYNC_DATA));

    CVehicleGTA* pGtaVehicle = m_pPlayerPed->GetGtaVehicle();
	psSync.VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
	if (psSync.VehicleID == INVALID_VEHICLE_ID) return;

	psSync.lrAnalog = lrAnalog;
	psSync.udAnalog = udAnalog;
	psSync.wKeys = wkeys;
	psSync.byteCurrentWeapon = (exKeys << 6) | psSync.byteCurrentWeapon & 0x3F;
	psSync.byteCurrentWeapon ^= (psSync.byteCurrentWeapon ^ m_pPlayerPed->GetCurrentWeapon()) & 0x3F;
	psSync.bytePlayerHealth = m_pPlayerPed->GetHealth();
	psSync.bytePlayerArmour = m_pPlayerPed->GetArmour();
	psSync.byteSeatFlags ^= (psSync.byteSeatFlags ^ m_pPlayerPed->GetVehicleSeatID()) & 0x3F;
	uint8_t byteUnk = psSync.byteSeatFlags & 0x7F;
	//if(m_pPlayerPed->IsCuffed()) byteUnk = psSync.byteSeatFlags | 0x80;
	psSync.byteSeatFlags = (byteUnk ^ (m_pPlayerPed->IsInPassengerDriveByMode() << 6)) & 0x40 ^ byteUnk;

	RwMatrix mat = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
	psSync.vecPos.x = mat.pos.x;
	psSync.vecPos.y = mat.pos.y;
	psSync.vecPos.z = mat.pos.z;

	if (IsNeedSyncDataSend(&m_psSync, &psSync, sizeof(PASSENGER_SYNC_DATA)))
	{
		bsData.Write((char)ID_PASSENGER_SYNC);
		bsData.Write((char*)& psSync, sizeof(PASSENGER_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1);
		memcpy(&m_psSync, &psSync, sizeof(PASSENGER_SYNC_DATA));
	}

	if(m_bPassengerDriveByMode)	SendAimSyncData();
}
// 0.3.7
void CLocalPlayer::SendAimSyncData()
{
	AIM_SYNC_DATA aimSync;

	CAMERA_AIM* caAim = m_pPlayerPed->GetCurrentAim();

	aimSync.byteCamMode = m_pPlayerPed->GetCameraMode();
	aimSync.vecAimf.x = caAim->f1x;
	aimSync.vecAimf.y = caAim->f1y;
	aimSync.vecAimf.z = caAim->f1z;
	aimSync.vecAimPos.x = caAim->pos1x;
	aimSync.vecAimPos.y = caAim->pos1y;
	aimSync.vecAimPos.z = caAim->pos1z;
	aimSync.fAimZ = m_pPlayerPed->GetAimZ();
	aimSync.aspect_ratio = GameGetAspectRatio() * 255.0;
	aimSync.byteCamExtZoom = static_cast<unsigned char>(m_pPlayerPed->GetCameraExtendedZoom() * 63.0f) & 63;

    CWeapon* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
	if (pwstWeapon->dwState == 2)
		aimSync.byteWeaponState = WEAPONSTATE_RELOADING;
	else
		aimSync.byteWeaponState = (pwstWeapon->dwAmmoInClip > 1) ? WEAPONSTATE_FIRING : pwstWeapon->dwAmmoInClip;

	if ((GetTickCount() - m_dwLastSendSyncTick) > 500 || memcmp(&m_aimSync, &aimSync, sizeof(AIM_SYNC_DATA)))
	{
		m_dwLastSendSyncTick = GetTickCount();
		RakNet::BitStream bsAimSync;
		bsAimSync.Write((char)ID_AIM_SYNC);
		bsAimSync.Write((char*)&aimSync, sizeof(AIM_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1);
		memcpy(&m_aimSync, &aimSync, sizeof(AIM_SYNC_DATA));
	}
}

void CLocalPlayer::SendStatsUpdate()
{
	if(m_statsData.dwLastMoney != pGame->GetLocalMoney() ||
	   m_statsData.dwLastDrunkLevel != m_pPlayerPed->GetDrunkLevel())
	{
		m_statsData.dwLastMoney = pGame->GetLocalMoney();
		m_statsData.dwLastDrunkLevel = m_pPlayerPed->GetDrunkLevel();

		RakNet::BitStream bsStats;
		bsStats.Write((uint8_t)ID_STATS_UPDATE);
		bsStats.Write(m_statsData.dwLastMoney);
		bsStats.Write(m_statsData.dwLastDrunkLevel);
		pNetGame->GetRakClient()->Send(&bsStats, HIGH_PRIORITY, UNRELIABLE, 0);
	}
}

void CLocalPlayer::CheckWeapons()
{
	if (m_pPlayerPed->IsInVehicle()) return;

	//uint8_t byteCurWep = m_pPlayerPed->GetCurrentWeapon();
	bool bMSend = false;

	for (int i = 0; i < 13; i++) {

		if (m_byteLastWeapon[i] != m_pPlayerPed->m_pPed->m_aWeapons[i].dwType ||
			m_dwLastAmmo[i] != m_pPlayerPed->m_pPed->m_aWeapons[i].dwAmmo)
		{
			m_byteLastWeapon[i] = m_pPlayerPed->m_pPed->m_aWeapons[i].dwType;
			m_dwLastAmmo[i] = m_pPlayerPed->m_pPed->m_aWeapons[i].dwAmmo;

			bMSend = true;
			break;
		}

	}
	if (bMSend) {
		RakNet::BitStream bsWeapons;
		bsWeapons.Write((BYTE) ID_WEAPONS_UPDATE);

		for (int i = 0; i < 13; i++) {

			bsWeapons.Write((uint8_t) i);
			bsWeapons.Write((uint8_t) m_byteLastWeapon[i]);
			bsWeapons.Write((uint16_t) m_dwLastAmmo[i]);
		}
		pNetGame->GetRakClient()->Send(&bsWeapons, HIGH_PRIORITY, RELIABLE, 0);
	}
}
// 0.3.7
void CLocalPlayer::UpdateRemoteInterior(uint8_t byteInterior)
{
	m_byteCurInterior = byteInterior;
	RakNet::BitStream bsUpdateInterior;
	bsUpdateInterior.Write(byteInterior);
	pNetGame->GetRakClient()->RPC(&RPC_SetInteriorId, &bsUpdateInterior, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::UpdateCameraTarget()
{

}

void CLocalPlayer::ProcessSurfing() {
	if(m_pPlayerPed && !m_pPlayerPed->IsDead() && !LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) {
        CVehicleGTA* contactVeh = (CVehicleGTA*)m_pPlayerPed->GetEntityUnderPlayer();
		if(contactVeh){
			VEHICLEID vehicleId = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(contactVeh);
			if(vehicleId && vehicleId != INVALID_VEHICLE_ID){
				CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehicleId);
				if(pVeh && (pVeh->HasADriver() || pVeh->m_pVehicle->GetModelId() == 569 || pVeh->m_pVehicle->GetModelId() == 570)
						   && pVeh->m_pVehicle->GetDistanceFromLocalPlayerPed() < 30.0){
					/*bool onFootObject = ScriptCommand(&is_char_touching_vehicle, m_pPlayerPed->m_dwGTAId, pVeh->m_dwGTAId);
                    if(onFootObject){*/
					if(m_surfData.bIsActive){
						return;
					}
					memset(&m_surfData, 0, sizeof(m_surfData));
					m_surfData.vecOffsetPos = CVector{0.0f, 0.0f, 0.0f};
					m_surfData.dwSurfVehID = vehicleId;
					m_surfData.pSurfInst = (uintptr_t)pVeh;
					m_surfData.bIsVehicle = true;

					static RwMatrix matVeh;
                    matVeh = pVeh->m_pVehicle->GetMatrix().ToRwMatrix();
					static RwMatrix matPed;
                    matPed = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
					static RwMatrix matOut;
					mat_invert(&matOut, &matVeh);
					ProjectMatrix(&m_surfData.vecOffsetPos, (CMatrix*)(&matOut), (CVector*)&matPed.pos);

					m_surfData.bIsActive = true;
					return;
					//}
				}
			}else{
                CEntityGTA* contactEntity = m_pPlayerPed->GetEntityUnderPlayer();
				if(contactEntity){
					uint32_t objectId = pNetGame->GetObjectPool()->FindIDFromGtaPtr(
                            dynamic_cast<CPhysical *>(contactEntity));
					if(objectId && objectId != INVALID_OBJECT_ID){
						CObject* pObject = pNetGame->GetObjectPool()->GetAt(objectId);
						if(pObject){
							//bool onFootObject = ScriptCommand(&is_char_touching_object, m_pPlayerPed->m_dwGTAId, pObject->m_dwGTAId);
							//if(onFootObject) {
							if(m_surfData.bIsActive){
								return;
							}
							memset(&m_surfData, 0, sizeof(m_surfData));
							m_surfData.bIsVehicle = false;
							m_surfData.pSurfInst = (uintptr_t) pObject;
							m_surfData.bIsActive = true;
							return;
							//}
						}
					}
				}
			}
		}
	}

	m_surfData.bIsActive = false;
	m_surfData.dwSurfVehID = INVALID_VEHICLE_ID;
	m_surfData.pSurfInst = 0;
	m_surfData.vecOffsetPos = CVector{0.0f, 0.0f, 0.0f};

}
void CLocalPlayer::UpdateSurfing() {
	static RwMatrix surfInstMatrix;
	static RwMatrix surfPedMatrix;
	static CVector surfInstMoveSpeed;
	static CVector surfInstTurnSpeed;
	if(m_pPlayerPed) {
		if (LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) {
			return;
		}
		if (m_surfData.bIsActive) {
			if (m_surfData.bIsVehicle) {
				CVehicle *pVeh = (CVehicle *) m_surfData.pSurfInst;
                surfInstMatrix = pVeh->m_pVehicle->GetMatrix().ToRwMatrix();
                surfPedMatrix = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
                surfInstMoveSpeed = pVeh->m_pVehicle->GetMoveSpeed();
                surfInstTurnSpeed = pVeh->m_pVehicle->GetTurnSpeed();

				uint16_t lrAnalog;
				uint16_t udAnalog;
				m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

				if (lrAnalog || udAnalog) {
					static RwMatrix matOut;
					mat_invert(&matOut, &surfInstMatrix);
					ProjectMatrix(&m_surfData.vecOffsetPos, (CMatrix*)&matOut,  (CVector*)&surfPedMatrix.pos);
				} else {
					ProjectMatrix( (CVector*)&surfPedMatrix.pos, (CMatrix*)&surfInstMatrix,  (CVector*)&m_surfData.vecOffsetPos);

					m_pPlayerPed->m_pPed->SetMatrix((CMatrix&)surfPedMatrix);
					CVector vecMoveSpeed;
                    vecMoveSpeed = m_pPlayerPed->m_pPed->GetMoveSpeed();
					m_pPlayerPed->m_pPed->SetVelocity(
							CVector{surfInstMoveSpeed.x, surfInstMoveSpeed.y, vecMoveSpeed.z});

					CVector vecTurnSpeed = m_pPlayerPed->m_pPed->GetTurnSpeed();
					m_pPlayerPed->m_pPed->SetTurnSpeed(
							CVector{vecTurnSpeed.x, vecTurnSpeed.y, surfInstTurnSpeed.z});
				}
			} else {
				CObject *pObject = (CObject *) m_surfData.pSurfInst;
				if (pObject && pObject->m_byteMoving & 1) {
                    surfInstMatrix = pObject->m_pEntity->GetMatrix().ToRwMatrix();
                    surfPedMatrix = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
                    surfInstMoveSpeed = pObject->m_pEntity->GetMoveSpeed();
                    surfInstTurnSpeed = pObject->m_pEntity->GetTurnSpeed();

					uint16_t lrAnalog;
					uint16_t udAnalog;
					m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

					if (lrAnalog || udAnalog) {
						static RwMatrix matOut;
						mat_invert(&matOut, &surfInstMatrix);
						ProjectMatrix(&m_surfData.vecOffsetPos, (CMatrix*)&matOut,  (CVector*)&surfPedMatrix.pos);
					} else {
						ProjectMatrix( (CVector*)&surfPedMatrix.pos, (CMatrix*)&surfInstMatrix,
									  &m_surfData.vecOffsetPos);

						m_pPlayerPed->m_pPed->SetMatrix((CMatrix&)surfPedMatrix);
						CVector vecMoveSpeed = m_pPlayerPed->m_pPed->GetMoveSpeed();
						m_pPlayerPed->m_pPed->SetVelocity(
								CVector{surfInstMoveSpeed.x, surfInstMoveSpeed.y, vecMoveSpeed.z});

						CVector vecTurnSpeed = m_pPlayerPed->m_pPed->GetTurnSpeed();
						m_pPlayerPed->m_pPed->SetTurnSpeed(
								CVector{vecTurnSpeed.x, vecTurnSpeed.y, surfInstTurnSpeed.z});
					}
				}

			}
		}

	}
}

void CLocalPlayer::MoveHeadWithCamera()
{

}
extern bool bNeedEnterVehicle;
// 0.3.7
/*bool CLocalPlayer::EnterVehicleAsPassenger()
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	// CTouchInterface::IsDoubleTapped
	//int isHeldDown = ((int (*)(int, bool, int))(g_libGTASA + 0x2B2068 + 1))(0, true, 1);
	//if (!isHeldDown) return false;
	//if (!bNeedEnterVehicle) return false;

	VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
	CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
	if (!pVehicle) return false;

	if (pVehicle->GetDistanceFromLocalPlayerPed() < 8.0f)
	{
		if (m_pPlayerPed->GetCurrentWeapon() == WEAPON_PARACHUTE) {
			m_pPlayerPed->SetArmedWeapon(0, false);
		}

		m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
		SendEnterVehicleNotification(ClosetVehicleID, true);
		bNeedEnterVehicle = false;
		return true;
	}
	bNeedEnterVehicle = false;
	return false;
}*/

bool CLocalPlayer::HandlePassengerEntry()
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	// CTouchInterface::IsDoubleTapped
	//int isHeldDown = ((int (*)(int, bool, int))(g_libGTASA + 0x2B2068 + 1))(0, true, 1);
	//if (!isHeldDown) return false;

	if (!bNeedEnterVehicle) return false;
	VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
	CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
	if (!pVehicle) return false;

	if (pVehicle->m_pVehicle->GetDistanceFromLocalPlayerPed() < 8.0f)
	{
		if (m_pPlayerPed->GetCurrentWeapon() == WEAPON_PARACHUTE) {
			m_pPlayerPed->SetArmedWeapon(0, false);
		}

		m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
		SendEnterVehicleNotification(ClosetVehicleID, true);
		bNeedEnterVehicle = false;
	}
	bNeedEnterVehicle = false;
	return true;
}

bool CLocalPlayer::GbuttonEnterVehicleAsPassenger()
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!bNeedEnterVehicle) return false;
	VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
	CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
	if (!pVehicle) return false;

	if (pVehicle->m_pVehicle->GetDistanceFromLocalPlayerPed() < 8.0f)
	{
		if (m_pPlayerPed->GetCurrentWeapon() == WEAPON_PARACHUTE) {
			m_pPlayerPed->SetArmedWeapon(0, false);
		}

		m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
		SendEnterVehicleNotification(ClosetVehicleID, true);
		bNeedEnterVehicle = false;
		return true;
	}
	bNeedEnterVehicle = false;
	return false;
}


bool CLocalPlayer::EnterVehicleAsDriver()
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
	CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
	if (!pVehicle) return false;

	if (pVehicle->m_pVehicle->GetDistanceFromLocalPlayerPed() < 8.0f)
	{
		if (m_pPlayerPed->GetCurrentWeapon() == WEAPON_PARACHUTE) {
			m_pPlayerPed->SetArmedWeapon(0, false);
		}

		m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, false);
		//SendEnterVehicleNotification(ClosetVehicleID, false);
		return true;
	}

	return false;
}

/*bool g_bForceEnterVehicle = false;

void CLocalPlayer::HandlePassengerEntry()
{
	if (bNeedEnterVehicle == true)
	{
		//GbuttonEnterVehicleAsPassenger();
		EnterVehicleAsPassenger();
		//bNeedEnterVehicle = false;
		return;
	}
	static uint32_t dwTimeElapsed = GetTickCount();
	static int iReleasedCount = 0;

	// íàæàòà êíîïêà ïîñàäêè â àâòî?
	int isReleased = ((int (*)(int, int, int))(g_libGTASA + 0x2B0D5C + 1))(0, 0, true);

	// êíîïêà íàæàòà â ïåðâûé ðàç?
	if (isReleased == true && iReleasedCount == 0)
	{
		dwTimeElapsed = GetTickCount();
		iReleasedCount = 1;
		return;
	}

	// óâåëè÷èâàåì ñ÷åò÷èê
	if (isReleased) iReleasedCount++;

	// âðåìÿ íà äàáë-êëèê ïðîøëî
	if (GetTickCount() - dwTimeElapsed > 300)
	{
		// ïðîèçâåäåíî 2 íàæàòèÿ çà 500ìñ
		if (iReleasedCount == 2)
		{
			EnterVehicleAsPassenger();
		}
		else if (iReleasedCount == 1)
		{
			//EnterVehicleAsDriver();
			g_bForceEnterVehicle = true;
		}

		// îáíóëÿåì ñ÷åò÷èêè
		dwTimeElapsed = GetTickCount();
		iReleasedCount = 0;
	}
}*/

void CLocalPlayer::ProcessOnFootWorldBounds()
{
	if(pGame->GetActiveInterior() != 0) return; // can't enforce inside interior

	/*if(m_pPlayerPed->EnforceWorldBoundries(
			pNetGame->m_pNetSet->fWorldBounds[0], pNetGame->m_pNetSet->fWorldBounds[1],
			pNetGame->m_pNetSet->fWorldBounds[2], pNetGame->m_pNetSet->fWorldBounds[3]))
	{
		m_pPlayerPed->SetArmedWeapon(0, 0);
		pGame->DisplayGameText("Stay within the ~r~world boundries", 1000, 5);
	}*/
}

void CLocalPlayer::ProcessInCarWorldBounds()
{
	/*CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool)
	{
		if(pGame->GetActiveInterior() != 0) return; // can't enforce inside interior

		VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
		if(vehicleId != INVALID_VEHICLE_ID)
		{
			CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
			if(pVehicle)
			{
				if(pVehicle->EnforceWorldBoundries(
						pNetGame->m_pNetSet->fWorldBounds[0], pNetGame->m_pNetSet->fWorldBounds[1],
						pNetGame->m_pNetSet->fWorldBounds[2], pNetGame->m_pNetSet->fWorldBounds[3]))
				{
					pGame->DisplayGameText("Stay within the ~r~world boundries", 1000, 5);
				}
			}
		}
	}*/
}
// 0.3.7
bool CLocalPlayer::CompareOnFootSyncKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	return wKeys != m_ofSync.wKeys || udAnalog != m_ofSync.udAnalog || lrAnalog != m_ofSync.lrAnalog;
}
// 0.3.7
int CLocalPlayer::GetOptimumOnFootSendRate()
{
	if(!m_pPlayerPed) return 1000;

	if(pNetGame->m_bLanMode) return 15;
	else
	{
		int iNumPlayersInRange = 0;
		for(int i = 2; i < 120; i++)
			if(bUsedPlayerSlots[i]) iNumPlayersInRange++;

		return (iNetModeNormalOnFootSendRate + iNumPlayersInRange);
	}
}
// 0.3.7
int CLocalPlayer::GetOptimumInCarSendRate()
{
	if(!m_pPlayerPed) return 1000;

	if(pNetGame->m_bLanMode) return 15;
	else
	{
		int iNumPlayersInRange = 0;
		for(int i = 0; i < 120; i++)
			if(bUsedPlayerSlots[i]) iNumPlayersInRange++;

		return (iNetModeNormalInCarSendRate + iNumPlayersInRange);
	}
}

void CLocalPlayer::UpdateVehicleDamage(VEHICLEID vehicleID)
{

}
// 0.3.7
void CLocalPlayer::SendNextClass()
{
	RwMatrix matPlayer;

	if (!m_bSpawnDialogShowed) return;

	m_bClearedToSpawn = false;
    matPlayer = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();

	if (m_iSelectedClass == (pNetGame->m_pNetSet->iSpawnsAvailable - 1)) m_iSelectedClass = 0;
	else m_iSelectedClass++;

	pGame->PlaySound(1052, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}
// 0.3.7
void CLocalPlayer::SendPrevClass()
{
	RwMatrix matPlayer;

	if (!m_bSpawnDialogShowed) return;

	m_bClearedToSpawn = false;
    matPlayer = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();

	if (m_iSelectedClass == 0) m_iSelectedClass = (pNetGame->m_pNetSet->iSpawnsAvailable - 1);
	else m_iSelectedClass--;

	pGame->PlaySound(1053, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}
// 0.3.7
void CLocalPlayer::SendSpawn()
{
	if (!m_bSpawnDialogShowed) return;

	RequestSpawn();
	m_bWaitingForSpawnRequestReply = true;
}
// 0.3.7
void CLocalPlayer::RequestClass(int iClass)
{
	RakNet::BitStream bsClassRequest;
	bsClassRequest.Write(iClass);
	pNetGame->GetRakClient()->RPC(&RPC_RequestClass, &bsClassRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7
void CLocalPlayer::RequestSpawn()
{
	RakNet::BitStream bsSpawnRequest;
	pNetGame->GetRakClient()->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::ApplySpecialAction(uint8_t byteSpecialAction)
{
	if(!m_pPlayerPed) return;

	m_pPlayerPed->iSpecialAction = byteSpecialAction;

	if(byteSpecialAction != SPECIAL_ACTION_USECELLPHONE && m_pPlayerPed->IsCellphoneEnabled())
		m_pPlayerPed->ToggleCellphone(0);
	if(byteSpecialAction != SPECIAL_ACTION_USEJETPACK && m_pPlayerPed->IsInJetpackMode())
		m_pPlayerPed->StopJetpack();
	if(byteSpecialAction != SPECIAL_ACTION_HANDSUP && m_pPlayerPed->HasHandsUp())
		m_pPlayerPed->TogglePlayerControllable(true);
	if(m_pPlayerPed->GetDanceStyle() != -1)
	{
		if((byteSpecialAction != SPECIAL_ACTION_DANCE1 ||
			byteSpecialAction != SPECIAL_ACTION_DANCE2 ||
			byteSpecialAction != SPECIAL_ACTION_DANCE3 ||
			byteSpecialAction != SPECIAL_ACTION_DANCE4))
		{
			m_pPlayerPed->StopDancing();
		}
	}
	if(byteSpecialAction != SPECIAL_ACTION_PISSING && m_pPlayerPed->IsPissing())
		m_pPlayerPed->StopPissing();
	if(m_pPlayerPed->GetStuff() != eStuffType::STUFF_TYPE_NONE)
	{
		if(byteSpecialAction != SPECIAL_ACTION_DRINK_BEER ||
			byteSpecialAction != SPECIAL_ACTION_SMOKE_CIGGY ||
			byteSpecialAction != SPECIAL_ACTION_DRINK_WINE ||
			byteSpecialAction != SPECIAL_ACTION_DRINK_SPRUNK)
		{
			m_pPlayerPed->DropStuff();
		}
	}

	switch(byteSpecialAction)
	{
		default:
		case SPECIAL_ACTION_NONE:
			// ~
		break;

		case SPECIAL_ACTION_USECELLPHONE:
			m_pPlayerPed->ToggleCellphone(1);
		break;

		case SPECIAL_ACTION_STOPUSECELLPHONE:
			if(m_pPlayerPed->IsCellphoneEnabled()) m_pPlayerPed->ToggleCellphone(0);
		break;

		case SPECIAL_ACTION_USEJETPACK:
			if(!m_pPlayerPed->IsInJetpackMode()) m_pPlayerPed->StartJetpack();
		break;

		case SPECIAL_ACTION_HANDSUP:
			m_pPlayerPed->HandsUp();
		break;

		case SPECIAL_ACTION_PISSING:
			m_pPlayerPed->StartPissing();
		break;

		case SPECIAL_ACTION_DANCE1:
			m_pPlayerPed->StartDancing(1);
		break;

		case SPECIAL_ACTION_DANCE2:
			m_pPlayerPed->StartDancing(2);
		break;

		case SPECIAL_ACTION_DANCE3:
			m_pPlayerPed->StartDancing(3);
		break;

		case SPECIAL_ACTION_DANCE4:
			m_pPlayerPed->StartDancing(4);
		break;

		case SPECIAL_ACTION_DRINK_BEER:
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_BEER);
		break;

		case SPECIAL_ACTION_SMOKE_CIGGY:
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_CIGGI);
		break;

		case SPECIAL_ACTION_DRINK_WINE:
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_DYN_BEER);
		break;

		case SPECIAL_ACTION_DRINK_SPRUNK:
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_PINT_GLASS);
		break;
	}
}
// 0.3.7
void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO* pSpawnInfo)
{
	memcpy(&m_SpawnInfo, pSpawnInfo, sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = true;
}
// 0.3.7
void CLocalPlayer::HandleClassSelectionOutcome(bool bOutcome)
{
	if (bOutcome)
	{
		if (m_pPlayerPed)
		{
			m_pPlayerPed->ClearWeapons();
			m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
		}

		m_bClearedToSpawn = true;
	}
}

uint8_t CLocalPlayer::GetSpecialAction()
{
	if(!m_pPlayerPed) return SPECIAL_ACTION_NONE;

	if(m_pPlayerPed->IsCrouching())
		return SPECIAL_ACTION_DUCK;

	if(m_pPlayerPed->IsEnteringVehicle() != 0)
		return SPECIAL_ACTION_ENTER_VEHICLE;

	if(m_pPlayerPed->IsExitingVehicle())
		return SPECIAL_ACTION_EXIT_VEHICLE;

	if(m_pPlayerPed->IsSitTask())
		return SPECIAL_ACTION_SITTING;

	if(m_pPlayerPed->IsInJetpackMode())
		return SPECIAL_ACTION_USEJETPACK;

	if(m_pPlayerPed->IsCuffed())
		return SPECIAL_ACTION_CUFFED;

	if(m_pPlayerPed->IsCarry())
		return SPECIAL_ACTION_CARRY;

	if(m_pPlayerPed->GetDanceStyle() != -1)
	{
		switch(m_pPlayerPed->GetDanceStyle())
		{
			case 0:
				return SPECIAL_ACTION_DANCE1;
				break;
			case 1:
				return SPECIAL_ACTION_DANCE2;
				break;
			case 2:
				return SPECIAL_ACTION_DANCE3;
				break;
			case 3:
				return SPECIAL_ACTION_DANCE4;
				break;
		}
	}

	if(m_pPlayerPed->HasHandsUp())
		return SPECIAL_ACTION_HANDSUP;

	if(m_pPlayerPed->IsCellphoneEnabled())
		return SPECIAL_ACTION_USECELLPHONE;

	if(m_pPlayerPed->IsPissing())
		return SPECIAL_ACTION_PISSING;

	if(m_pPlayerPed->GetStuff() == eStuffType::STUFF_TYPE_BEER)
		return SPECIAL_ACTION_DRINK_BEER;

	if(m_pPlayerPed->GetStuff() == eStuffType::STUFF_TYPE_DYN_BEER)
		return SPECIAL_ACTION_DRINK_WINE;

	if(m_pPlayerPed->GetStuff() == eStuffType::STUFF_TYPE_PINT_GLASS)
		return SPECIAL_ACTION_DRINK_SPRUNK;

	if(m_pPlayerPed->GetStuff() == eStuffType::STUFF_TYPE_CIGGI)
		return SPECIAL_ACTION_SMOKE_CIGGY;

	return SPECIAL_ACTION_NONE;
}
// 0.3.7
uint32_t CLocalPlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID()) >> 8) | 0xFF000000;
}
// 0.3.7
uint32_t CLocalPlayer::GetPlayerColorAsRGBA()
{
	return TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID());
}
// 0.3.7
bool CLocalPlayer::IsNeedSyncDataSend(const void* data1, const void* data2, size_t size)
{
	if (GetTickCount() - m_dwLastSendSyncTick <= 500 && !memcmp(data1, data2, size)) {
		return false;
	}

	m_dwLastSendSyncTick = GetTickCount();
	return true;
}
// 0.3.7
void CLocalPlayer::SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
	uint8_t bytePassenger = 0;

	if(bPassenger)
		bytePassenger = 1;
	RakNet::BitStream bsSend;
	bsSend.Write(VehicleID);
	bsSend.Write(bytePassenger);
	pNetGame->GetRakClient()->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if (pVehiclePool)
	{
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pVehicle)
		{
			if (pVehicle->IsATrainPart())
			{
				ScriptCommand(&camera_on_vehicle, pVehicle->m_dwGTAId, 3, 2);
				dwEnterVehTimeElasped = GetTickCount();
			}
		}
	}
}
// 0.3.7
void CLocalPlayer::SpectatePlayer(PLAYERID PlayerID)
{
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool && pPlayerPool->GetSlotState(PlayerID))
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer->GetState() != PLAYER_STATE_NONE &&
			pRemotePlayer->GetState() != PLAYER_STATE_WASTED)
		{
			m_byteSpectateType = SPECTATE_TYPE_PLAYER;
			m_SpectateID = PlayerID;
			m_bSpectateProcessed = false;
		}
	}
}

void CLocalPlayer::SpectateVehicle(VEHICLEID VehicleID)
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if (pVehiclePool && pVehiclePool->GetSlotState(VehicleID))
	{
		m_byteSpectateType = SPECTATE_TYPE_VEHICLE;
		m_SpectateID = VehicleID;
		m_bSpectateProcessed = false;
	}
}
// 0.3.7
void CLocalPlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(pNetGame->GetPlayerPool()->GetLocalPlayerID(), dwColor);
}
// 0.3.7
void CLocalPlayer::SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle)
	{
		if (!m_pPlayerPed->IsAPassenger())
			m_LastVehicle = VehicleID;

		if (pVehicle->IsATrainPart())
            CCamera::SetBehindPlayer();

		if (!pVehicle->IsRCVehicle())
		{
			bsSend.Write(VehicleID);
			pNetGame->GetRakClient()->RPC(&RPC_ExitVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
		}
	}
}
// 0.3.7
void CLocalPlayer::SendTakeDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType)
{
	fDamageFactor *= 0.33f;

	RakNet::BitStream bsSend;
	bsSend.Write(true);
	bsSend.Write(PlayerID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7
void CLocalPlayer::SendGiveDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType)
{
	fDamageFactor *= 0.33f;

	RakNet::BitStream bsSend;
	bsSend.Write(false);
	bsSend.Write(PlayerID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7
void CLocalPlayer::SendGiveDamageActorEvent(PLAYERID ActorID, float fDamageFactor, int weaponType, int pedPieceType)
{
	RakNet::BitStream bsSend;
	bsSend.Write(false);
	bsSend.Write(ActorID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_GiveDamageActor, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::GiveTakeDamage(bool bGiveOrTake, uint16_t wPlayerID, float damage_amount, uint32_t weapon_id, uint32_t bodypart)
{
	RakNet::BitStream bitStream;

	bitStream.Write((bool)bGiveOrTake);
	bitStream.Write((uint16_t)wPlayerID);
	bitStream.Write((float)damage_amount);
	bitStream.Write((uint32_t)weapon_id);
	bitStream.Write((uint32_t)bodypart);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::GiveActorDamage(PLAYERID wPlayerID, float damage_amount, uint32_t weapon_id, uint32_t bodypart)
{
	RakNet::BitStream bitStream;
	bitStream.Write((uint16_t)wPlayerID);
	bitStream.Write((float)damage_amount);
	bitStream.Write((uint32_t)weapon_id);
	bitStream.Write((uint32_t)bodypart);

	int RPC_GiveActorDamage = 177;
	pNetGame->GetRakClient()->RPC(&RPC_GiveActorDamage, &bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

/*
void CLocalPlayer::UpdateVoice()
{
	if (!pVoice || !pUI) return;
	VoiceButton* vbutton = pUI->voicebutton();

	switch (m_iVCState)
	{
	case VOICE_CHANNEL_STATE_CLOSED:
		if (vbutton->recording()) {
			// îòïðàâëÿåì çàïðîñ íà îòêðûòèå êàíàëà
			SendVCOpenRequest();
		}
		break;

	case VOICE_CHANNEL_STATE_WAIT_ACCEPT:
		// æäåì îòâåòà áîëüøå 2 ñåêóíä? 
		if (!vbutton->recording() || GetTickCount() - m_dwVCOpenRequestTick > 2000) {
			SendVCClose();
		}
		break;

	case VOICE_CHANNEL_STATE_ACCEPTED:
		if (vbutton->recording()) {
			SendVCData();
		}
		else {
			SendVCClose();
		}
		break;
	}
}
*/

/*
void CLocalPlayer::VoiceChannelAccept()
{
	m_iVCState = VOICE_CHANNEL_STATE_ACCEPTED;
}*/

/*
void CLocalPlayer::VoiceChannelClose()
{
	pUI->voicebutton()->setRecording(false);
	m_iVCState = VOICE_CHANNEL_STATE_CLOSED;
}
*/

/*
void CLocalPlayer::SendVCOpenRequest()
{
	RakNet::BitStream bsData;
	bsData.Write((char)ID_VOICE_CHANNEL_OPEN_REQUEST);
	pNetGame->GetRakClient()->Send(&bsData, HIGH_PRIORITY, RELIABLE_ORDERED, 0);

	m_dwVCOpenRequestTick = GetTickCount();
	m_iVCState = VOICE_CHANNEL_STATE_WAIT_ACCEPT;
}*/

/*
void CLocalPlayer::SendVCData()
{
	unsigned char voiceData[MAX_VOICE_PACKET_SIZE];
	int size = pVoice->Frame(voiceData, sizeof(voiceData));
	if (size == -1) return;

	RakNet::BitStream bsData;
	bsData.Write((char)ID_VOICE_DATA);
	bsData.Write(size);
	bsData.Write((char*)voiceData, size);
	pNetGame->GetRakClient()->Send(&bsData, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}*/

/*
void CLocalPlayer::SendVCClose()
{
	RakNet::BitStream bsData;
	bsData.Write((char)ID_VOICE_CHANNEL_CLOSE);
	pNetGame->GetRakClient()->Send(&bsData, HIGH_PRIORITY, RELIABLE_ORDERED, 0);

	VoiceChannelClose();
}
*/

uint32_t CLocalPlayer::GetCurrentAnimationIndexFlag()
{
	uint32_t dwAnim = 0;

	float fBlendData = 4.0f;

	int iAnimIdx = m_pPlayerPed->GetCurrentAnimationIndex();

	uint32_t hardcodedBlend = 0b00000100;	// 4
	hardcodedBlend <<= 16;

	uint32_t hardcodedFlags = 0;

	if (iAnimIdx)
	{
		hardcodedFlags = 0b00010001;
	}
	else
	{
		hardcodedFlags = 0b10000000;
		iAnimIdx = 1189;
	}

	hardcodedFlags <<= 24;

	uint16_t usAnimidx = (uint16_t)iAnimIdx;

	dwAnim = (uint32_t)usAnimidx;
	dwAnim |= hardcodedBlend;
	dwAnim |= hardcodedFlags;

	return dwAnim;
}

void CLocalPlayer::SendBulletSyncData(PLAYERID byteHitID, uint8_t byteHitType, CVector vecHitPos)
{
    if (!m_pPlayerPed) return;
    switch (byteHitType)
    {
        case BULLET_HIT_TYPE_NONE:
            break;
        case BULLET_HIT_TYPE_PLAYER:
            if (!pNetGame->GetPlayerPool()->GetSlotState((PLAYERID)byteHitID)) return;
            break;

    }
	uint8_t byteCurrWeapon = m_pPlayerPed->GetCurrentWeapon(), byteShotWeapon;
    //uint8_t byteCurrWeapon = m_pPlayerPed->GetCurrentWeapon(), byteShotWeapon;

    RwMatrix matPlayer;
    BULLET_SYNC blSync;

    matPlayer = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();

    blSync.hitId = byteHitID;
    blSync.hitType = byteHitType;

    if (byteHitType == BULLET_HIT_TYPE_PLAYER)
    {
        float fDistance = pNetGame->GetPlayerPool()->GetAt((PLAYERID)byteHitID)->GetPlayerPed()->m_pPed->GetDistanceFromLocalPlayerPed();
        if (byteCurrWeapon != 0 && fDistance < 1.0f)
            byteShotWeapon = 0;
        else
            byteShotWeapon = byteCurrWeapon;
    }
    else
    {
        byteShotWeapon = m_pPlayerPed->GetCurrentWeapon();
    }
    blSync.weapId = byteShotWeapon;

    blSync.hitPos[0] = vecHitPos.x;
    blSync.hitPos[1] = vecHitPos.y;
    blSync.hitPos[2] = vecHitPos.z;

    blSync.offsets[0] = 0.0f;
    blSync.offsets[1] = 0.0f;
    blSync.offsets[2] = 0.0f;

	FLog("SendBulletSync: %d, %d, %d, %f, %f, %f, %f, %f, %f", blSync.hitId, blSync.hitType, blSync.weapId,
		 blSync.hitPos[0], blSync.hitPos[1], blSync.hitPos[2], blSync.offsets[0], blSync.offsets[1], blSync.offsets[2]);

    RakNet::BitStream bsBulletSync;
    bsBulletSync.Write((uint8_t)ID_BULLET_SYNC);
    bsBulletSync.Write((const char*)& blSync, sizeof(BULLET_SYNC));
    pNetGame->GetRakClient()->Send(&bsBulletSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

int CLocalPlayer::GetOptimumUnoccupiedSendRate()
{
	if(!m_pPlayerPed) return 1000;

	if(m_pPlayerPed->GetGtaVehicle()) // ramming an unoccupied vehicle
		return GetOptimumInCarSendRate();
	else return GetOptimumOnFootSendRate(); // pushing an unoccupied vehicle
}

bool CLocalPlayer::ProcessUnoccupiedSync(VEHICLEID vehicleId, CVehicle *pVehicle)
{
	if((GetTickCount() - m_dwLastSendTick) > (unsigned int)GetOptimumUnoccupiedSendRate())
	//if(vehicleId <= 2000 && pVehicle)
	{
		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if(!pPlayerPool || !pVehiclePool) return false;

        CVehicleGTA *pVehicleType = pVehicle->m_pVehicle;
		if(pVehicleType && m_pPlayerPed && !pVehicle->IsATrainPart() &&
		   !pVehicle->IsATrailer() && !pVehicle->GetTractor())
		{
			CPedGTA *pDriver = pVehicleType->pDriver;
			if(pDriver && pDriver->IsInVehicle() ||
			   pVehicle->m_pVehicle->GetDistanceFromLocalPlayerPed() > 90.0f /*||
			   pVehicle->m_pVehicle->IsStationary()*/)
			{
				return false;
			}

			float fDistance = 0.0f, fSmallest = 1000000.0f;
			PLAYERID iClosestPlayerId = 0;

			for(int i = 0; i < 7; i++)
			{
				CPedGTA *pPassenger = pVehicleType->m_apPassengers[i];
				if(pPassenger && pPassenger->m_nPedType == (ePedType)0)
				{
					if(pPassenger == m_pPlayerPed->m_pPed) goto sync;
					return false;
				}
			}

			for(PLAYERID i = 0; i < MAX_PLAYERS; i++)
			{
				CPlayerPed* pPlayerPed;

				if(i == pPlayerPool->GetLocalPlayerID())
					pPlayerPed = m_pPlayerPed;
				else
				{
					CRemotePlayer* pTmpPlayer = pPlayerPool->GetAt(i);
					if(pTmpPlayer) pPlayerPed = pTmpPlayer->GetPlayerPed();
				}

				if(pVehicle && pPlayerPed && pPlayerPed->m_pPed->IsAdded())
				{
					fDistance = pPlayerPed->GetDistanceFromVehicle(pVehicle);
					if(i)
					{
						if(fDistance < fSmallest)
						{
							fSmallest = fDistance;
							iClosestPlayerId = i;
						}
					}
					else fSmallest = fDistance;
				}
			}

			if(iClosestPlayerId == pPlayerPool->GetLocalPlayerID() && fSmallest <= 90.0f)
			{
				sync:
				SendUnoccupiedData(vehicleId, pVehicle);
                m_dwLastSendTick = GetTickCount();
				return true;
			}
		}
	}

	return false;
}

void CompressNormalVector(CVector *vecOut, CVector vecIn)
{
	vecOut->x = (short)(vecIn.x * 10000.0);
	vecOut->y = (short)(vecIn.y * 10000.0);
	vecOut->z = (short)(vecIn.z * 10000.0);
}

void CLocalPlayer::SendUnoccupiedData(VEHICLEID vehicleId, CVehicle *pVehicle)
{
	RakNet::BitStream bsUnoccupiedSync;
	CVector vecMoveSpeed, vecTurnSpeed;
	RwMatrix matVehicle;
	UNOCCUPIED_SYNC_DATA unSync;

    matVehicle = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();

	CompressNormalVector(&unSync.vecRoll, matVehicle.right);
	CompressNormalVector(&unSync.vecDirection, matVehicle.up);

    unSync.vehicleId = vehicleId;

	unSync.byteSeatId = m_pPlayerPed->GetVehicleSeatID();

    unSync.vecMoveSpeed = pVehicle->m_pVehicle->GetMoveSpeed();
    unSync.vecTurnSpeed = pVehicle->m_pVehicle->GetTurnSpeed();

	unSync.vecPos.x = matVehicle.pos.x;
	unSync.vecPos.y = matVehicle.pos.y;
	unSync.vecPos.z = matVehicle.pos.z;

	unSync.fCarHealth = pVehicle->GetHealth();

	if(IsNeedSyncDataSend(&m_UnoccupiedData, &unSync, sizeof(UNOCCUPIED_SYNC_DATA)))
	{
		bsUnoccupiedSync.Write((uint8_t)ID_UNOCCUPIED_SYNC);
		bsUnoccupiedSync.Write((char*)&unSync, sizeof(UNOCCUPIED_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsUnoccupiedSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_UnoccupiedData, &unSync, sizeof(UNOCCUPIED_SYNC_DATA));
	}
}