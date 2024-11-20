#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "remoteplayer.h"
#include "../gui/gui.h"
#include "../vendor/encoding/encoding.h"

extern UI* pUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
//extern CVoice* pVoice;

CRemotePlayer::CRemotePlayer()
{
	ResetAllSyncAttributes();

	m_VehicleID = 0;
	m_byteState = PLAYER_STATE_NONE;
	m_PlayerID = INVALID_PLAYER_ID;
	m_pPlayerPed = nullptr;
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_bShowNameTag = true;
	// field_C = 0;
	// field_14 = 0;
	m_dwMarker = 0;
	m_byteSpecialAction = SPECIAL_ACTION_NONE;
	m_bIsNPC = false;
	m_bAppliedAnimation = false;
	// field_1E1 = GetTickCount();
	// field_1E5 = GetTickCount();
	m_dwLastRecvTick = GetTickCount();
	m_bWasAnimSettedFlag = false;
	m_dwPlayingAnimIndex = 0;
}

CRemotePlayer::~CRemotePlayer()
{
	if (m_dwMarker)
	{
		pGame->DisableMarker(m_dwMarker);
		m_dwMarker = 0;
	}

	// field_1E9 = 0;

	if (m_pPlayerPed) {
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}
}

void CRemotePlayer::Process()
{
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	CQuaternion quat;
	CVector vecMoveSpeed;
	RwMatrix matPlayer;
	RwMatrix matVehicle;

	if (IsActive())
	{
		// ---- ONFOOT NETWORK PROCESSING ----
		if (GetState() == PLAYER_STATE_ONFOOT &&
			m_byteUpdateFromNetwork == UPDATE_TYPE_ONFOOT &&
			!m_pPlayerPed->IsInVehicle())
		{
			UpdateOnFootPositionAndSpeed(&m_ofSync.vecPos, &m_ofSync.vecMoveSpeed);
			UpdateOnFootTargetPosition();

			// UPDATE CURRENT WEAPON
			uint8_t byteCurrentWeapon = m_ofSync.byteCurrentWeapon & 0x3F;
			if (m_pPlayerPed->IsAdded() && m_pPlayerPed->GetCurrentWeapon() != byteCurrentWeapon) {
				m_pPlayerPed->SetArmedWeapon(byteCurrentWeapon, false);

				// double check
				if (m_pPlayerPed->GetCurrentWeapon() != byteCurrentWeapon) {
					m_pPlayerPed->GiveWeapon(byteCurrentWeapon, 9999);
					m_pPlayerPed->SetArmedWeapon(byteCurrentWeapon, false);
				}
			}

			if(IS_TARGETING(m_ofSync.wKeys) && 
				(m_pPlayerPed->GetCurrentWeapon() == 34 ||
				m_pPlayerPed->GetCurrentWeapon() == 35 ||
				m_pPlayerPed->GetCurrentWeapon() == 36))
			{
				m_pPlayerPed->SetActionTrigger(ACTION_SCOPE);
			}
			else
			{
				ProcessAnimation();
				m_pPlayerPed->SetActionTrigger(ACTION_NORMAL);
			}

			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}
		// ---- DRIVER NETWORK PROCESSING ----
		else if (GetState() == PLAYER_STATE_DRIVER &&
			m_byteUpdateFromNetwork == UPDATE_TYPE_INCAR &&
			m_pPlayerPed->IsInVehicle())
		{
			if (!m_pCurrentVehicle || !m_pCurrentVehicle->VerifyInstance()) {
				return;
			}

			m_icSync.quat.Normalize();
			m_icSync.quat.GetMatrix(&matVehicle);
			matVehicle.pos.x = m_icSync.vecPos.x;
			matVehicle.pos.y = m_icSync.vecPos.y;
			matVehicle.pos.z = m_icSync.vecPos.z;

			if (m_pCurrentVehicle->GetModelIndex() == 538 ||
				m_pCurrentVehicle->GetModelIndex() == 537 ||
				m_pCurrentVehicle->GetModelIndex() == 449)
			{
				//UpdateTrainDriverMatrixAndSpeed(&matVehicle, &m_icSync.vecMoveSpeed, m_icSync.fTrainSpeed);
			}
			else
			{
				UpdateInCarMatrixAndSpeed(&matVehicle, &m_icSync.vecPos, &m_icSync.vecMoveSpeed);
				UpdateInCarTargetPosition();
			}

			if(m_icSync.byteSirenOn) m_pCurrentVehicle->EnableSiren(true);
			else m_pCurrentVehicle->EnableSiren(false);

			if (m_pCurrentVehicle->GetModelIndex() == HYDRA)
				m_pCurrentVehicle->m_pVehicle->wHydraThrusters = m_icSync.fTrainSpeed;

			if (m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PUSHBIKE)
			{
				//sub_100B7A40(_this->m_pCurrentVehicle, m_icSync.fTrainSpeed);
			}

			m_pCurrentVehicle->EnableSiren(m_icSync.byteSirenOn);
			//m_pCurrentVehicle->SetLandingGearState(m_icSync.byteLandingGearState);
			m_pCurrentVehicle->SetHealth(m_icSync.fCarHealth);

			PLAYERID TrailerID = m_icSync.TrailerID;
			if((TrailerID == 0 || TrailerID >= MAX_VEHICLES) && m_pCurrentVehicle->GetTrailer())
			{
				m_pCurrentVehicle->DetachTrailer();
				m_pCurrentVehicle->SetTrailer(0);
			}

			uint8_t byteCurWeapon = m_icSync.byteCurrentWeapon & 0x3F;
			if (m_pPlayerPed->GetCurrentWeapon() != byteCurWeapon)
			{
				m_pPlayerPed->SetArmedWeapon(byteCurWeapon, false);
				if (m_pPlayerPed->GetCurrentWeapon() != byteCurWeapon)
				{
					m_pPlayerPed->GiveWeapon(byteCurWeapon, 9999);
					m_pPlayerPed->SetArmedWeapon(byteCurWeapon, false);
				}
			}

			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}
		// ---- PASSENGER NETWORK PROCESSING ----
		else if (GetState() == PLAYER_STATE_PASSENGER &&
			m_byteUpdateFromNetwork == UPDATE_TYPE_PASSENGER)
		{
			if (!m_pCurrentVehicle || !m_pCurrentVehicle->VerifyInstance()) {
				return;
			}

			uint8_t byteCurrentWeapon = m_psSync.byteCurrentWeapon & 0x3F;
			if(m_pPlayerPed->IsAdded() && m_pPlayerPed->GetCurrentWeapon() != byteCurrentWeapon)
			{
				m_pPlayerPed->SetArmedWeapon(byteCurrentWeapon, 0);
				if(m_pPlayerPed->GetCurrentWeapon() != byteCurrentWeapon)
				{
					m_pPlayerPed->GiveWeapon(byteCurrentWeapon, 9999);
					m_pPlayerPed->SetArmedWeapon(byteCurrentWeapon, 0);
				}
			}

			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}

		// ------ PROCESSED FOR ALL FRAMES ----- 
		if (GetState() == PLAYER_STATE_ONFOOT && !m_pPlayerPed->IsInVehicle())
		{
			InterpolateAndRotate();
			//SyncHead();
			m_bPassengerDriveByMode = false;
			ProcessSpecialActions(m_ofSync.byteSpecialAction);

            if (m_byteWeaponShotID != 0xFF)
            {
                //weapon sync
                //RwMatrix localMat;
                //pPool->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&localMat);

                m_ofSync.byteCurrentWeapon = m_byteWeaponShotID;
                m_pPlayerPed->SetCurrentWeapon(m_byteWeaponShotID);
                //ScriptCommand(&task_shoot_at_coord, m_pPlayerPed->m_dwGTAId, localMat.pos.x, localMat.pos.y, localMat.pos.z, 10);
                m_pPlayerPed->SetCurrentAim(pGame->FindPlayerPed()->GetCurrentAim());
                m_pPlayerPed->SetKeys(m_ofSync.lrAnalog, m_ofSync.udAnalog, (uint16_t)4);

                //unknown weapon
                m_byteWeaponShotID = 0xFF;
            }
            else
                m_pPlayerPed->SetKeys(m_ofSync.lrAnalog, m_ofSync.udAnalog, m_ofSync.wKeys);

			if (m_ofSync.vecMoveSpeed.x == 0.0f &&
				m_ofSync.vecMoveSpeed.y == 0.0f &&
				m_ofSync.vecMoveSpeed.z == 0.0f)
			{
				m_pPlayerPed->SetMoveSpeedVector(m_ofSync.vecMoveSpeed);
			}

			if ((GetTickCount() - m_dwLastRecvTick) > 1500) {
				m_bIsAFK = true;
			}

			if (m_bIsAFK && ((GetTickCount() - m_dwLastRecvTick) > 3000))
			{
				m_ofSync.lrAnalog = 0;
				m_ofSync.udAnalog = 0;

				vecMoveSpeed.x = 0.0f;
				vecMoveSpeed.y = 0.0f;
				vecMoveSpeed.z = 0.0f;
				m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);

				m_pPlayerPed->GetMatrix(&matPlayer);
				matPlayer.pos.x = m_ofSync.vecPos.x;
				matPlayer.pos.y = m_ofSync.vecPos.y;
				matPlayer.pos.z = m_ofSync.vecPos.z;
				m_pPlayerPed->SetMatrix(matPlayer);
			}

			if (SurfingOnObject() || SurfingOnVehicle())
			{
				ProcessSurfing();
				m_pPlayerPed->SetGravityProcessing(false);
				m_pPlayerPed->SetCollisionChecking(false);
			}
			else
			{
				if(m_byteSpecialAction == SPECIAL_ACTION_ENTER_VEHICLE)
				{
					m_pPlayerPed->SetGravityProcessing(0);
					m_pPlayerPed->SetCollisionChecking(0);
				}
				else
				{
					m_pPlayerPed->SetGravityProcessing(1);
					m_pPlayerPed->SetCollisionChecking(1);
				}

			}
		}
		else if (GetState() == PLAYER_STATE_DRIVER && m_pPlayerPed->IsInVehicle())
		{
			if (!m_pCurrentVehicle)
			{
				return;
			}
			if (!GamePool_Vehicle_GetAt(m_pCurrentVehicle->m_dwGTAId))
			{
				return;
			}

			m_bPassengerDriveByMode = false;

			if (m_pCurrentVehicle &&
				m_pCurrentVehicle->GetModelIndex() != 538 &&
				m_pCurrentVehicle->GetModelIndex() != 537 &&
				m_pCurrentVehicle->GetModelIndex() != 449)
			{
				UpdateVehicleRotation();
			}

			m_pPlayerPed->CheckVehicleParachute();

			if (m_pCurrentVehicle && m_pCurrentVehicle->IsRCVehicle()) {
				m_pPlayerPed->Remove();
			}

			if (m_bIsNPC)
			{
				if (m_icSync.vecMoveSpeed.x == 0.0f &&
					m_icSync.vecMoveSpeed.y == 0.0f &&
					m_icSync.vecMoveSpeed.z == 0.0f)
				{
					if (m_pCurrentVehicle)
					{
						m_pCurrentVehicle->SetMoveSpeedVector(m_icSync.vecMoveSpeed);
					}
				}
			}

			m_pPlayerPed->SetKeys(m_icSync.lrAnalog, m_icSync.udAnalog, m_icSync.wKeys);
			m_pPlayerPed->ProcessVehicleHorn();

			if (m_icSync.vecMoveSpeed.x != 0.0f ||
				m_icSync.vecMoveSpeed.y != 0.0f ||
				m_icSync.vecMoveSpeed.z != 0.0f)
			{
				if (!m_bIsAFK)
				{
					if ((GetTickCount() - m_dwLastRecvTick) > 1500)
						m_bIsAFK = true;
				}
			}
			else if (!m_bIsAFK)
			{
				if ((GetTickCount() - m_dwLastRecvTick) > 3000)
					m_bIsAFK = true;
			}
		}
		else if (GetState() == PLAYER_STATE_PASSENGER && m_pPlayerPed->IsInVehicle())
		{
			if((GetTickCount() - m_dwLastRecvTick) > 3000)
				m_bIsAFK = true;

			m_pPlayerPed->SetKeys(m_psSync.wKeys,0,0);
		}
		else
		{
			m_pPlayerPed->SetKeys(0, 0, 0);
			vecMoveSpeed.x = 0.0f;
			vecMoveSpeed.y = 0.0f;
			vecMoveSpeed.z = 0.0f;
			m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
			m_bPassengerDriveByMode = false;
		}

		if (GetState() != PLAYER_STATE_WASTED) {
			m_pPlayerPed->SetHealth(1000.0f);
		}

		if ((GetTickCount() - m_dwLastRecvTick) < 3000) {
			m_bIsAFK = false;
		}
	}
	else
	{
		if (m_pPlayerPed) {
			ResetAllSyncAttributes();
			pGame->RemovePlayer(m_pPlayerPed);
			m_pPlayerPed = nullptr;
		}
	}
}
// 0.3.7
void CRemotePlayer::InterpolateAndRotate()
{
	if(m_pPlayerPed)
	{
		RwMatrix matPlayer;
		m_pPlayerPed->GetMatrix(&matPlayer);

		CQuaternion quatPlayer;
		quatPlayer.SetFromMatrix(&matPlayer);

		CQuaternion quatResult;
		quatResult.Slerp(&m_ofSync.quat, &quatPlayer, 0.75f);
		quatResult.GetMatrix(&matPlayer);

		m_pPlayerPed->SetMatrix(matPlayer);

		float fZ = atan2(-matPlayer.up.x, matPlayer.up.y) * 57.295776f; /* rad to deg */
		if(fZ > 360.0f) fZ -= 360.0f;
		else if(fZ < 0.0f) fZ += 360.0f;

		m_pPlayerPed->SetRotation(fZ);
	}
}
// 0.3.7
void CRemotePlayer::UpdateOnFootTargetPosition()
{
	if(!m_pPlayerPed) return;

	RwMatrix matPlayer;
	m_pPlayerPed->GetMatrix(&matPlayer);

	if(m_pPlayerPed->IsAdded())
	{
		m_vecPosOffset.x = FloatOffset(m_vecOnFootTargetPos.x, matPlayer.pos.x);
		m_vecPosOffset.y = FloatOffset(m_vecOnFootTargetPos.y, matPlayer.pos.y);
		m_vecPosOffset.z = FloatOffset(m_vecOnFootTargetPos.z, matPlayer.pos.z);

		if(m_vecPosOffset.x > 0.00001f || m_vecPosOffset.y > 0.00001f || m_vecPosOffset.z > 0.00001f)
		{
			if(m_vecPosOffset.x > 2.0f || m_vecPosOffset.y > 2.0f || m_vecPosOffset.z > 1.0f)
			{
				matPlayer.pos.x = m_vecOnFootTargetPos.x;
				matPlayer.pos.y = m_vecOnFootTargetPos.y;
				matPlayer.pos.z = m_vecOnFootTargetPos.z;

				m_pPlayerPed->SetMatrix(matPlayer);
			}
			else
			{
				CVector vecMoveSpeed;
				m_pPlayerPed->GetMoveSpeedVector(&vecMoveSpeed);
				if(m_vecPosOffset.x > 0.00001f)
					vecMoveSpeed.x += (m_vecOnFootTargetPos.x - matPlayer.pos.x) * 0.1f;
				if(m_vecPosOffset.y > 0.00001f)
					vecMoveSpeed.y += (m_vecOnFootTargetPos.y - matPlayer.pos.y) * 0.1f;
				if(m_vecPosOffset.z > 0.00001f)
					vecMoveSpeed.z += (m_vecOnFootTargetPos.z - matPlayer.pos.z) * 0.1f;

				m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
			}
		}
	}
	else
	{
		matPlayer.pos.x = m_vecOnFootTargetPos.x;
		matPlayer.pos.y = m_vecOnFootTargetPos.y;
		matPlayer.pos.z = m_vecOnFootTargetPos.z;

		m_pPlayerPed->SetMatrix(matPlayer);
	}
}
// 0.3.7
void CRemotePlayer::UpdateOnFootPositionAndSpeed(CVector* vecPos, CVector* vecMove)
{
	m_vecOnFootTargetPos.x = vecPos->x;
	m_vecOnFootTargetPos.y = vecPos->y;
	m_vecOnFootTargetPos.z = vecPos->z;

	m_vecOnFootTargetSpeed.x = vecMove->x;
	m_vecOnFootTargetSpeed.y = vecMove->y;
	m_vecOnFootTargetSpeed.z = vecMove->z;

	m_pPlayerPed->SetMoveSpeedVector(m_vecOnFootTargetSpeed);
}
// 0.3.7
bool CRemotePlayer::SurfingOnVehicle()
{
	if(GetState() == PLAYER_STATE_ONFOOT) 
	{
		if(m_ofSync.wSurfID != -1 && m_ofSync.wSurfID < MAX_VEHICLES) // its an vehicle
			return true;
	}
	return false;
}

