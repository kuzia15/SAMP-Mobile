#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "vehicle.h"
#include "Streaming.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

bool bInProcessDetachTrailer = false;

CVehicle::CVehicle(int iType, float fX, float fY, float fZ, float fRotation, bool bPreloaded, bool bSiren)
{
	uint32_t dwRetID = 0;
	CMatrix matEnt;
	static CVehicleGTA* pCreatedTrain = nullptr;

	m_pVehicle = nullptr;
	m_dwGTAId = 0;
	m_pTrailer = nullptr;

	if ((iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM)) {

        if (!CStreaming::TryLoadModel(iType))
            throw std::runtime_error("Model not loaded");

		ScriptCommand(&create_car, iType, fX, fY, fZ, &dwRetID);
		ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
		ScriptCommand(&car_gas_tank_explosion, dwRetID, 0);
		ScriptCommand(&set_car_hydraulics, dwRetID, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 0);

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_dwGTAId = dwRetID;

		if (m_pVehicle) 
		{
			m_pVehicle->m_nDoorLock = (eCarLock)0;
			m_bIsLocked = false;

			m_pVehicle->GetMatrix(reinterpret_cast<RwMatrix *>(&matEnt));
			matEnt.m_pos.x = fX;
			matEnt.m_pos.y = fY;
			matEnt.m_pos.z = fZ;

			if( GetVehicleSubtype() != VEHICLE_SUBTYPE_BIKE &&
				GetVehicleSubtype() != VEHICLE_SUBTYPE_PUSHBIKE)
				matEnt.m_pos.z += 0.25f;

			m_pVehicle->SetMatrix(matEnt);
			m_bPreloaded = bPreloaded;
		}

		//Remove();
		//pGame->RemoveModel(iType, false);
	}
	else if ((iType == TRAIN_PASSENGER_LOCO) ||
		(iType == TRAIN_FREIGHT_LOCO) ||
		(iType == TRAIN_TRAM))
	{
		if (iType == TRAIN_PASSENGER_LOCO) iType = 5;
		else if (iType == TRAIN_FREIGHT_LOCO) iType = 3;
		else if (iType == TRAIN_TRAM) iType = 9;

		uint32_t dwDirection = 0;
		if (fRotation > 180.0f) {
			dwDirection = 1;
		}

        if (!CStreaming::TryLoadModel(TRAIN_PASSENGER_LOCO))
            throw std::runtime_error("Model not loaded");

        if (!CStreaming::TryLoadModel(TRAIN_PASSENGER))
            throw std::runtime_error("Model not loaded");

        if (!CStreaming::TryLoadModel(TRAIN_FREIGHT_LOCO))
            throw std::runtime_error("Model not loaded");

        if (!CStreaming::TryLoadModel(TRAIN_FREIGHT))
            throw std::runtime_error("Model not loaded");
        if (!CStreaming::TryLoadModel(TRAIN_TRAM))
            throw std::runtime_error("Model not loaded");

		ScriptCommand(&create_train, iType, fX, fY, fZ, dwDirection, &dwRetID);
		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_dwGTAId = dwRetID;

		pCreatedTrain = m_pVehicle;

		GamePrepareTrain(m_pVehicle);
	}
	else if ((iType == TRAIN_PASSENGER) ||
		(iType == TRAIN_FREIGHT))
	{
		if (!pCreatedTrain)
		{
			m_pVehicle = nullptr;
			return;
		}

		m_dwGTAId = GamePool_Vehicle_GetIndex(m_pVehicle);
		pCreatedTrain = m_pVehicle;
	}

	m_bIsInvulnerable = false;
	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = false;
	m_dwMarkerID = 0;
	//_this->field_6D = 0;
	//_this->field_69 = GetTickCount();
	m_iEngineState = -1;
	m_iLightState = -1;
	m_bDoorsLocked = false;
	m_bHaveColor = false;
	m_bHasBeenDriven = false;
	m_dwTimeSinceLastDriven = GetTickCount();
	//_this->field_71 = 0;
	//_this->field_75 = 0;
	//_this->field_8F = 0;
	memset(m_szPlateText, 0, sizeof(m_szPlateText));
}

