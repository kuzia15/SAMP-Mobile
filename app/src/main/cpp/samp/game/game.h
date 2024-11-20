#pragma once

#include "common.h"
#include "RW/RenderWare.h"
#include "aimstuff.h"
#include "Camera.h"
#include "playerped.h"
#include "actor.h"
#include "vehicle.h"
#include "object.h"
#include "font.h"
#include "textdraw.h"
#include "scripting.h"
#include "util.h"
#include "radarcolors.h"
#include "pad.h"
#include "snapshothelper.h"
#include "materialtextgenerator.h"
#include "../game/Core/Quaternion.h"

class CGame
{
public:
	CGame();
	~CGame();

	void Initialize();
	void StartGame();

	void SetMaxStats();
	void ToggleThePassingOfTime(bool bOnOff);
	void EnableClock(bool bEnable);
	void EnableZoneNames(bool bEnable);
	void SetWorldTime(int iHour, int iMinute);
	void GetWorldTime(int *iHour, int *iMinute);
	void PreloadObjectsAnims();
	void SetWorldWeather(int iWeatherID);
	void DisplayHUD(bool bDisp);
	void UpdateCheckpoints();
	uint8_t GetActiveInterior();
	uint8_t GetPedSlotsUsed();
	void PlaySound(int iSound, float fX, float fY, float fZ);
	void RefreshStreamingAt(float x, float y);
	void DisableTrainTraffic();
	void UpdateGlobalTimer(uint32_t dwTime);
	void SetGravity(float fGravity);
	float FindGroundZForCoord(float fX, float fY, float fZ);

	void DrawGangZone(float fPos[], uint32_t dwColor, uint32_t dwUnk);
	
	uint32_t CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int* pdwindex);
	CObject* NewObject(int iModel, CVector vecPos, CVector vecRot, float fDrawDistance);
	CPlayerPed* NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, bool unk, bool bIsNPC);
	bool RemovePlayer(CPlayerPed* pPlayer);

	CVehicle* NewVehicle(int iVehicleType, float fX, float fY, float fZ, float fRotation, bool bAddSiren);

	void DisableMarker(uint32_t dwMarker);

	uint32_t CreateRadarMarkerIcon(uint8_t byteType, float fPosX, float fPosY, float fPosZ, uint32_t dwColor, uint8_t byteStyle);

	bool IsModelLoaded(int iModel);
	void RequestModel(uint16_t iModelID, uint8_t iLoadingStream = 0x2);
	void LoadRequestedModels();
	void RemoveModel(int iModel, bool bFromStreaming);

	bool IsAnimationLoaded(const char* szAnimLib);
	void RequestAnimation(const char* szAnimLib);


	static const char* GetDataDirectory();

	// 0.3.7
	CPlayerPed* FindPlayerPed() {
		if (m_pGamePlayer == nullptr) {
			m_pGamePlayer = new CPlayerPed();
		}
		return m_pGamePlayer;
	}

	bool IsGamePaused();
	bool IsGameLoaded();
	void DisableAutoAim();
	void EnabledAutoAim();
	void SetWantedLevel(uint8_t level);
	void EnableStuntBonus(bool bEnable);
	void DisplayGameText(const char* szStr, int iTime, int iSize);
	void AddToLocalMoney(int iAmmount);
	void ResetLocalMoney();
	int GetLocalMoney();
	void DisableEnterExits();

	void SetCheckpointInformation(CVector* vecPos, CVector* vecSize);
	void SetRaceCheckpointInformation(uint8_t byteType, CVector *vecPos, CVector* vecNextPos, float fRadius);
	void MakeRaceCheckpoint();
	void DisableRaceCheckpoint();

	void EnableGameInput(bool enable) { m_bInputEnable = enable; };
	bool IsGameInputEnabled() const { return m_bInputEnable; }

	void ToggleCJWalk(bool bUseCJWalk);

	bool bIsGameExiting = false;
public:
	bool m_bCheckpointsEnabled;
	bool m_bRaceCheckpointsEnabled;

private:
	CPlayerPed* m_pGamePlayer;

	bool m_bClockEnabled;
	bool m_bPreloadedVehicleModels[212];

	CVector m_vecCheckpointPos;
	CVector m_vecCheckpointExtent;
	uint32_t m_dwCheckpointMarker;

	CVector m_vecRaceCheckpointPos;
	CVector m_vecRaceCheckpointNextPos;
	uint8_t m_byteRaceType;
	float m_fRaceCheckpointRadius;
	uint32_t m_dwRaceCheckpointMarker;
	uint32_t m_dwRaceCheckpointHandle;

	bool m_bInputEnable;
};