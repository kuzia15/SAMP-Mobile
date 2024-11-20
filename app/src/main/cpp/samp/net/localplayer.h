#pragma once

#define SPECTATE_TYPE_NONE						0
#define SPECTATE_TYPE_PLAYER					1
#define SPECTATE_TYPE_VEHICLE					2

#define SPECIAL_ACTION_NONE						0
#define SPECIAL_ACTION_DUCK						1
#define SPECIAL_ACTION_USEJETPACK				2
#define SPECIAL_ACTION_ENTER_VEHICLE			3
#define SPECIAL_ACTION_EXIT_VEHICLE				4
#define SPECIAL_ACTION_DANCE1					5
#define SPECIAL_ACTION_DANCE2					6
#define SPECIAL_ACTION_DANCE3					7
#define SPECIAL_ACTION_DANCE4					8
#define SPECIAL_ACTION_HANDSUP					10
#define SPECIAL_ACTION_USECELLPHONE				11
#define SPECIAL_ACTION_SITTING					12
#define SPECIAL_ACTION_STOPUSECELLPHONE			13
#define SPECIAL_ACTION_DRINK_BEER				20
#define SPECIAL_ACTION_SMOKE_CIGGY				21
#define SPECIAL_ACTION_DRINK_WINE				22
#define SPECIAL_ACTION_DRINK_SPRUNK				23
#define SPECIAL_ACTION_CUFFED					24
#define SPECIAL_ACTION_CARRY					25
#define SPECIAL_ACTION_PISSING					68

#define BULLET_HIT_TYPE_NONE					0
#define BULLET_HIT_TYPE_PLAYER					1
#define BULLET_HIT_TYPE_VEHICLE					2
#define BULLET_HIT_TYPE_OBJECT					3
#define BULLET_HIT_TYPE_PLAYER_OBJECT			4

#define INVALID_PLAYER_ID	0xFFFF
#define NO_TEAM 0xFF

#define VOICE_CHANNEL_STATE_CLOSED		0
#define VOICE_CHANNEL_STATE_WAIT_ACCEPT	1
#define VOICE_CHANNEL_STATE_ACCEPTED	2
#pragma pack(push, 1)
typedef struct _PLAYER_SPAWN_INFO
{
	uint8_t byteTeam;
	int iSkin;
	uint8_t unk;
	CVector vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _ONFOOT_SYNC_DATA
{
	uint16_t lrAnalog;				// +0
	uint16_t udAnalog;				// +2
	uint16_t wKeys;					// +4
	CVector vecPos;					// +6
	CQuaternion quat;				// +18
	uint8_t byteHealth;				// +34
	uint8_t byteArmour;				// +35
	uint8_t byteCurrentWeapon;		// +36
	uint8_t byteSpecialAction;		// +37
	CVector vecMoveSpeed;			// +38
	CVector vecSurfOffsets;			// +50
	uint16_t wSurfID;				// +62
	uint32_t dwAnimation;			// 64
} ONFOOT_SYNC_DATA;					// size = 68
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _SPECTATOR_SYNC_DATA
{
	uint16_t	lrAnalog;
	uint16_t	udAnalog;
	uint16_t	wKeys;
	CVector		vecPos;
} SPECTATOR_SYNC_DATA;
#pragma pack(pop)

enum eWeaponState
{
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};
#pragma pack(push, 1)
typedef struct _AIM_SYNC_DATA
{
	uint8_t	byteCamMode;
	CVector vecAimf;
	CVector vecAimPos;
	float fAimZ;
	uint8_t byteCamExtZoom : 6;
	uint8_t byteWeaponState : 2;
	uint8_t aspect_ratio;
} AIM_SYNC_DATA;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	CQuaternion quat;
	CVector vecPos;
	CVector vecMoveSpeed;
	float fCarHealth;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSirenOn;
	uint8_t byteLandingGearState;
	uint16_t TrailerID;
	float fTrainSpeed;
} INCAR_SYNC_DATA;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;
	uint8_t byteSeatFlags;
	uint8_t byteCurrentWeapon;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	CVector vecPos;
} PASSENGER_SYNC_DATA;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _BULLET_SYNC_DATA
{
	uint8_t byteHitType;
	PLAYERID PlayerID;
	CVector vecOrigin;
	CVector vecPos;
	CVector vecOffset;
	uint8_t byteWeaponID;
} BULLET_SYNC_DATA;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _TRAILER_SYNC_DATA
{
	VEHICLEID trailerId;			// +0
	CVector vecPos;			// +2
	CQuaternion quat;		// +14
	CVector vecMoveSpeed;	// +30
	CVector vecTurnSpeed;	// +42
} TRAILER_SYNC_DATA; 		// size = 54
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _UNOCCUPIED_SYNC_DATA
{
	VEHICLEID vehicleId;			// +0
	uint8_t byteSeatId;		// +2
	CVector vecRoll;			// +3
	CVector vecDirection;	// +15
	CVector vecPos;			// +27
	CVector vecMoveSpeed;	// +39
	CVector vecTurnSpeed;	// +51
	float fCarHealth;		// +63
} UNOCCUPIED_SYNC_DATA;		// size = 67
#pragma pack(pop)
class CLocalPlayer
{
public:
	CLocalPlayer();
	~CLocalPlayer();

	bool Process();
	bool Spawn();
	void ResetAllSyncAttributes();

	void ToggleSpectating(bool bToggle);
	void ProcessSpectating();
	bool IsSpectating() { return m_bIsSpectating; }
	bool IsInRCMode() { return m_bInRCMode; };
	void SpectatePlayer(PLAYERID PlayerID);
	void SpectateVehicle(VEHICLEID VehicleID);