CVehicle::~CVehicle()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if (m_pVehicle) {
		if(m_dwMarkerID)
		{
			pGame->DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		RemoveEveryoneFromVehicle();

		if(SirenEnabled()) EnableSiren(false);

		if (m_pTrailer) {
			DetachTrailer();
			m_pTrailer = nullptr;
		}

		int iModel = m_pVehicle->m_nModelIndex;
		if (iModel == 538 || iModel == 537)
		{
			ScriptCommand(&destroy_train, m_dwGTAId);
		}
		else
		{
			ScriptCommand(&destroy_car, m_dwGTAId);
			int iRefs = GetModelRefCounts(iModel);
			FLog("Destroy car(%d): Refs: %d", iModel, iRefs);
			if (!iRefs)
			{
				pGame->RemoveModel(iModel, true);
			}
		}
	}
}
// 0.3.7
bool CVehicle::IsRCVehicle()
{
	if (m_pVehicle)
	{
		if (GamePool_Vehicle_GetAt(m_dwGTAId))
		{
			int iModel = m_pVehicle->m_nModelIndex;
			if (iModel == 441
				|| iModel == 464
				|| iModel == 465
				|| iModel == 594
				|| iModel == 501
				|| iModel == 564) {
				return true;
			}
		}
	}

	return false;
}
// 0.3.7
float CVehicle::GetHealth()
{
	if (m_pVehicle) {
		return m_pVehicle->fHealth;
	}

	return 0.0f;
}
// 0.3.7
void CVehicle::SetHealth(float fHealth)
{
	if (m_pVehicle) {
		m_pVehicle->fHealth = fHealth;
	}
}
// 0.3.7
int CVehicle::GetVehicleSubtype()
{
	if (m_pVehicle) {
		return ::GetVehicleSubtype(m_pVehicle);
	}

	return 0;
}
// 0.3.7
void CVehicle::AddComponent(int iComponentID)
{
	if (!m_pVehicle || !GamePool_Vehicle_GetAt(m_dwGTAId)) return;
	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR) return;

    if (!CStreaming::TryLoadModel(iComponentID))
        throw std::runtime_error("Model not loaded");

	if (!ScriptCommand(&is_component_available, iComponentID)) {
		return;
	}

	uint32_t dwRet;
	ScriptCommand(&add_car_component, m_dwGTAId, iComponentID, &dwRet);
}
// 0.3.7
void CVehicle::SetPaintJob(uint8_t bytePaintJobID)
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		{
			if (bytePaintJobID <= 3) {
                if (m_dwGTAId) ScriptCommand(&change_car_skin, m_dwGTAId, bytePaintJobID);
			}
		}
	}
}
// 0.3.7
void CVehicle::SetColor(uint8_t byteColor1, uint8_t byteColor2)
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		m_pVehicle->m_nPrimaryColor = byteColor1;
		m_pVehicle->m_nSecondaryColor = byteColor2;
	}

	m_byteColor1 = byteColor1;
	m_byteColor2 = byteColor2;
	m_bHaveColor = true;
}
// 0.3.7
void CVehicle::DetachTrailer()
{
	bInProcessDetachTrailer = true;
	if (m_pTrailer)
	{
		if (m_dwGTAId && GamePool_Vehicle_GetAt(m_dwGTAId))
		{
			if (m_pTrailer->m_pVehicle)
				ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
		}
	}
}
// 0.3.7
void CVehicle::SetTrailer(CVehicle* pTrailer)
{
	m_pTrailer = pTrailer;
}
// 0.3.7
void CVehicle::LinkToInterior(int iInterior)
{
	if (GamePool_Vehicle_GetAt(m_dwGTAId)) {
		ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
	}
}

void CVehicle::SetDamageStatus(uint32_t dwPanelDamageStatus, uint32_t dwDoorDamageStatus, uint8_t byteLightDamageStatus)
{

}

