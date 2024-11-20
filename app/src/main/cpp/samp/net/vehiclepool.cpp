#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "vehiclepool.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

CVehiclePool::CVehiclePool()
{
    for (int i = 0; i < MAX_VEHICLES; i++)
    {
        m_pVehicles[i] = nullptr;
        m_bVehicleSlotState[i] = false;
        m_pGTAVehicles[i] = nullptr;
        m_bIsActive[i] = false;
        m_bIsWasted[i] = false;
        m_bIsMarker[i] = 0;
        m_dwWastedTime[i] = 0;
    }
}
// 0.3.7
CVehiclePool::~CVehiclePool()
{
    for (int i = 0; i < MAX_VEHICLES; i++)
    {
        Delete(i);
    }
}
// 0.3.7
bool CVehiclePool::New(NEW_VEHICLE* new_veh)
{
    if (m_pVehicles[new_veh->VehicleID]) {
        //if (pUI) gui->chat()->addDebugMessage("Warning: vehicle %u was not deleted", new_veh->VehicleID);
        Delete(new_veh->VehicleID);
    }

    CVehicle* pNewVehicle = pGame->NewVehicle(
            new_veh->iVehicleType,
            new_veh->vecPos.x,
            new_veh->vecPos.y,
            new_veh->vecPos.z,
            new_veh->fRotation,
            new_veh->byteAddSiren);

    m_pVehicles[new_veh->VehicleID] = pNewVehicle;

    if (!m_pVehicles[new_veh->VehicleID]) return false;

    if (new_veh->byteInteriorColor1 != 0xFF || new_veh->byteInreriorColor2 != 0xFF) {
        m_pVehicles[new_veh->VehicleID]->SetColor(new_veh->byteInteriorColor1, new_veh->byteInreriorColor2);
    }

    m_pVehicles[new_veh->VehicleID]->SetHealth(new_veh->fHealth);
    m_pGTAVehicles[new_veh->VehicleID] = m_pVehicles[new_veh->VehicleID]->m_pVehicle;
    m_bVehicleSlotState[new_veh->VehicleID] = true;

    m_vecPos[new_veh->VehicleID].x = new_veh->vecPos.x;
    m_vecPos[new_veh->VehicleID].y = new_veh->vecPos.y;
    m_vecPos[new_veh->VehicleID].z = new_veh->vecPos.z;

    if (new_veh->byteInterior) {
        m_pVehicles[new_veh->VehicleID]->LinkToInterior(new_veh->byteInterior);
    }

    if(new_veh->byteTireDamageStatus)
        m_pVehicles[new_veh->VehicleID]->SetWheelPoppedStatus(new_veh->byteTireDamageStatus);

    if(new_veh->dwPanelDamageStatus || new_veh->dwDoorDamageStatus || new_veh->byteLightDamageStatus)
    {
        m_pVehicles[new_veh->VehicleID]->UpdateDamageStatus(
                new_veh->dwPanelDamageStatus,
                new_veh->dwDoorDamageStatus,
                new_veh->byteLightDamageStatus);
    }

    // Pos & Rotation
    m_vecSpawnPos[new_veh->VehicleID].x = new_veh->vecPos.x;
    m_vecSpawnPos[new_veh->VehicleID].y = new_veh->vecPos.y;
    m_vecSpawnPos[new_veh->VehicleID].z = new_veh->vecPos.z;
    m_fSpawnRotation[new_veh->VehicleID] = new_veh->fRotation;

    m_bIsActive[new_veh->VehicleID] = true;
    m_bIsVehicleRespawned[new_veh->VehicleID] = false;
    m_bIsWasted[new_veh->VehicleID] = false;
    m_dwWastedTime[new_veh->VehicleID] = 0;
    m_bIsMarker[new_veh->VehicleID] = 0;

    //CountVehicles();

    return true;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
    if(!GetSlotState(VehicleID) || !m_pVehicles[VehicleID])
        return false;

    m_bIsActive[VehicleID] = false;
    m_bIsMarker[VehicleID] = 0;
    m_bVehicleSlotState[VehicleID] = false;
    delete m_pVehicles[VehicleID];
    m_pVehicles[VehicleID] = nullptr;
    m_pGTAVehicles[VehicleID] = nullptr;

    //CountVehicles();

    return true;
}
// 0.3.7
void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked)
{
    if (VehicleID < MAX_VEHICLES && m_bVehicleSlotState[VehicleID])
    {
        CVehicle* pVehicle = m_pVehicles[VehicleID];
        if (pVehicle && m_bIsActive[VehicleID])
        {
            if (byteObjective)
            {
                pVehicle->m_byteObjectiveVehicle = true;
                pVehicle->m_bSpecialMarkerEnabled = false;
            }

            pVehicle->SetDoorState(byteDoorsLocked);
        }
    }
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, uint8_t byteInterior)
{
    if (m_bVehicleSlotState[VehicleID] && m_pVehicles[VehicleID]) {
        m_pVehicles[VehicleID]->LinkToInterior(byteInterior);
    }
}
// 0.3.7
VEHICLEID CVehiclePool::FindNearestToLocalPlayerPed()
{
    float fLeastDistance = 10000.0f;
    float fThisDistance;
    VEHICLEID ClosestSoFar = INVALID_VEHICLE_ID;

    VEHICLEID x = 0;
    while (x < MAX_VEHICLES)
    {
        if (m_bVehicleSlotState[x] && m_bIsActive[x])
        {
            fThisDistance = m_pVehicles[x]->GetDistanceFromLocalPlayerPed();
            if (fThisDistance < fLeastDistance)
            {
                fLeastDistance = fThisDistance;
                ClosestSoFar = x;
            }
        }
        x++;
    }

    return ClosestSoFar;
}