bool CRemotePlayer::SurfingOnObject()
{
	if(GetState() == PLAYER_STATE_ONFOOT) 
	{
		if(m_ofSync.wSurfID != -1 && m_ofSync.wSurfID >= MAX_VEHICLES &&
			m_ofSync.wSurfID < MAX_VEHICLES + MAX_OBJECTS) // its an object
		{
			return true;
		}
	}
	return false;
}

void CRemotePlayer::ResetAllSyncAttributes()
{
	m_VehicleID = 0;
	m_fReportedHealth = 0.0f;
	m_fReportedArmour = 0.0f;
	m_pCurrentVehicle = nullptr;
	m_byteSeatID = 0;
    m_byteWeaponShotID = 0xFF;
	m_bPassengerDriveByMode = false;
	// field_1ED = 0;
	// field_1F1 = 0;
	// field_1F5 = 0;
	m_byteSpecialAction = SPECIAL_ACTION_NONE;
	m_dwAnimation = 0;
	m_bIsAFK = true;
	memset(&m_ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	memset(&m_icSync, 0, sizeof(INCAR_SYNC_DATA));
	memset(&m_psSync, 0, sizeof(PASSENGER_SYNC_DATA));
	// memset(&field_8E
	// memset(&field_1D5

	if (m_dwMarker)
	{
		pGame->DisableMarker(m_dwMarker);
		m_dwMarker = 0;
	}

	// field_1E9 = 0;

	if (pNetGame) {
		m_dwLastStoredSyncDataTime = GetTickCount();
	}
}
// 0.3.7
void CRemotePlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(m_PlayerID, dwColor);
}