void CVehicle::SetTireDamageStatus(uint8_t byteTireDamageStatus)
{

}
// 0.3.7
void CVehicle::RemoveEveryoneFromVehicle()
{
	if (!m_pVehicle) return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	float fPosX = m_pVehicle->GetPosition().x;
	float fPosY = m_pVehicle->GetPosition().y;
	float fPosZ = m_pVehicle->GetPosition().z;

	int iPlayerID = 0;
	if (m_pVehicle->pDriver) {
		iPlayerID = GamePool_Ped_GetIndex(m_pVehicle->pDriver);
		ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2);
	}

	for (int i = 0; i < 7; i++) {
		if (m_pVehicle->m_apPassengers[i] != NULL) {
			iPlayerID = GamePool_Ped_GetIndex(m_pVehicle->m_apPassengers[i]);
			ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2);
		}
	}
}
// 0.3.7
CVehicle* CVehicle::GetTrailer()
{
	if (m_pVehicle)
	{
        CVehicleGTA* pTrailer = m_pVehicle->m_pTrailer;
		if (pTrailer)
		{
			if (pNetGame)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool) {
					VEHICLEID TrailerID = pVehiclePool->FindIDFromGtaPtr(pTrailer);
					if (TrailerID != INVALID_VEHICLE_ID)
					{
						return pVehiclePool->GetAt(TrailerID);
					}
				}
			}
		}
	}

	return nullptr;
}
// 0.3.7
CVehicle* CVehicle::GetTractor()
{
	if (!m_pVehicle) return nullptr;

	int iSubType = ::GetVehicleSubtype(m_pVehicle);
	if (iSubType == VEHICLE_SUBTYPE_BIKE
		|| iSubType == VEHICLE_SUBTYPE_BOAT
		|| iSubType == VEHICLE_SUBTYPE_TRAIN
		|| iSubType == VEHICLE_SUBTYPE_PUSHBIKE)
	{
		return nullptr;
	}

    CVehicleGTA* pTractor = m_pVehicle->m_pTowingVehicle;
	if (pTractor)
	{
		if (pNetGame)
		{
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				VEHICLEID TrailerID = pVehiclePool->FindIDFromGtaPtr(pTractor);
				if (TrailerID != INVALID_VEHICLE_ID)
				{
					return pVehiclePool->GetAt(TrailerID);
				}
			}
		}
	}

	return nullptr;
}
// 0.3.7
float CVehicle::GetTrainSpeed()
{
	//if (m_pVehicle) {
	//	return m_pVehicle->fTrainSpeed;
	//}
	
	return 0.0f;
}
// 0.3.7 (dword -> word)
uint16_t CVehicle::GetHydraThrusters()
{
	//if (m_pVehicle) {
	//	return m_pVehicle->wHydraThrusters;
	//}

	return 0.0f;
}

bool CVehicle::IsLandingGearNotUp()
{
	//if(m_pVehicle)
		//return m_pVehicle->fPlaneLandingGear != 0.0f;

	return 0;
}

float CVehicle::GetBikeLean()
{
	//if (m_pVehicle && GamePool_Vehicle_GetAt(m_dwGTAId))
		//return m_pVehicle->fBikeLean;

	return 0.0f;
}
// 0.3.7
bool CVehicle::IsATrainPart()
{
	if (m_pVehicle)
	{
		int iModelIndex = m_pVehicle->m_nModelIndex;
		if (iModelIndex == 538 || iModelIndex == 570 || iModelIndex == 537 ||
			iModelIndex == 569 || iModelIndex == 449) {
			return true;
		}
	}

	return false;
}
// 0.3.7
bool CVehicle::VerifyInstance()
{
	return GamePool_Vehicle_GetAt(m_dwGTAId) != nullptr;
}
// 0.3.7
void CVehicle::SetDoorState(int state)
{
	if (state)
	{
		m_pVehicle->m_nDoorLock = (eCarLock)2;
		m_bDoorsLocked = true;
	}
	else
	{
		m_pVehicle->m_nDoorLock =(eCarLock) 0;
		m_bDoorsLocked = false;
	}
}

void CVehicle::SetComponentOpenState(int iDoor, int iComponent, float fDoorOpenRatio)
{
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
	{
		ScriptCommand(&set_car_door_rotation_to, m_dwGTAId, iDoor, iComponent, fDoorOpenRatio);
	
	}
}

void CVehicle::OpenWindow(uint8_t component)
{
	if (m_pVehicle) {

	}
}