	void CheckWeapons();
	void UpdateRemoteInterior(uint8_t byteInterior);
	void UpdateCameraTarget();
	void ProcessSurfing();
	void UpdateSurfing();
	void UpdateVehicleDamage(VEHICLEID vehicleID);
	bool IsClearedToSpawn() { return m_bClearedToSpawn; }
	void HandleClassSelection();
	void MoveHeadWithCamera();

	//void HandlePassengerEntry();
	//bool EnterVehicleAsPassenger();
	bool HandlePassengerEntry();
	bool GbuttonEnterVehicleAsPassenger();
	bool EnterVehicleAsDriver();

	void ProcessOnFootWorldBounds();
	void ProcessInCarWorldBounds();
	bool CompareOnFootSyncKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	void ApplySpecialAction(uint8_t byteSpecialAction);
	void SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawnInfo);
	void HandleClassSelectionOutcome(bool bOutcome);
	uint8_t GetSpecialAction();
	uint32_t GetPlayerColorAsARGB();
	uint32_t GetPlayerColorAsRGBA();

	void DisableSurf() { m_surfData.bIsActive = false; }

	void SetPlayerColor(uint32_t dwColor);

	int GetOptimumOnFootSendRate();
	int GetOptimumInCarSendRate();

	void SendPrevClass();
	void SendNextClass();
	void SendSpawn();
	void RequestClass(int iClass);
	void RequestSpawn();
	void ProcessClassSelection();

	void SendStatsUpdate();
	void SendWastedNotification();
	void SendOnFootFullSyncData();
	void SendInCarFullSyncData();
	void SendPassengerFullSyncData();
	void SendAimSyncData();

	bool IsNeedSyncDataSend(const void* data1, const void* data2, size_t size);

	void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger);
	void SendExitVehicleNotification(VEHICLEID VehicleID);

	void SendTakeDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType);
	void SendGiveDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType);
	void SendGiveDamageActorEvent(PLAYERID ActorID, float fDamageFactor, int weaponType, int pedPieceType);

	CPlayerPed* GetPlayerPed() { return m_pPlayerPed; }

	/* voice */
	void UpdateVoice();
	void VoiceChannelAccept();
	void VoiceChannelClose();

	void SendVCData();
	void SendVCOpenRequest();
	void SendVCClose();

	void SendTrailerData(VEHICLEID vehicleId);

	void GiveTakeDamage(bool bGiveOrTake, uint16_t wPlayerID, float damage_amount, uint32_t weapon_id, uint32_t bodypart);

	uint32_t GetCurrentAnimationIndexFlag();

	void SendBulletSyncData(PLAYERID byteHitID, uint8_t byteHitType, CVector vecHitPos);

	void GiveActorDamage(PLAYERID wPlayerID, float damage_amount, uint32_t weapon_id, uint32_t bodypart);

	int					m_iVCState;
	uint32_t			m_dwVCOpenRequestTick;

	void SetTeam(uint8_t byteTeam) { m_byteTeam = byteTeam; };
	uint8_t GetTeam() { return m_byteTeam; };

	bool ProcessUnoccupiedSync(VEHICLEID vehicleId, CVehicle *pVehicle);

    bool				m_bPassengerDriveByMode;
public:
	bool				m_bWaitingForSpawnRequestReply;
	uint32_t			m_SpectateID;
	bool				m_bSpectateProcessed;
	uint8_t				m_byteSpectateMode;
	uint8_t				m_byteSpectateType;
	uint8_t				m_byteTeam;
	bool				m_bIsActive;
	int					m_iSelectedClass;
	VEHICLEID			m_LastVehicle;
	VEHICLEID			m_nLastVehicle;


	bool                 m_bWasInCar;
private:
	CPlayerPed*			m_pPlayerPed;
	bool				m_bIsWasted;
	bool				m_bClearedToSpawn;
	bool				m_bInRCMode;
    bool				m_bHasSpawnInfo;
	uint32_t			m_dwAnimation;
	bool				m_bSpawnDialogShowed;
	uint8_t				m_byteCurrentWeapon;
	uint8_t				m_byteLastWeapon[13];
	uint32_t			m_dwLastAmmo[13];

	PLAYER_SPAWN_INFO	m_SpawnInfo;
	ONFOOT_SYNC_DATA	m_ofSync;
	INCAR_SYNC_DATA		m_icSync;
	PASSENGER_SYNC_DATA	m_psSync;
	AIM_SYNC_DATA		m_aimSync;
	TRAILER_SYNC_DATA 	m_TrailerData;
	UNOCCUPIED_SYNC_DATA m_UnoccupiedData;

	bool				m_bIsSpectating;

	int					m_iDisplayZoneTick;
	uint32_t			m_dwLastStatsUpdateTick;
	uint32_t			m_dwLastSendTick;
	uint32_t    		m_dwLastUpdateOnFootData;
	uint32_t			m_dwLastSendAimSyncTick;
	uint32_t			m_dwLastSendSyncTick;
	uint32_t			m_dwLastUpdateInCarData;
	uint32_t			m_dwLastSendSpecTick;
	uint32_t			m_dwLastWeaponsUpdateTick;
	uint32_t 			m_dwLastPerformStuffAnimTick;

	uint8_t				m_byteCurInterior;
	VEHICLEID			m_CurrentVehicle;

	bool				m_bPerformingStuffAnim;

	struct {
		CVector vecOffsetPos;
		int dwSurfVehID;

		bool bIsActive;
		bool bIsVehicle;
		uintptr_t pSurfInst;
	} m_surfData;

	struct {
		uint32_t dwLastMoney;
		uint32_t dwLastDrunkLevel;
	} m_statsData;

	void SendUnoccupiedData(VEHICLEID vehicleId, CVehicle *pVehicle);

	int GetOptimumUnoccupiedSendRate();

    bool EnterVehicleAsPassenger();
};