uint32_t CRemotePlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(m_PlayerID);
}

// 0.3.7
void CRemotePlayer::Say(char *szMessage)
{
	char* szPlayerName = pNetGame->GetPlayerPool()->GetPlayerName(m_PlayerID);
	uint32_t dwColor = TranslateColorCodeToRGBA(m_PlayerID);
	if (pUI) pUI->chat()->addChatMessage(Encoding::cp2utf(szMessage), szPlayerName, UI::fixcolor(dwColor));
}
// 0.3.7
bool CRemotePlayer::Spawn(uint8_t byteTeam, int iSkin, CVector *vecPos, float fRotation,
	uint32_t dwColor, uint8_t byteFightingStyle)
{
	if (!pGame->IsGameLoaded()) return false;

	if (m_pPlayerPed)
	{
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}

	CPlayerPed* pPlayerPed = pGame->NewPlayer(iSkin, vecPos->x, vecPos->y, vecPos->z, fRotation, true, m_bIsNPC);
	if (pPlayerPed)
	{
		if (dwColor) {
			SetRadarColor(m_PlayerID, dwColor);
		}

		if (m_dwMarker) {
			pGame->DisableMarker(m_dwMarker);
			m_dwMarker = 0;
		}

		if (pNetGame->m_pNetSet->iShowPlayerMarkers/*bShowNameTags*/) {
			pPlayerPed->ShowMarker(m_PlayerID);
		}

		SetTeam(byteTeam);

		m_pPlayerPed = pPlayerPed;
		m_fReportedHealth = 100.0f;
		pPlayerPed->SetKeys(0, 0, 0);
		if (byteFightingStyle != 4) {
			pPlayerPed->SetFightingStyle(byteFightingStyle);
		}

		SetState(PLAYER_STATE_SPAWNED);
		return true;
	}
	else
	{
		SetState(PLAYER_STATE_NONE);
		return false;
	}
}
// 0.3.7
void CRemotePlayer::Remove()
{
	if (m_pPlayerPed)
	{
		ResetAllSyncAttributes();
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}

	SetState(PLAYER_STATE_NONE);
	m_bShowNameTag = true;
}
// 0.3.7
void CRemotePlayer::HandleDeath()
{
	if (m_pPlayerPed)
	{
		m_pPlayerPed->SetKeys(0, 0, 0);
		m_pPlayerPed->SetDead();
	}

	SetState(PLAYER_STATE_WASTED);
	ResetAllSyncAttributes();
}

void CRemotePlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *ofSync, uint32_t dwTime)
{
	if (dwTime == 0 || dwTime - m_dwLastStoredSyncDataTime >= 0)
	{
		m_dwLastStoredSyncDataTime = dwTime;
		memcpy(&m_ofSync, ofSync, sizeof(ONFOOT_SYNC_DATA));
		m_fReportedHealth = ofSync->byteHealth;
		m_fReportedArmour = ofSync->byteArmour;
		m_byteUpdateFromNetwork = UPDATE_TYPE_ONFOOT;
		m_dwLastRecvTick = GetTickCount();
		m_byteSpecialAction = ofSync->byteSpecialAction;

		if (m_pPlayerPed && m_pPlayerPed->IsInVehicle())
		{
			if (m_byteSpecialAction != SPECIAL_ACTION_ENTER_VEHICLE &&
				m_byteSpecialAction != SPECIAL_ACTION_EXIT_VEHICLE )//&&
				//!m_pPlayerPed->sub_100ABDB0())
			{
				RemoveFromVehicle();
			}
		}
	}

	SetState(PLAYER_STATE_ONFOOT);
}
// 0.3.7
void CRemotePlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync, uint32_t dwTime)
{
	if (!dwTime || dwTime - m_dwLastStoredSyncDataTime >= 0) {
		m_dwLastStoredSyncDataTime = dwTime;
		memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));
		m_VehicleID = picSync->VehicleID;

		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if (!pVehiclePool) return;
		if (!pVehiclePool->GetSlotState(m_VehicleID)) return;

		m_pCurrentVehicle = pVehiclePool->GetAt(m_VehicleID);
		if (!m_pCurrentVehicle)return;

		m_byteSeatID = 0;
		m_fReportedHealth = picSync->bytePlayerHealth;
		m_fReportedArmour = picSync->bytePlayerArmour;
		//m_pCurrentVehicle->SetHealth(picSync->fCarHealth);
		m_byteUpdateFromNetwork = UPDATE_TYPE_INCAR;
		m_dwLastRecvTick = GetTickCount();
		m_byteSpecialAction = 0;

		if(m_pPlayerPed && !m_pPlayerPed->IsInVehicle())
			PutInCurrentVehicle();

		SetState(PLAYER_STATE_DRIVER);
	}
}