/*
 if (pVehicle->GetHealth() <= 0.0f ||
					pVehicle->GetVehicleSubtype() != VEHICLE_SUBTYPE_BOAT &&
					pVehicle->HasSunk()) {
if (pLocalPlayer && pLocalPlayer->m_LastVehicle == VehicleID)
NotifyVehicleDeath(VehicleID);

continue;
}
*/

void CVehiclePool::Process()
{
    uint8_t byteSentUndrivenSync = 0;
    uint32_t dwThisTick = GetTickCount();
    CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

    for (VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++)
    {
        if (m_bVehicleSlotState[VehicleID] && m_bIsActive[VehicleID])
        {
            if (m_bIsWasted[VehicleID] && (dwThisTick - m_dwWastedTime[VehicleID]) > 15000)
            {
                this->Delete(VehicleID);
            }
            else
            {
                CVehicle* pVehicle = m_pVehicles[VehicleID];
                if (pVehicle->IsDriverLocalPlayer()) {
                    pVehicle->SetInvulnerable(false);
                }
                else {
                    pVehicle->SetInvulnerable(true);
                }

                if (pVehicle->GetHealth() == 0.0f)
                {
                    NotifyVehicleDeath(VehicleID);
                    if (!m_bIsWasted[VehicleID])
                    {
                        m_bIsWasted[VehicleID] = true;
                        m_dwWastedTime[VehicleID] = GetTickCount();
                    }
                }
                else
                {
                    float fDistance = pVehicle->GetDistanceFromLocalPlayerPed();
                    if (pVehicle->GetVehicleSubtype() != VEHICLE_SUBTYPE_BOAT &&
                        fDistance < 200.0f &&
                        pVehicle->HasSunk()) {
                        NotifyVehicleDeath(VehicleID);
                    } else {
                        if ((GetTickCount() - m_dwLastUndrivenProcessTick[VehicleID]) > 100 &&
                            byteSentUndrivenSync < 3 &&
                            pLocalPlayer && pLocalPlayer->ProcessUnoccupiedSync(VehicleID,
                                                                                m_pVehicles[VehicleID])) {
                            m_lastUndrivenId[VehicleID] = pNetGame->GetPlayerPool()->GetLocalPlayerID();
                            m_dwLastUndrivenProcessTick[VehicleID] = GetTickCount();
                            byteSentUndrivenSync++;
                        }

                        if (pNetGame->m_pNetSet->bManualVehicleEngineAndLight) {
                            pVehicle->ApplyEngineState(pVehicle->GetEngineState());
                            pVehicle->ApplyLightState(pVehicle->GetLightState());
                        } else {
                            if (pVehicle->GetEngineState() == -1) {
                                if (!pVehicle->IsDriverLocalPlayer())
                                    pVehicle->ApplyEngineState(0);
                                else
                                    pVehicle->ApplyEngineState(1);
                            } else if (pVehicle->GetEngineState() != -1) {
                                pVehicle->ApplyEngineState(pVehicle->GetEngineState());
                            }

                            pVehicle->ApplyLightState(pVehicle->GetLightState());
                        }

                        if (pVehicle->m_pVehicle != m_pGTAVehicles[VehicleID])
                            m_pGTAVehicles[VehicleID] = pVehicle->m_pVehicle;

                        //ProcessColors();
                        pVehicle->UpdateLastDrivenTime();
                        pVehicle->UpdateColor();
                        pVehicle->ProcessMarkers();
                    }
                }
            }
        }
    }
}
/*if((GetTickCount() - m_dwLastUndrivenProcessTick[x]) < 100 &&
					byteSentUndrivenSync < 3 &&
					pLocalPlayer && pLocalPlayer->ProcessUnoccupiedSync(x, m_pVehicles[x]))
				{
					m_lastUndrivenId[x] = pPlayerPool->GetLocalPlayerID();
					m_dwLastUndrivenProcessTick[x] = GetTickCount();
					byteSentUndrivenSync++;
				}*/
// 0.3.7
void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
    RakNet::BitStream bsDeath;
    bsDeath.Write(VehicleID);
    pNetGame->GetRakClient()->RPC(&RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(pPlayerPool)
    {
        CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
        if(pLocalPlayer)
            pLocalPlayer->m_LastVehicle = INVALID_VEHICLE_ID;
    }
}