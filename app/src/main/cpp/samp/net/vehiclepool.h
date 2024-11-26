#pragma once

#define INVALID_VEHICLE_ID	0xFFFF
#include "../game/util.h"

#pragma pack(push, 1)
typedef struct _NEW_VEHICLE
{
	VEHICLEID VehicleID;
	int iVehicleType;
	CVector vecPos;
	float fRotation;
	uint8_t byteInteriorColor1;
	uint8_t byteInreriorColor2;
	float fHealth;
	uint8_t byteInterior;
	uint32_t dwDoorDamageStatus;
	uint32_t dwPanelDamageStatus;
	uint8_t byteLightDamageStatus;
	uint8_t byteTireDamageStatus;
	uint8_t byteAddSiren;
	uint8_t byteModSlots[14];
	uint8_t bytePaintJob;
	int iBodyColor1;
	int iBodyColor2;
} NEW_VEHICLE;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _VEHICLE_PARAMS_EX
{
	uint8_t byteEngine;		// 0
	uint8_t byteLight;		// 1
	uint8_t byteAlarm;		// 2
	uint8_t byteDoors;		// 3
	uint8_t byteBonnet;		// 4
	uint8_t byteBoot;		// 5
	uint8_t byteObjective;	// 6
	uint8_t byteSiren;		// 7
	uint8_t byteDoor1;		// 8
	uint8_t byteDoor2;		// 9
	uint8_t byteDoor3;		// 10
	uint8_t byteDoor4;		// 11
	uint8_t byteWindow1;	// 12
	uint8_t byteWindow2;	// 13
	uint8_t byteWindow3;	// 14
	uint8_t byteWindow4;	// 15
} VEHICLE_PARAMS_EX;
#pragma pack(pop)

class CVehiclePool
{
public:
	CVehiclePool();
	~CVehiclePool();

	bool New(NEW_VEHICLE* new_veh);
	bool Delete(VEHICLEID VehicleID);

	void Process();
	void NotifyVehicleDeath(VEHICLEID VehicleID);

	// 0.3.7
	CVehicle* GetAt(VEHICLEID vehicleID) {
		if (vehicleID < MAX_VEHICLES && m_bVehicleSlotState[vehicleID])
			return m_pVehicles[vehicleID];

		return nullptr;
	}
	// 0.3.7
	bool GetSlotState(VEHICLEID VehicleID)
	{
		if (VehicleID < MAX_VEHICLES)
			return m_bVehicleSlotState[VehicleID];

		return false;
	}
	// 0.3.7
	VEHICLEID FindIDFromGtaPtr(CVehicleGTA* pGtaVehicle)
	{
		for (int i = 0; i < MAX_VEHICLES; i++)
		{
			if (m_pGTAVehicles[i] == pGtaVehicle) {
				return i;
			}
		}
	}

	// 0.3.7
	uint32_t FindGtaIDFromID(VEHICLEID VehicleID)
	{
		if (m_pGTAVehicles[VehicleID]) {
			return GamePool_Vehicle_GetIndex(m_pGTAVehicles[VehicleID]);
		}
        else
            return INVALID_VEHICLE_ID;
	}

	void AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked);
	void LinkToInterior(VEHICLEID VehicleID, uint8_t byteInterior);
	VEHICLEID FindNearestToLocalPlayerPed();
public:
	CVector			m_vecSpawnPos[MAX_VEHICLES];
	float 			m_fSpawnRotation[MAX_VEHICLES];
private:
	CVehicle* m_pVehicles[MAX_VEHICLES];
	bool m_bVehicleSlotState[MAX_VEHICLES];
    CVehicleGTA* m_pGTAVehicles[MAX_VEHICLES];
	CVector m_vecPos[MAX_VEHICLES];
	bool m_bIsActive[MAX_VEHICLES];
	bool m_bIsWasted[MAX_VEHICLES];
	uint8_t			m_bIsMarker[MAX_VEHICLES];
	bool m_bIsVehicleRespawned[MAX_VEHICLES];
	uint32_t m_dwWastedTime[MAX_VEHICLES];

	PLAYERID		m_lastUndrivenId[MAX_VEHICLES]; // not used
	uint32_t		m_dwLastUndrivenProcessTick[MAX_VEHICLES];
};