float DecompressCameraExtZoom(unsigned char v)
{
	return (v & 63) / 63.0f;
}

float DecompressAspectRatio(char v)
{
	return v / 255.0f;
}

// 0.3.7
void CRemotePlayer::StoreAimFullSyncData(AIM_SYNC_DATA* paimSync)
{
	if (!m_pPlayerPed) return;
	m_pPlayerPed->SetCameraMode(paimSync->byteCamMode);

	CAMERA_AIM Aim;
	Aim.f1x = paimSync->vecAimf.x;
	Aim.f1y = paimSync->vecAimf.y;
	Aim.f1z = paimSync->vecAimf.z;

	CVector vec1, vec2;
	vec1.x = Aim.f1x;
	vec1.y = Aim.f1y;
	vec1.z = Aim.f1z;
	vec2.x = 0.0f;
	vec2.y = 0.0f;
	vec2.z = 0.0f;
	calculateAimVector(&vec1, &vec2);
	Aim.f2x = vec2.x;
	Aim.f2y = vec2.y;
	Aim.f2z = vec2.z;

	Aim.pos1x = paimSync->vecAimPos.x;
	Aim.pos1y = paimSync->vecAimPos.y;
	Aim.pos1z = paimSync->vecAimPos.z;
	Aim.pos2x = Aim.pos1x;
	Aim.pos2y = Aim.pos1y;
	Aim.pos2z = Aim.pos1z;
	m_pPlayerPed->SetCurrentAim(&Aim);

	m_pPlayerPed->SetAimZ(paimSync->fAimZ);

	float fExtZoom = DecompressCameraExtZoom(paimSync->byteCamExtZoom);
	float fAspect = DecompressAspectRatio(paimSync->aspect_ratio);

	m_pPlayerPed->SetCameraZoomAndAspect(fExtZoom, fAspect);

	WEAPON_SLOT_TYPE* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
	if (paimSync->byteWeaponState == WS_RELOADING)
		pwstWeapon->dwState = 2;		// Reloading
	else
		if (paimSync->byteWeaponState != WS_MORE_BULLETS)
			pwstWeapon->dwAmmoInClip = (uint32_t)paimSync->byteWeaponState;
		else
			if (pwstWeapon->dwAmmoInClip < 2)
				pwstWeapon->dwAmmoInClip = 2;
}
// 0.3.7
void CRemotePlayer::StorePassengerFullSyncData(PASSENGER_SYNC_DATA* psSync)
{
	memcpy(&m_psSync, psSync, sizeof(PASSENGER_SYNC_DATA));
	m_VehicleID = psSync->VehicleID;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool->GetSlotState(m_VehicleID))
		return;

	CVehicle* pVehicle = pVehiclePool->GetAt(m_VehicleID);

	m_pCurrentVehicle = pVehicle;

	if(!m_pCurrentVehicle)return;

	m_byteSeatID = psSync->byteSeatFlags & 0x3F;
	m_fReportedHealth = psSync->bytePlayerHealth;
	m_fReportedArmour = psSync->bytePlayerArmour;
	m_byteUpdateFromNetwork = UPDATE_TYPE_PASSENGER;
	m_dwLastRecvTick = GetTickCount();
	m_byteSpecialAction = 0;

	if(m_pPlayerPed && !m_pPlayerPed->IsInVehicle())
		PutInCurrentVehicle();

	SetState(PLAYER_STATE_PASSENGER);
}
// 0.3.7
void CRemotePlayer::StoreBulletFullSyncData(BULLET_SYNC_DATA* btSync)
{
	if (!m_pPlayerPed || !m_pPlayerPed->IsAdded()) return;

	BULLET_DATA btData;
	memset(&btData, 0, sizeof(BULLET_DATA));

	btData.vecOrigin.x = btSync->vecOrigin.x;
	btData.vecOrigin.y = btSync->vecOrigin.y;
	btData.vecOrigin.z = btSync->vecOrigin.z;

	btData.vecPos.x = btSync->vecPos.x;
	btData.vecPos.y = btSync->vecPos.y;
	btData.vecPos.z = btSync->vecPos.z;

	btData.vecOffset.x = btSync->vecOffset.x;
	btData.vecOffset.y = btSync->vecOffset.y;
	btData.vecOffset.z = btSync->vecOffset.z;

	if (btSync->byteHitType != BULLET_HIT_TYPE_NONE)
	{
		if (btData.vecOffset.x > 300.0f ||
			btData.vecOffset.x < -300.0f ||
			btData.vecOffset.y > 300.0f ||
			btData.vecOffset.y < -300.0f ||
			btData.vecOffset.z > 300.0f ||
			btData.vecOffset.z < -300.0f)
		{
			return;
		}

        ENTITY_TYPE* pEntity = nullptr;

		if (btSync->byteHitType == BULLET_HIT_TYPE_PLAYER)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				if (btSync->PlayerID == pPlayerPool->GetLocalPlayerID())
				{
					pEntity = &pGame->FindPlayerPed()->m_pPed->entity;
				}
				else if (btSync->PlayerID == m_PlayerID)
				{
					return;
				}
				else if (pPlayerPool->GetSlotState(btSync->PlayerID))
				{
					CPlayerPed* pPlayerPed = pPlayerPool->GetAt(btSync->PlayerID)->GetPlayerPed();
					if (pPlayerPed) {
						pEntity = &pPlayerPed->m_pPed->entity;
					}
				}
			}
		}
		else if (btSync->byteHitType == BULLET_HIT_TYPE_VEHICLE)
		{
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				if (pVehiclePool->GetSlotState(btSync->PlayerID))
				{
					pEntity = &pVehiclePool->GetAt(btSync->PlayerID)->m_pVehicle->entity;
				}
			}
		}

		btData.pEntity = pEntity;
	}

	if (m_pPlayerPed->IsAdded())
	{
		if (m_pPlayerPed->GetCurrentWeapon() != btSync->byteWeaponID)
		{
			m_pPlayerPed->SetArmedWeapon(btSync->byteWeaponID, true);

			if (m_pPlayerPed->GetCurrentWeapon() != btSync->byteWeaponID)
			{
				m_pPlayerPed->GiveWeapon(btSync->byteWeaponID, 9999);
				m_pPlayerPed->SetArmedWeapon(btSync->byteWeaponID, true);
			}
		}
	}

	//m_byteWeaponShotID = btSync->byteWeaponID;

	m_pPlayerPed->ProcessBulletData(&btData);
	m_pPlayerPed->FireInstant();
}
// 0.3.7
void CRemotePlayer::RemoveFromVehicle()
{
	RwMatrix mat;

	if (m_pPlayerPed)
	{
		if (m_pPlayerPed->IsInVehicle())
		{
			m_pPlayerPed->GetMatrix(&mat);
			m_pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.x, mat.pos.y, mat.pos.z+1.0f);
		}
	}
}
// 0.3.7
void CRemotePlayer::UpdateInCarMatrixAndSpeed(RwMatrix* pMatrix, CVector* pVecPos, CVector* pVecMoveSpeed)
{
	m_quat.SetFromMatrix(*&pMatrix);
	m_vecInCarTargetPos.x = pVecPos->x;
	m_vecInCarTargetPos.y = pVecPos->y;
	m_vecInCarTargetPos.z = pVecPos->z;

	m_vecInCarTargetSpeed.x = pVecMoveSpeed->x;
	m_vecInCarTargetSpeed.y = pVecMoveSpeed->y;
	m_vecInCarTargetSpeed.z = pVecMoveSpeed->z;

	m_pCurrentVehicle->SetMoveSpeedVector(m_vecInCarTargetSpeed);
}
// 0.3.7
void CRemotePlayer::UpdateInCarTargetPosition()
{
	if (!m_pCurrentVehicle) return;

	RwMatrix matEnt;
	m_pCurrentVehicle->GetMatrix(&matEnt);

	if (m_pCurrentVehicle->IsAdded())
	{
		m_vecPositionInaccuracy.x = FloatOffset(m_vecInCarTargetPos.x, matEnt.pos.x);
		m_vecPositionInaccuracy.y = FloatOffset(m_vecInCarTargetPos.y, matEnt.pos.y);
		m_vecPositionInaccuracy.z = FloatOffset(m_vecInCarTargetPos.z, matEnt.pos.z);

		if (m_vecPositionInaccuracy.x > 0.05f ||
			m_vecPositionInaccuracy.y > 0.05f ||
			m_vecPositionInaccuracy.z > 0.05f)
		{
			float fTestZInaccuracy = 0.5f;
			if (m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_HELI)
			{
				fTestZInaccuracy = 2.0f;
			}

			if (m_vecPositionInaccuracy.x > 8.0f ||
				m_vecPositionInaccuracy.y > 8.0f ||
				m_vecPositionInaccuracy.z > fTestZInaccuracy)
			{
				matEnt.pos.x = m_vecInCarTargetPos.x;
				matEnt.pos.y = m_vecInCarTargetPos.y;
				matEnt.pos.z = m_vecInCarTargetPos.z;
				m_pCurrentVehicle->SetMatrix(matEnt);
				m_pCurrentVehicle->SetMoveSpeedVector(m_vecInCarTargetSpeed);
			}
			else
			{
				CVector vecMoveSpeed;
				m_pCurrentVehicle->GetMoveSpeedVector(&vecMoveSpeed);

				if (vecMoveSpeed.x > 0.05f) {
					vecMoveSpeed.x += (m_vecInCarTargetPos.x - matEnt.pos.x) * 0.05f;
				}

				if (vecMoveSpeed.y > 0.05f) {
					vecMoveSpeed.y += (m_vecInCarTargetPos.y - matEnt.pos.y) * 0.05f;
				}

				if (vecMoveSpeed.z > 0.05f) {
					vecMoveSpeed.z += (m_vecInCarTargetPos.z - matEnt.pos.z) * 0.05f;
				}

				m_pCurrentVehicle->SetMoveSpeedVector(vecMoveSpeed);
			}
		}
	}
	else
	{
		matEnt.pos.x = m_vecInCarTargetPos.x;
		matEnt.pos.y = m_vecInCarTargetPos.y;
		matEnt.pos.z = m_vecInCarTargetPos.z;
		m_pCurrentVehicle->SetMatrix(matEnt);
	}
}
// 0.3.7
void CRemotePlayer::UpdateVehicleRotation()
{
	CQuaternion quat, qresult;
	RwMatrix matEnt;
	CVector vec = { 0.0f, 0.0f, 0.0f };


	if (!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetTurnSpeedVector(&vec);
	if (vec.x <= 0.02f)
	{
		if (vec.x < -0.02f) vec.x = -0.02f;
	}
	else vec.x = 0.02f;

	if (vec.y <= 0.02f)
	{
		if (vec.y < -0.02f) vec.y = -0.02f;
	}
	else vec.y = 0.02f;

	if (vec.z <= 0.02f)
	{
		if (vec.z < -0.02f) vec.z = -0.02f;
	}
	else vec.z = 0.02f;

	m_pCurrentVehicle->SetTurnSpeedVector(vec);

	m_pCurrentVehicle->GetMatrix(&matEnt);
	quat.SetFromMatrix(&matEnt);
	qresult.Slerp(&m_quat, &quat, 0.75f);
	qresult.Normalize();
	qresult.GetMatrix(&matEnt);
	m_pCurrentVehicle->SetMatrix(matEnt);
}
// 0.3.7
void CRemotePlayer::PutInCurrentVehicle()
{
	FLog("PutInCurrentVehicle");

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if (m_pPlayerPed && !m_pPlayerPed->IsInVehicle())
	{
		CVehicle* pVehicle = pVehiclePool->GetAt(m_VehicleID);

		if (pVehicle)
		{
			uint8_t byteCurWeapon;
			if (m_byteSeatID != 0) {
				byteCurWeapon = m_psSync.byteCurrentWeapon;
			}
			else {
				byteCurWeapon = m_icSync.byteCurrentWeapon;
			}

			byteCurWeapon = byteCurWeapon & 0x3F;
			if (m_pPlayerPed->GetCurrentWeapon() != byteCurWeapon)
			{
				m_pPlayerPed->SetArmedWeapon(byteCurWeapon, false);
				if (m_pPlayerPed->GetCurrentWeapon() != byteCurWeapon)
				{
					m_pPlayerPed->GiveWeapon(byteCurWeapon, 9999);
					m_pPlayerPed->SetArmedWeapon(byteCurWeapon, false);
				}
			}

			uint32_t dwGTAId = pVehiclePool->FindGtaIDFromID(m_VehicleID);
			m_pPlayerPed->PutDirectlyInVehicle(dwGTAId, m_byteSeatID);
		}
	}
}
// 0.3.7
float CRemotePlayer::GetDistanceFromLocalPlayer()
{
	if (!m_pPlayerPed) return 10000.0f;

	if (GetState() == PLAYER_STATE_DRIVER && m_pCurrentVehicle && m_pPlayerPed->IsInVehicle()) {
		return m_pCurrentVehicle->GetDistanceFromLocalPlayerPed();
	}
	else {
		return m_pPlayerPed->GetDistanceFromLocalPlayerPed();
	}
}
// 0.3.7
void CRemotePlayer::EnterVehicle(VEHICLEID VehicleID, bool bPassenger)
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);

	if (m_pPlayerPed && pVehicle)
	{
		if (!m_pPlayerPed->IsInVehicle())
		{
			uint32_t dwGTAId = pVehiclePool->FindGtaIDFromID(VehicleID);
			if (dwGTAId && dwGTAId != INVALID_VEHICLE_ID) {
				m_pPlayerPed->SetKeys(0, 0, 0);
				m_pPlayerPed->EnterVehicle(dwGTAId, bPassenger);
			}
		}
	}
}
// 0.3.7
void CRemotePlayer::ExitVehicle()
{
	if (m_pPlayerPed && m_pPlayerPed->IsInVehicle())
	{
		m_pPlayerPed->SetKeys(0, 0, 0);
		m_pPlayerPed->ExitCurrentVehicle();
	}
}