void CVehicle::CloseWindow(uint8_t component)
{
	if (m_pVehicle) {

	}
}
// 0.3.7
void CVehicle::AttachTrailer()
{
	if (m_pTrailer) {
		ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
	}
}
// 0.3.7
void CVehicle::RemoveComponent(int iComponentID)
{
	if (!m_pVehicle || !GamePool_Vehicle_GetAt(m_dwGTAId))
		return;

	ScriptCommand(&remove_component, m_dwGTAId, iComponentID);
}
// 0.3.7
void CVehicle::SetZAngle(float fAngle)
{
	if (GamePool_Vehicle_GetAt(m_dwGTAId)) {
		ScriptCommand(&set_car_z_angle, m_dwGTAId, fAngle);
	}
}
// 0.3.7
bool CVehicle::IsDriverLocalPlayer()
{
	if (m_pVehicle && m_pVehicle->pDriver == GamePool_FindPlayerPed())
		return true;

	return false;
}
// 0.3.7
void CVehicle::SetInvulnerable(bool bInv)
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		if (bInv)
		{
			ScriptCommand(&set_car_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
			ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
			m_bIsInvulnerable = true;
		}
		else
		{
			ScriptCommand(&set_car_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
			ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
			m_bIsInvulnerable = false;
		}
	}
}
// 0.3.7
bool CVehicle::HasSunk()
{
	if (m_pVehicle) {
		return ScriptCommand(&has_car_sunk, m_dwGTAId);
	}

	return false;
}
// 0.3.7
bool CVehicle::HasADriver()
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		if (m_pVehicle->pDriver)
		{
			if (m_pVehicle->pDriver->IsInVehicle() &&
				m_pVehicle->pDriver->m_nPedType == (ePedType)0)
				return true;
		}
	}

	return false;
}

void CVehicle::ApplyEngineState(int iState)
{
	if (!m_pVehicle) return;

	m_iEngineState = iState;

    m_pVehicle->m_nVehicleFlags.bEngineOn = iState;
}

