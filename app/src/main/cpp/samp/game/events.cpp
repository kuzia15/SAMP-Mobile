#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "Streaming.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

#define EVENT_TYPE_PAINTJOB			1
#define EVENT_TYPE_CARCOMPONENT		2
#define EVENT_TYPE_CARCOLOR			3
#define EVENT_ENTEREXIT_MODSHOP		4

extern bool bFirstSpawn;

void ProcessIncommingEvent(PLAYERID playerId, int iEventType,
	uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	if (pNetGame == nullptr) return;
	if (bFirstSpawn) return;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	CVehicle* pVehicle = nullptr;
	CRemotePlayer* pPlayer = nullptr;

    int iVehicleID;
    int iPaintJob;
    int iComponent;
    uint32_t v;

	switch (iEventType)
	{
	case EVENT_TYPE_PAINTJOB:
        iVehicleID = pNetGame->GetVehiclePool()->FindGtaIDFromID(dwParam1);
        iPaintJob = (int)dwParam2;
        if (iVehicleID) ScriptCommand(&change_car_skin, iVehicleID, dwParam2);
		break;

	case EVENT_TYPE_CARCOMPONENT:
        iVehicleID = pNetGame->GetVehiclePool()->FindGtaIDFromID(dwParam1);
        iComponent = (int)dwParam2;

        if(CStreaming::TryLoadModel(iComponent))
            ScriptCommand(&add_car_component, iVehicleID, iComponent, &v);
		break;

	case EVENT_TYPE_CARCOLOR:
		pVehicle = pVehiclePool->GetAt(dwParam1);
		if (pVehicle) {
			pVehicle->SetColor(dwParam2, dwParam3);
		}
		break;

	case EVENT_ENTEREXIT_MODSHOP:
		if (playerId > MAX_PLAYERS) return;

		pVehicle = pVehiclePool->GetAt(dwParam1);
		pPlayer = pPlayerPool->GetAt(playerId);

		if (pPlayer && pVehicle)
		{
			CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
			CPlayerPed* pLocalPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();

			if (pPlayerPed && pLocalPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					if (pLocalPlayerPed->IsInVehicle())
					{
						if (!pPlayerPed->IsAPassenger())
						{
							if (pLocalPlayerPed->IsAPassenger())
							{
								VEHICLEID VehicleID = pPlayerPed->GetCurrentVehicleID();
								if (VehicleID == pLocalPlayerPed->GetCurrentVehicleID())
								{
									CVehicle* pCurrentVehicle = pPlayer->m_pCurrentVehicle;

									if (dwParam2)
									{
										if (pCurrentVehicle) {
											pCurrentVehicle->LinkToInterior(dwParam3);
										}
										pLocalPlayerPed->m_pPed->SetInterior(dwParam3, false);
										pLocalPlayerPed->TogglePlayerControllable(false);
									}
									else
									{
										if (pCurrentVehicle) {
											pCurrentVehicle->LinkToInterior(0);
										}
										pLocalPlayerPed->m_pPed->SetInterior(0, false);
										pLocalPlayerPed->TogglePlayerControllable(true);
									}
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}