// 0.3.7
void CRemotePlayer::ShowGlobalMarker(short sPosX, short sPosY, short sPosZ)
{
	m_bGlobalMarkerShown = true;

	if (m_dwMarker)
	{
		pGame->DisableMarker(m_dwMarker);
		m_dwMarker = 0;
	}

	if (m_pPlayerPed)
	{
		uint32_t dwMarker = pGame->CreateRadarMarkerIcon(0, sPosX, sPosY, sPosZ, m_PlayerID, 0);
		m_vecGlobalMarkerPos.x = sPosX;
		m_vecGlobalMarkerPos.y = sPosY;
		m_vecGlobalMarkerPos.z = sPosZ;
		m_dwMarker = dwMarker;
	}
}
// 0.3.7
void CRemotePlayer::HideGlobalMarker()
{
	if (m_dwMarker)
	{
		pGame->DisableMarker(m_dwMarker);
		m_dwMarker = 0;
	}

	m_bGlobalMarkerShown = false;
}

// 0.3.7
void CRemotePlayer::StateChange(uint8_t byteNewState, uint8_t byteOldState)
{
	if (byteNewState == PLAYER_STATE_DRIVER && byteOldState == PLAYER_STATE_ONFOOT)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

		if (pPlayerPed)
		{
			if (pPlayerPed->IsInVehicle() && !pPlayerPed->IsAPassenger())
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				VEHICLE_TYPE* pGtaVehicle = pPlayerPed->GetGtaVehicle();

				if (pVehiclePool->FindIDFromGtaPtr(pGtaVehicle) == m_VehicleID)
				{
					RwMatrix mat;
					pPlayerPed->GetMatrix(&mat);
					pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.x, mat.pos.y, mat.pos.z + 1.0f);
					pGame->DisplayGameText("~r~Car Jacked~w~!", 1000, 5);
				}
			}
		}
	}
}