void CVehicle::ApplyLightState(int iState)
{
	if (!m_pVehicle) return;
	//if(iState > 1)
	//	return;

	m_iLightState = iState;
    m_pVehicle->m_nVehicleFlags.bLightsOn = iState;
}
// 0.3.7
void CVehicle::ProcessMarkers()
{
	if(!m_pVehicle || !GamePool_Vehicle_GetAt(m_dwGTAId))
		return;

	if(m_byteObjectiveVehicle)
	{
		if(!m_bSpecialMarkerEnabled)
		{
			if(m_dwMarkerID)
			{
				pGame->DisableMarker(m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}

		return;
	}

	if(m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
	{
		if(m_dwMarkerID)
		{
			pGame->DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;

			m_bSpecialMarkerEnabled = false;
		}
	}

	if(m_pVehicle->GetDistanceFromLocalPlayerPed() < 200.0f && GetTractor() == NULL &&
	   !IsOccupied())
	{
		if(!m_dwMarkerID)
		{
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
		}
	}

	else if(IsOccupied() || GetTractor() != NULL ||
            m_pVehicle->GetDistanceFromLocalPlayerPed() >= 200.0f)
	{
		if(m_dwMarkerID)
		{
			pGame->DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}
// 0.3.7
bool CVehicle::IsOccupied()
{
	if (m_pVehicle)
	{
		if (m_pVehicle &&
			(m_pVehicle->m_apPassengers[0] ||
			m_pVehicle->m_apPassengers[1] ||
			m_pVehicle->m_apPassengers[2] ||
			m_pVehicle->m_apPassengers[3] ||
			m_pVehicle->m_apPassengers[4] ||
			m_pVehicle->m_apPassengers[5] ||
			m_pVehicle->m_apPassengers[6])) {
			return true;
		}
	}

	return false;
}

bool CVehicle::IsATowTruck()
{
	if(!m_pVehicle) return false;
	return m_pVehicle->m_nModelIndex == 525;
}

bool CVehicle::IsATrailer()
{
	if(!m_pVehicle) return false;

  	int nModel = m_pVehicle->m_nModelIndex;
	return (nModel == 435 ||
		nModel == 450 || 
		nModel == 584 || 
		nModel == 591 || 
		nModel == 606 || 
		nModel == 607 || 
		nModel == 608 || 
		nModel == 610 || 
		nModel == 611
	);
}

void CVehicle::UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage)
{
	if(m_pVehicle)
	{
		if(GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		{
			if(!dwPanelDamage && !dwDoorDamage && !byteLightDamage)
			{
				if(GetPanelDamageStatus() || GetDoorDamageStatus() || GetLightDamageStatus())
				{
					// CAutoMobile::Fix
					(( void (*)(CVehicleGTA*))(g_libGTASA+0x55D550+1))(m_pVehicle);
					return;
				}
			}

			SetDoorDamageStatus(dwDoorDamage);
			SetPanelDamageStatus(dwPanelDamage);
			SetLightDamageStatus(byteLightDamage);

			// CAutomobile::SetupDamageAfterLoad
			(( void (*)(CVehicleGTA*))(g_libGTASA+0x55D816+1))(m_pVehicle);
		}
	}
}

void CVehicle::SetPanelDamageStatus(uint32_t dwPanelStatus)
{
	//if(m_pVehicle && GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		//m_pVehicle->dwPanelStatus = dwPanelStatus;
}

uint32_t CVehicle::GetPanelDamageStatus()
{
	//if(m_pVehicle && GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		//return m_pVehicle->dwPanelStatus;

	return 0;
}

void CVehicle::SetDoorDamageStatus(uint32_t dwDoorStatus)
{
	/*if(m_pVehicle && GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
	{
		m_pVehicle->byteDoorStatus[0] = dwDoorStatus & 7;
		m_pVehicle->byteDoorStatus[1] = (dwDoorStatus >> 8) & 7;
		m_pVehicle->byteDoorStatus[2] = (dwDoorStatus >> 16) & 7;
		m_pVehicle->byteDoorStatus[3] = (dwDoorStatus >> 24) & 7;
	}*/
}

uint32_t CVehicle::GetDoorDamageStatus()
{
	/*if(m_pVehicle && GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
	{
		uint32_t dwDoorStatus = (m_pVehicle->byteDoorStatus[0] | (m_pVehicle->byteDoorStatus[1] << 8) | (m_pVehicle->byteDoorStatus[2] << 16) | (m_pVehicle->byteDoorStatus[3] << 24));
		return dwDoorStatus;
	}*/

	return 0;
}

void CVehicle::SetLightDamageStatus(uint8_t byteLightStatus)
{
	//if(m_pVehicle && GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		//m_pVehicle->dwLightStatus = (uint32_t)byteLightStatus;
}

uint8_t CVehicle::GetLightDamageStatus()
{
	//if(m_pVehicle && GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		//return (uint8_t)m_pVehicle->dwLightStatus;

	return 0;
}

void CVehicle::SetWheelPoppedStatus(uint8_t byteWheelStatus)
{
	/*if(m_pVehicle)
	{
		if(GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		{
			m_pVehicle->byteWheelStatus[3] = byteWheelStatus & 1;
			m_pVehicle->byteWheelStatus[2] = (byteWheelStatus >> 1) & 1;
			m_pVehicle->byteWheelStatus[1] = (byteWheelStatus >> 2) & 1;
			m_pVehicle->byteWheelStatus[0] = (byteWheelStatus >> 3) & 1;
		}
		else if(GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
		{
			m_pVehicle->byteBikeWheelStatus[1] = byteWheelStatus & 1;
			m_pVehicle->byteBikeWheelStatus[0] = (byteWheelStatus >> 1) & 1;
		}
	}*/
}

uint8_t CVehicle::GetWheelPoppedStatus()
{
	/*if(m_pVehicle)
	{
		if(GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		{
			uint8_t byteWheelStatus = (m_pVehicle->byteWheelStatus[3] | (m_pVehicle->byteWheelStatus[2] << 1) | (m_pVehicle->byteWheelStatus[1] << 2) | (m_pVehicle->byteWheelStatus[0] << 3));
			return byteWheelStatus;
		}
		else if(GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
		{
			uint8_t byteWheelStatus = (m_pVehicle->byteBikeWheelStatus[1] | (m_pVehicle->byteBikeWheelStatus[0] << 1));
			return byteWheelStatus;
		}
	}*/

	return 0;
}

bool CVehicle::SirenEnabled()
{
	if(!m_pVehicle)
		return m_pVehicle->m_nVehicleFlags.bSirenOrAlarm;

	return false;
}

void CVehicle::EnableSiren(bool bState)
{
	if(!m_pVehicle) return;

	m_pVehicle->m_nVehicleFlags.bSirenOrAlarm = (int)bState;
}

void CVehicle::UpdateColor()
{
	if(m_pVehicle)
	{
		if(m_bHaveColor)
		{
			if(!m_bHasBeenDriven)
			{
				if(m_pVehicle->m_nPrimaryColor != m_byteColor1 || m_pVehicle->m_nSecondaryColor != m_byteColor2)
				{
					m_pVehicle->m_nPrimaryColor = m_byteColor1;
					m_pVehicle->m_nSecondaryColor = m_byteColor2;
				}
			}
		}
	}
}

bool CVehicle::UpdateLastDrivenTime()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->pDriver)
		{
			m_bHasBeenDriven = true;
			m_dwTimeSinceLastDriven = GetTickCount();
			return true;
		}
	}

	return false;
}