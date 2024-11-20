#pragma once

#define PLAYER_STATE_NONE			0
#define PLAYER_STATE_ONFOOT			17
#define PLAYER_STATE_PASSENGER		18
#define PLAYER_STATE_DRIVER			19
#define PLAYER_STATE_WASTED			32
#define PLAYER_STATE_SPAWNED		33

#define UPDATE_TYPE_NONE			0
#define UPDATE_TYPE_ONFOOT			16
#define UPDATE_TYPE_INCAR			17
#define UPDATE_TYPE_PASSENGER		18

class CRemotePlayer
{
public:
	CRemotePlayer();
	~CRemotePlayer();

	void SetID(PLAYERID playerId) {
		m_PlayerID = playerId;
	}

	PLAYERID GetID() {
		return m_PlayerID;
	}

	void SetNPC() {
		m_bIsNPC = true;
	}

	bool IsNPC() {
		return m_bIsNPC;
	}

	uint8_t GetState() { return m_byteState; }
	void SetState(uint8_t byteState) {
		if (byteState != m_byteState) {
			StateChange(byteState, m_byteState);
			m_byteState = byteState;
		}
	}
	void StateChange(uint8_t byteNewState, uint8_t byteOldState);

	VEHICLEID CurrentVehicleID() { return m_VehicleID; }

	void SetPlayerColor(uint32_t dwColor);
	uint32_t GetPlayerColor();

	void Process();
	bool Spawn(uint8_t byteTeam, int iSkin, CVector* vecPos, float fRotation,
		uint32_t dwColor, uint8_t byteFightingStyle);
	void Remove();
	void HandleDeath();

	void Say(char* szMessage);

	void SetTeam(uint8_t byteTeam) { m_byteTeam = byteTeam; }
	

	CPlayerPed* GetPlayerPed() { return m_pPlayerPed; }

	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *ofSync, uint32_t dwTime);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync, uint32_t dwTime);
	void StoreAimFullSyncData(AIM_SYNC_DATA* aimSync);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA* psSync);
	void StoreBulletFullSyncData(BULLET_SYNC_DATA* btSync);
	void StoreTrailerFullSyncData(TRAILER_SYNC_DATA *trSync);

	void ResetAllSyncAttributes();
	bool IsActive() {
		if (m_pPlayerPed && m_byteState != PLAYER_STATE_NONE) {
			return true;
		}
		else {
			return false;
		}
	}
	bool SurfingOnVehicle();
	bool SurfingOnObject();

	void UpdateOnFootTargetPosition();
	void UpdateOnFootPositionAndSpeed(CVector* vecPos, CVector* vecMove);

	void UpdateInCarTargetPosition();
	void UpdateInCarMatrixAndSpeed(RwMatrix* pMat, CVector* pVecPos, CVector* pVecMoveSpeed);
	void UpdateVehicleRotation();

	void InterpolateAndRotate();

	void RemoveFromVehicle();
	void PutInCurrentVehicle();
	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();

	float GetDistanceFromLocalPlayer();

	void ShowGlobalMarker(short sPosX, short sPosY, short sPosZ);
	void HideGlobalMarker();

	bool IsVoiceActive();

	void ProcessSurfing();

	void ProcessSpecialActions(uint8_t byteSpecialAction);

	void ProcessAnimation();

	void StoreUnoccupiedSyncData(UNOCCUPIED_SYNC_DATA *unocSync);

	void HandleVehicleEntryExit();

public:
	bool			m_bShowNameTag;
	CVehicle*		m_pCurrentVehicle;
	float			m_fReportedHealth;
	float			m_fReportedArmour;
	bool			m_bIsAFK;
	bool			m_bAppliedAnimation;
	uint8_t			m_byteTeam;

private:
	PLAYERID		m_PlayerID;
	VEHICLEID		m_VehicleID;
	bool			m_bIsNPC;
	uint8_t			m_byteState;
	uint8_t			m_byteUpdateFromNetwork;
	uint8_t			m_byteSpecialAction;
	
	bool			m_bPassengerDriveByMode;
	uint32_t		m_dwAnimation;
	uint8_t			m_byteSeatID;
	
	
	CVector			m_vecOnFootTargetPos;
	CVector			m_vecOnFootTargetSpeed;
	CVector			m_vecPositionInaccuracy;
	CQuaternion		m_quat;
	CVector			m_vecInCarTargetPos;
	CVector			m_vecInCarTargetSpeed;

	ONFOOT_SYNC_DATA		m_ofSync;
	INCAR_SYNC_DATA			m_icSync;
	PASSENGER_SYNC_DATA		m_psSync;


	uint32_t		m_dwLastRecvTick;
	uint32_t		m_dwLastStoredSyncDataTime;

	CPlayerPed*		m_pPlayerPed;
	uint32_t		m_dwMarker;

	bool			m_bGlobalMarkerShown;
	CVector			m_vecGlobalMarkerPos;

	bool 			m_bWasAnimSettedFlag;
	uint32_t 		m_dwPlayingAnimIndex;
    uint8_t 		m_byteWeaponShotID;

	CVector 				m_vecPosOffset;

};