/*
bool CRemotePlayer::IsVoiceActive()
{
	return pVoice->IsPlayerVoiceActive(m_PlayerID);
} 	
*/

void CRemotePlayer::ProcessSurfing()
{
	if(!m_pPlayerPed || GetState() != PLAYER_STATE_ONFOOT || m_ofSync.wSurfID == INVALID_VEHICLE_ID)
		return;

	CVehicle *pVehicleSurfing = 0;
	CObject* pObjectSurfing = 0;
	
	if(SurfingOnVehicle())
	{
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if(pVehiclePool)
		{
			CVehicle *pVehicle = pVehiclePool->GetAt(m_ofSync.wSurfID);
			if(pVehicle) 
				pVehicleSurfing = pVehicle;
		}
	}
	else if(SurfingOnObject())
	{
		m_ofSync.wSurfID -= MAX_VEHICLES; // derive proper object id
		CObjectPool *pObjectPool = pNetGame->GetObjectPool();
		if(pObjectPool)
		{
			CObject *pObject = pObjectPool->GetAt((uint16_t)m_ofSync.wSurfID);
			if(pObject)
				pObjectSurfing = pObject;
		}
	}

	if(pVehicleSurfing )
	{
		RwMatrix matEntity, matPlayer;
		CVector vecMoveSpeed, vecTurnSpeed;

		pVehicleSurfing->GetMatrix(&matEntity);

		/* unused ? */
		pVehicleSurfing->GetMoveSpeedVector(&vecMoveSpeed);
		pVehicleSurfing->GetTurnSpeedVector(&vecTurnSpeed);
		/* unused ? */

		m_pPlayerPed->GetMatrix(&matPlayer);

		ProjectMatrix(&matPlayer.pos, &matEntity, &m_ofSync.vecSurfOffsets);

		/*matPlayer.pos.x = matEntity.pos.x + m_ofSync.vecSurfOffsets.x;
		matPlayer.pos.y = matEntity.pos.y + m_ofSync.vecSurfOffsets.y;
		matPlayer.pos.z = matEntity.pos.z + m_ofSync.vecSurfOffsets.z;*/

		m_pPlayerPed->UpdateMatrix(matPlayer);
		//m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
		//m_pPlayerPed->SetTurnSpeedVector(vecTurnSpeed);
	}
	else if(pObjectSurfing)
	{
		RwMatrix matEntity, matPlayer;
		CVector vecMoveSpeed, vecTurnSpeed;

		pObjectSurfing->GetMatrix(&matEntity);

		/* unused ? */
		pObjectSurfing->GetMoveSpeedVector(&vecMoveSpeed);
		pObjectSurfing->GetTurnSpeedVector(&vecTurnSpeed);
		/* unused ? */

		m_pPlayerPed->GetMatrix(&matPlayer);

		ProjectMatrix(&matPlayer.pos, &matEntity, &m_ofSync.vecSurfOffsets);

		/*matPlayer.pos.x = matEntity.pos.x + m_ofSync.vecSurfOffsets.x;
		matPlayer.pos.y = matEntity.pos.y + m_ofSync.vecSurfOffsets.y;
		matPlayer.pos.z = matEntity.pos.z + m_ofSync.vecSurfOffsets.z;*/

		m_pPlayerPed->UpdateMatrix(matPlayer);
		//m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
		//m_pPlayerPed->SetTurnSpeedVector(vecTurnSpeed);
	}
}

void CRemotePlayer::StoreTrailerFullSyncData(TRAILER_SYNC_DATA *trSync)
{
	CVector 	m_vecPosOffset;

	VEHICLEID trailerId = m_icSync.TrailerID;
	if(trailerId < 0 || trailerId >= MAX_VEHICLES) 
		return;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool) 
	{
		CVehicle *pTrailer = pVehiclePool->GetAt(trailerId);
		if(pTrailer && (pTrailer->IsATrailer() || pTrailer->IsATowTruck()))
		{
			if(m_pCurrentVehicle->GetTrailer() != pTrailer)
			{
				m_pCurrentVehicle->SetTrailer(pTrailer);
				m_pCurrentVehicle->AttachTrailer();
			}

			RwMatrix matTrailer;
			pTrailer->GetMatrix(&matTrailer);
			trSync->quat.GetMatrix(&matTrailer);

			m_vecPosOffset.x = FloatOffset(trSync->vecPos.x, matTrailer.pos.x);
			m_vecPosOffset.y = FloatOffset(trSync->vecPos.y, matTrailer.pos.y);
			m_vecPosOffset.z = FloatOffset(trSync->vecPos.z, matTrailer.pos.z);

			if(m_vecPosOffset.x > 0.5f || m_vecPosOffset.y > 0.5f || m_vecPosOffset.z > 0.5f)
			{
				if(m_vecPosOffset.x > 6.0f || m_vecPosOffset.y > 6.0f || m_vecPosOffset.z > 3.0f)
				{
					matTrailer.pos.x = trSync->vecPos.x;
					matTrailer.pos.y = trSync->vecPos.y;
					matTrailer.pos.z = trSync->vecPos.z;

					pTrailer->SetMatrix(matTrailer);
					pTrailer->SetMoveSpeedVector(trSync->vecMoveSpeed);
					pTrailer->SetTurnSpeedVector(trSync->vecTurnSpeed);
				}
				else
				{
					pTrailer->SetMatrix(matTrailer);
					pTrailer->SetTurnSpeedVector(trSync->vecTurnSpeed);

					CVector vecMoveSpeed;
					pTrailer->GetMoveSpeedVector(&vecMoveSpeed);

					if(m_vecPosOffset.x > 0.050000001f)
						vecMoveSpeed.x += (trSync->vecPos.x - matTrailer.pos.x) * 0.025f;
					if(m_vecPosOffset.y > 0.050000001f)
						vecMoveSpeed.y += (trSync->vecPos.y - matTrailer.pos.y) * 0.025f;
					if(m_vecPosOffset.z > 0.050000001f)
						vecMoveSpeed.z += (trSync->vecPos.z - matTrailer.pos.z) * 0.025f;

					pTrailer->SetMoveSpeedVector(vecMoveSpeed);
				}
			}
		}
	}
}

void DecompressNormalVector(RwV3d *vecOut, RwV3d vecIn)
{
	vecOut->x = (float)(vecIn.x / 10000.0);
	vecOut->y = (float)(vecIn.y / 10000.0);
	vecOut->z = (float)(vecIn.z / 10000.0);
}

void CRemotePlayer::StoreUnoccupiedSyncData(UNOCCUPIED_SYNC_DATA *unocSync)
{
	VEHICLEID UnocID = unocSync->vehicleId;
	if (!UnocID || UnocID == INVALID_VEHICLE_ID) return;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle *pVehicle = NULL;
	if (pVehiclePool) {
		pVehicle = pVehiclePool->GetAt(UnocID);
		//pVehiclePool->SetLastUndrivenID(UnocID, m_PlayerID);
	}

	if(pVehicle && !pVehicle->HasADriver())
	{
		RwMatrix matWorld;

		pVehicle->GetMatrix(&matWorld);

		DecompressNormalVector(&matWorld.up, unocSync->vecDirection);
		DecompressNormalVector(&matWorld.right, unocSync->vecRoll);

		// if we're pretty already there.. no translation.
		if( (FloatOffset(unocSync->vecPos.x,matWorld.pos.x) <= 0.1f) &&
			(FloatOffset(unocSync->vecPos.y,matWorld.pos.y) <= 0.1f) &&
			(FloatOffset(unocSync->vecPos.z,matWorld.pos.z) <= 0.1f) )
		{
			return;
		}

		// if difference is over 8 units, direct translation
		if( !pVehicle->IsAdded() ||
			(FloatOffset(unocSync->vecPos.x,matWorld.pos.x) > 8.0f) ||
			(FloatOffset(unocSync->vecPos.y,matWorld.pos.y) > 8.0f) ||
			(FloatOffset(unocSync->vecPos.z,matWorld.pos.z) > 8.0f) ) {

			matWorld.pos.x = unocSync->vecPos.x;
			matWorld.pos.y = unocSync->vecPos.y;
			matWorld.pos.z = unocSync->vecPos.z;

			pVehicle->SetMatrix(matWorld);
			pVehicle->SetMoveSpeedVector(unocSync->vecMoveSpeed);
			pVehicle->SetTurnSpeedVector(unocSync->vecTurnSpeed);
			return;
		}

		// gradually increase/decrease velocities towards the target
		pVehicle->SetMatrix(matWorld);							// rotation
		pVehicle->SetMoveSpeedVector(unocSync->vecMoveSpeed);	// move velocity
		pVehicle->SetTurnSpeedVector(unocSync->vecTurnSpeed);	// turn velocity

		CVector vec = {0.0f, 0.0f, 0.0f};
		pVehicle->GetMoveSpeedVector(&vec);

		if( FloatOffset(unocSync->vecPos.x,matWorld.pos.x) > 0.05 ) {
			vec.x += (unocSync->vecPos.x - matWorld.pos.x) * 0.05f;
		}
		if( FloatOffset(unocSync->vecPos.y,matWorld.pos.y) > 0.05 ) {
			vec.y += (unocSync->vecPos.y - matWorld.pos.y) * 0.05f;
		}
		if( FloatOffset(unocSync->vecPos.z,matWorld.pos.z) > 0.05 ) {
			vec.z += (unocSync->vecPos.z - matWorld.pos.z) * 0.05f;
		}

		pVehicle->SetMoveSpeedVector(vec);
		//pVehicle->m_bRemoteUnocSync = true;
	}
	/*VEHICLEID vehicleId = unocSync->vehicleId;
	if(vehicleId < 0 || vehicleId >= MAX_VEHICLES)
		return;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool)
	{
		CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
		if(pVehicle)
		{
			RwMatrix matVehicle;
			pVehicle->GetMatrix(&matVehicle);

			DecompressNormalVector(&matVehicle.right, unocSync->vecRoll);
			DecompressNormalVector(&matVehicle.up, unocSync->vecDirection);

			//unocSync->quat.GetAsMatrix(&matVehicle);

			if(pVehicle->IsAdded() && !pVehicle->HasADriver() && pVehicle->GetTractor() == NULL)
			{
				m_vecPosOffset.x = FloatOffset(unocSync->vecPos.x, matVehicle.pos.x);
				m_vecPosOffset.y = FloatOffset(unocSync->vecPos.y, matVehicle.pos.y);
				m_vecPosOffset.z = FloatOffset(unocSync->vecPos.z, matVehicle.pos.z);

				if(m_vecPosOffset.x > 0.1f && m_vecPosOffset.y > 0.1f && m_vecPosOffset.z > 0.1f)
				{
					if(m_vecPosOffset.x > 6.0f || m_vecPosOffset.y > 6.0f || m_vecPosOffset.z > 3.0f)
					{
						matVehicle.pos.x = unocSync->vecPos.x;
						matVehicle.pos.y = unocSync->vecPos.y;
						matVehicle.pos.z = unocSync->vecPos.z;

						pVehicle->SetMatrix(matVehicle);
						pVehicle->SetMoveSpeedVector(unocSync->vecMoveSpeed);
						pVehicle->SetTurnSpeedVector(unocSync->vecTurnSpeed);
					}
					else
					{
						pVehicle->SetMatrix(matVehicle);
						pVehicle->SetMoveSpeedVector(unocSync->vecMoveSpeed);
						pVehicle->SetTurnSpeedVector(unocSync->vecTurnSpeed);

						CVector vecMoveSpeed;
						pVehicle->GetMoveSpeedVector(&vecMoveSpeed);

						if(m_vecPosOffset.x > 0.05)
							vecMoveSpeed.x += (unocSync->vecPos.x - matVehicle.pos.x) * 0.05f;
						if(m_vecPosOffset.y > 0.05)
							vecMoveSpeed.y += (unocSync->vecPos.y - matVehicle.pos.y) * 0.05f;
						if(m_vecPosOffset.z > 0.05)
							vecMoveSpeed.z += (unocSync->vecPos.z - matVehicle.pos.z) * 0.05f;

						pVehicle->SetMoveSpeedVector(vecMoveSpeed);
					}
				}
			}
			else
			{
				matVehicle.pos.x = unocSync->vecPos.x;
				matVehicle.pos.y = unocSync->vecPos.y;
				matVehicle.pos.z = unocSync->vecPos.z;

				pVehicle->SetMatrix(matVehicle);
			}
		}
	}*/
}

void CRemotePlayer::ProcessSpecialActions(uint8_t byteSpecialAction)
{
	if(!m_pPlayerPed || !m_pPlayerPed->IsAdded()) return;

	if(GetState() != PLAYER_STATE_ONFOOT) 
	{
		byteSpecialAction = SPECIAL_ACTION_NONE;
		m_ofSync.byteSpecialAction = SPECIAL_ACTION_NONE;
	}

	// duck:start
	if(byteSpecialAction == SPECIAL_ACTION_DUCK && !m_pPlayerPed->IsCrouching())
	{
		if (!m_pPlayerPed->IsCrouching()) {
			m_pPlayerPed->ApplyCrouch();
		}
		return;
	}

	// duck:stop
	if(byteSpecialAction != SPECIAL_ACTION_DUCK && m_pPlayerPed->IsCrouching()) 
	{
		if (m_pPlayerPed->IsCrouching())
		{
			m_pPlayerPed->ResetCrouch();
		}
		return;
	}

	// cellphone:start
	if(byteSpecialAction == SPECIAL_ACTION_USECELLPHONE && !m_pPlayerPed->IsCellphoneEnabled()) 
	{
		m_pPlayerPed->ToggleCellphone(1);
		return;
	}

	// cellphone:stop
	if(byteSpecialAction != SPECIAL_ACTION_USECELLPHONE && m_pPlayerPed->IsCellphoneEnabled()) 
	{
		m_pPlayerPed->ToggleCellphone(0);
		return;
	}

	// jetpack:start
	if(byteSpecialAction == SPECIAL_ACTION_USEJETPACK && !m_pPlayerPed->IsInJetpackMode()) 
	{
		m_pPlayerPed->StartJetpack();
		return;
	}

	// jetpack:stop
	if(byteSpecialAction != SPECIAL_ACTION_USEJETPACK && m_pPlayerPed->IsInJetpackMode()) 
	{
		m_pPlayerPed->StopJetpack();
		return;
	}

	if(byteSpecialAction == SPECIAL_ACTION_CARRY && !m_pPlayerPed->IsCarry())
	{
		m_pPlayerPed->m_iCarryState = 1;
		m_pPlayerPed->ApplyAnimation("CRRY_PRTIAL", "CARRY", 4.1, 0, 0, 0, 1, 1);
		return;
	}

	if(byteSpecialAction != SPECIAL_ACTION_CARRY && m_pPlayerPed->IsCarry())
	{
		m_pPlayerPed->ApplyAnimation("crry_prtial", "CARRY", 4.00, false, false, false, false, 1);
		m_pPlayerPed->m_iCarryState = 0;
		return;
	}

	if(byteSpecialAction == SPECIAL_ACTION_CUFFED && !m_pPlayerPed->IsCuffed())
	{
		m_pPlayerPed->m_iCuffedState = 1;
		return;
	}

	if(byteSpecialAction != SPECIAL_ACTION_CUFFED && m_pPlayerPed->IsCuffed())
	{
		m_pPlayerPed->m_iCuffedState = 0;
		return;
	}

	// handsup:start
	if(byteSpecialAction == SPECIAL_ACTION_HANDSUP && !m_pPlayerPed->HasHandsUp())
		m_pPlayerPed->HandsUp();

	// handsup:stop
	if(byteSpecialAction != SPECIAL_ACTION_HANDSUP && m_pPlayerPed->HasHandsUp())
		m_pPlayerPed->TogglePlayerControllable(true);

	// urinating:start
	if(byteSpecialAction == SPECIAL_ACTION_PISSING && !m_pPlayerPed->IsPissing())
		m_pPlayerPed->StartPissing();

	// urinating:start
	if(byteSpecialAction != SPECIAL_ACTION_PISSING && m_pPlayerPed->IsPissing())
		m_pPlayerPed->StopPissing();

	// dancing:start
	if(!m_pPlayerPed->GetDanceStyle() == -1)
	{
		if(byteSpecialAction == SPECIAL_ACTION_DANCE1)
			m_pPlayerPed->StartDancing(0);
		if(byteSpecialAction == SPECIAL_ACTION_DANCE2)
			m_pPlayerPed->StartDancing(1);
		if(byteSpecialAction == SPECIAL_ACTION_DANCE3)
			m_pPlayerPed->StartDancing(2);
		if(byteSpecialAction == SPECIAL_ACTION_DANCE4)
			m_pPlayerPed->StartDancing(3);
	}

	// dancing:stop
	if(m_pPlayerPed->GetDanceStyle() != -1)
	{
		if(byteSpecialAction != SPECIAL_ACTION_DANCE1 && 
			byteSpecialAction != SPECIAL_ACTION_DANCE2 && 
			byteSpecialAction != SPECIAL_ACTION_DANCE3 && 
			byteSpecialAction != SPECIAL_ACTION_DANCE4)
		{
			m_pPlayerPed->StopDancing();
		}
	}

	// stuff:start
	if(m_pPlayerPed->GetStuff() == eStuffType::STUFF_TYPE_NONE)
	{
		if(byteSpecialAction == SPECIAL_ACTION_DRINK_BEER)
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_BEER);
		
		if(byteSpecialAction == SPECIAL_ACTION_SMOKE_CIGGY)
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_CIGGI);
		
		if(byteSpecialAction == SPECIAL_ACTION_DRINK_WINE)
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_DYN_BEER);
		
		if(byteSpecialAction == SPECIAL_ACTION_DRINK_SPRUNK)
			m_pPlayerPed->GiveStuff(eStuffType::STUFF_TYPE_PINT_GLASS);
	}

	// stuff:process and stop
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

	// parachutes:we don't have any network indicators for this yet
	//m_pPlayerPed->ProcessParachutes();
}

void CRemotePlayer::ProcessAnimation()
{
	if(m_pPlayerPed)
  	{
		ANIMATION_DATA animationData;
		animationData.iValue = m_ofSync.dwAnimation;
		int flag = m_ofSync.dwAnimation >> 31;
		if(!flag) {
			if(animationData.sId < 0)
			{
				if(m_bWasAnimSettedFlag)
				{
					m_dwPlayingAnimIndex = 0;
					m_pPlayerPed->PlayAnimationFromIndex(0, animationData.cFrameDelta);
					m_bWasAnimSettedFlag = false;
				}
			}
			else
			{
				if(m_dwPlayingAnimIndex != animationData.sId)
				{
					m_dwPlayingAnimIndex = animationData.sId;
					m_pPlayerPed->PlayAnimationFromIndex(animationData.sId, animationData.cFrameDelta);
				}

				if(m_dwPlayingAnimIndex)
				{
					if(!m_pPlayerPed->IsPlayingAnimation(m_dwPlayingAnimIndex))
						m_pPlayerPed->PlayAnimationFromIndex(animationData.sId, animationData.cFrameDelta);
				}

				m_bWasAnimSettedFlag = true;
			}
		}
		if (flag && m_bWasAnimSettedFlag)
		{
			m_dwPlayingAnimIndex = 0;
			// we have to reset anim
			m_pPlayerPed->PlayAnimationFromIndex(0, 4.0f);
			m_bWasAnimSettedFlag = false;
		}
	}
}

void CRemotePlayer::HandleVehicleEntryExit()
{
	if(!m_pPlayerPed) return;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool)
	{
		if(!m_pPlayerPed->IsInVehicle())
		{
			// if they're onfoot, put them in the vehicle.
			CVehicle *pVehicle = pVehiclePool->GetAt(m_VehicleID);
			if(pVehicle)
			{
				int iCarID = pVehiclePool->FindGtaIDFromID(m_VehicleID);
				m_pPlayerPed->PutDirectlyInVehicle(iCarID, m_byteSeatID);
			}
		}
	}
}