#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../vendor/armhook/patch.h"

extern UI* pUI;
extern CGame* pGame;
extern CNetGame* pNetGame;

extern uint8_t byteCurPlayer;
extern uintptr_t dwCurPlayerActor;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uint16_t(*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
uint16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if ((dwResult == 0xFF80 || dwResult == 0x80) &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x20;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyLR = CPad__GetPedWalkLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t(*CPad__GetPedWalkUpDown)(uintptr_t thiz);
uint16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if ((dwResult == 0xFF80 || dwResult == 0x80) &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x20;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyUD = CPad__GetPedWalkUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint32_t(*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{
	if (*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t(*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		if (!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t(*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		if (RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t(*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t(*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t(*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t(*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t(*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if (*pbyteCurrentPlayer)
	{
		return 0;
	}
	else
	{
		return CPad__DuckJustDown(thiz, unk);
	}
}

uint32_t(*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{
	/*
		0 - �� ����
		1 - ������� ���� (���)
		2 - ������� ���� (��� + F)
	*/

	if (*pbyteCurrentPlayer)
	{
		if (RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK])
			return 2;

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;

		//if(dwResult == 2) 
		//{
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		//}
		//else if(dwResult == 1)
		//{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = false;
		//}

		return dwResult;
	}
}

uint32_t(*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		if (RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

int16_t(*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurPlayer].wKeyLR;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyLR = CPad__GetSteeringLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t(*CPad__GetSteeringUpDown)(uintptr_t thiz);
uint16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].wKeyUD;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyUD = CPad__GetSteeringUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint16_t(*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		// local player
		uint16_t wAccelerate = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
		if (wAccelerate == 0xFF)
		{
			if (pPlayerPed)
			{
				VEHICLE_TYPE* pGtaVehicle = pPlayerPed->GetGtaVehicle();
				if (pGtaVehicle)
				{
					if (pGtaVehicle->dwFlags.bEngineOn == 0)
					{
						wAccelerate = 0;
					}
				}
			}
		}

		return wAccelerate;
	}
}

uint16_t(*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
        CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
        if (pPlayerPed)
        {
            if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
                return 0;
        }

		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
        if (wBrake == 0xFF)
        {
            if (pPlayerPed)
            {
                VEHICLE_TYPE* pGtaVehicle = pPlayerPed->GetGtaVehicle();
                if (pGtaVehicle)
                {
                    if (pGtaVehicle->dwFlags.bEngineOn == 0)
                    {
                        wBrake = 0;
                    }
                }
            }
        }
		return wBrake;
	}
}

uint32_t(*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

uint32_t(*CPad__GetHorn)(uintptr_t thiz);
uint32_t CPad__GetHorn_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		// local player
		uint32_t horn = CPad__GetHorn(thiz);
		//Log("horn: %d", horn);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

/*extern bool g_bLockEnterVehicleWidget;
extern bool g_bForceEnterVehicle;
uint32_t(*CPad__ExitVehicleJustDown)(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec);
uint32_t CPad__ExitVehicleJustDown_hook(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	int result = CPad__ExitVehicleJustDown(thiz, a2, vehicle, a4, vec);

	if (g_bForceEnterVehicle)
	{
		g_bForceEnterVehicle = false;
		return true;
	}

	if (g_bLockEnterVehicleWidget) return false;

	return result;
}
*/

uint32_t(*CPad__ExitVehicleJustDown)(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec);
uint32_t CPad__ExitVehicleJustDown_hook(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	static uint32_t dwPassengerEnterExit = GetTickCount();

	if (GetTickCount() - dwPassengerEnterExit < 1000)
		return 0;

	if (pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool)
		{
			CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
			if (pLocalPlayer) {
				if (pLocalPlayer->HandlePassengerEntry())
				{
					dwPassengerEnterExit = GetTickCount();
					return 0;
				}
			}
		}
	}

	return CPad__ExitVehicleJustDown(thiz, a2, vehicle, a4, vec);
}

uint32_t(*CPad__GetExitVehicle)(uintptr_t thiz);
uint32_t CPad__GetExitVehicle_hook(uintptr_t thiz)
{
    return 0;
}


/* Weapons */

bool (*CPad__GetEnterTargeting)(uintptr_t thiz);
bool CPad__GetEnterTargeting_hook(uintptr_t thiz)
{
    if (*pbyteCurrentPlayer)
    {
        return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];
    }
    else
    {
        uint8_t old = *pbyteCurrentPlayer;
        *pbyteCurrentPlayer = byteCurPlayer;
        uintptr_t result = CPad__GetEnterTargeting(thiz);
        LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result;
        *pbyteCurrentPlayer = old;
        return result;
    }
}

uint32_t(*CPad__CycleWeaponRightJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponRightJustDown_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		return 0;
	}
	else
	{
		return CPad__CycleWeaponRightJustDown(thiz);
	}
}

uint32_t(*CPad__CycleWeaponLeftJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponLeftJustDown_hook(uintptr_t thiz)
{
	if (*pbyteCurrentPlayer)
	{
		return 0;
	}
	else
	{
		return CPad__CycleWeaponLeftJustDown(thiz);
	}
}

bool (*CPad__GetWeapon)(uintptr_t thiz, PED_TYPE* pPed);
bool CPad__GetWeapon_hook(uintptr_t thiz, PED_TYPE* pPed)
{
	if (*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetWeapon(thiz, pPed);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t(*CCamera_IsTargetingActive)(uintptr_t thiz, PED_TYPE* pPed);
uint32_t CCamera_IsTargetingActive_hook(uintptr_t thiz, PED_TYPE* pPed)
{
	if (pPed != GamePool_FindPlayerPed())
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 1 : 0;
	}
	else
	{
		/* CCamera::IsTargetingActive */
		uint32_t bIsTargeting = ((uint32_t (*)(uintptr_t))(g_libGTASA + 0x3E157C + 1))(g_libGTASA + 0x951FA8);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = bIsTargeting;
		return bIsTargeting;
	}
}

uint32_t(*CPad__GetDisplayVitalStats)(uint32_t thiz);
uint32_t CPad__GetDisplayVitalStats_hook(uint32_t thiz)
{
	uint32_t result = CPad__GetDisplayVitalStats(thiz);

	if (pUI) {
		if (result) pUI->playertablist()->show();
	}

	return 0;
}

uint32_t(*CPad__GetLookBehindForPed)(uint32_t thiz);
uint32_t CPad__GetLookBehindForPed_hook(uint32_t thiz)
{
	uint32_t result = CPad__GetLookBehindForPed(thiz);

	VoiceButton* vbutton = pUI->voicebutton();
	if (vbutton->countdown > 50) return 0;

	//return 0;
}

int (*CPad__GetNitroFired)(uintptr_t thiz);
int CPad__GetNitroFired_hook(uintptr_t thiz)
{
    if(*pbyteCurrentPlayer)
    {
        if(RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE])
            return 1;
    }
    else
    {
        LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetNitroFired(thiz);
        return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
    }
}

uint32_t (*CPad__GetLookLeft)(uintptr_t thiz);
uint32_t CPad__GetLookLeft_hook(uintptr_t thiz)
{
    if(*pbyteCurrentPlayer)
    {
        if(RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE])
            return 1;
    }
    else
    {
        LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetLookLeft(thiz);
        return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
    }
}

uint32_t (*CPad__GetLookRight)(uintptr_t thiz);
uint32_t CPad__GetLookRight_hook(uintptr_t thiz)
{
    if(*pbyteCurrentPlayer)
    {
        if(RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE])
            return 1;
    }
    else
    {
        LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetLookRight(thiz);
        return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
    }
}

uint16_t(*CPad__GetCarGunLeftRight)(unsigned int a1, int a2, int a3);
uint16_t CPad__GetCarGunLeftRight_hook(unsigned int a1, int a2, int a3)
{
	if (*pbyteCurrentPlayer)
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if (RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE])
		{
			dwResult = 0xFFFFFF80;
		}
		return dwResult;
	}
	else
	{
		// Local player
		uint16_t dwResult = CPad__GetCarGunLeftRight(a1, a2, a3);

		if ( dwResult == 0x80 )
		{
			LocalPlayerKeys.wKeyLR = 1;
			dwResult = 0x80;
		}
		else if ( dwResult == 0xFFFFFF80 )
		{
			LocalPlayerKeys.wKeyLR = 1;
			dwResult = 0xFFFFFF80;
		}
		else
		{
			LocalPlayerKeys.wKeyLR = 0;
		}

		return dwResult;
	}
}

uint16_t(*CPad__GetCarGunUpDown)(unsigned int a1, int a2, void *a3, float a4, int a5);
uint16_t CPad__GetCarGunUpDown_hook(unsigned int a1, int a2, void *a3, float a4, int a5)
{
	if (*pbyteCurrentPlayer)
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if (RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE])
		{
			dwResult = 0xFFFFFF80;
		}
		return dwResult;
	}
	else
	{
		// Local player
		uint16_t dwResult = CPad__GetCarGunUpDown(a1, a2, a3, a4, a5);

		if ( dwResult == 0x80 )
		{
			LocalPlayerKeys.wKeyUD = 1;
			dwResult = 0x80;
		}
		else if ( dwResult == 0xFFFFFF80 )
		{
			LocalPlayerKeys.wKeyUD = 1;
			dwResult = 0xFFFFFF80;
		}
		else
		{
			LocalPlayerKeys.wKeyUD = 0;
		}

		return dwResult;
	}
}

uint32_t (*CPad__GetCarGunFired)(uintptr_t thiz);
uint32_t CPad__GetCarGunFired_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetCarGunFired(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

bool (*CPad__GetTurretRight)(uintptr_t *thiz);
bool CPad__GetTurretRight_hook(uintptr_t *thiz)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__GetTurretRight(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
}

bool (*CPad__GetTurretLeft)(uintptr_t *thiz);
bool CPad__GetTurretLeft_hook(uintptr_t *thiz)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__GetTurretLeft(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	// lr/ud (onfoot)
	CHook::InstallPLT(g_libGTASA + 0x671014, (uintptr_t)CPad__GetPedWalkLeftRight_hook, (uintptr_t*)&CPad__GetPedWalkLeftRight);
	CHook::InstallPLT(g_libGTASA + 0x6706D0, (uintptr_t)CPad__GetPedWalkUpDown_hook, (uintptr_t*)&CPad__GetPedWalkUpDown);

	// sprint/jump stuff
	CHook::InstallPLT(g_libGTASA + 0x670CE0, (uintptr_t)CPad__GetSprint_hook, (uintptr_t*)&CPad__GetSprint);
	CHook::InstallPLT(g_libGTASA + 0x670274, (uintptr_t)CPad__JumpJustDown_hook, (uintptr_t*)& CPad__JumpJustDown);
	CHook::InstallPLT(g_libGTASA + 0x66FAE0, (uintptr_t)CPad__GetJump_hook, (uintptr_t*)& CPad__GetJump);
	CHook::InstallPLT(g_libGTASA + 0x674A0C, (uintptr_t)CPad__GetAutoClimb_hook, (uintptr_t*)& CPad__GetAutoClimb);
	CHook::InstallPLT(g_libGTASA + 0x6718D4, (uintptr_t)CPad__GetAbortClimb_hook, (uintptr_t*)& CPad__GetAbortClimb);

	// swimm
	CHook::InstallPLT(g_libGTASA + 0x672FD0, (uintptr_t)CPad__DiveJustDown_hook, (uintptr_t*)& CPad__DiveJustDown);
	CHook::InstallPLT(g_libGTASA + 0x674030, (uintptr_t)CPad__SwimJumpJustDown_hook, (uintptr_t*)& CPad__SwimJumpJustDown);

	CHook::InstallPLT(g_libGTASA + 0x67127C, (uintptr_t)CPad__MeleeAttackJustDown_hook, (uintptr_t*)& CPad__MeleeAttackJustDown);
	CHook::InstallPLT(g_libGTASA + 0x6727CC, (uintptr_t)CPad__DuckJustDown_hook, (uintptr_t*)& CPad__DuckJustDown);
	CHook::InstallPLT(g_libGTASA + 0x66FAD8, (uintptr_t)CPad__GetBlock_hook, (uintptr_t*)& CPad__GetBlock);

	// steering lr/ud (incar)
	CHook::InstallPLT(g_libGTASA + 0x673D84, (uintptr_t)CPad__GetSteeringLeftRight_hook, (uintptr_t*)& CPad__GetSteeringLeftRight);
	CHook::InstallPLT(g_libGTASA + 0x672C14, (uintptr_t)CPad__GetSteeringUpDown_hook, (uintptr_t*)& CPad__GetSteeringUpDown);

	CHook::InstallPLT(g_libGTASA + 0x67482C, (uintptr_t)CPad__GetAccelerate_hook, (uintptr_t*)& CPad__GetAccelerate);
	CHook::InstallPLT(g_libGTASA + 0x66EBE0, (uintptr_t)CPad__GetBrake_hook, (uintptr_t*)& CPad__GetBrake);
	CHook::InstallPLT(g_libGTASA + 0x670514, (uintptr_t)CPad__GetHandBrake_hook, (uintptr_t*)& CPad__GetHandBrake);
	CHook::InstallPLT(g_libGTASA + 0x673010, (uintptr_t)CPad__GetHorn_hook, (uintptr_t*)& CPad__GetHorn);
    CHook::InstallPLT(g_libGTASA + 0x674B54, (uintptr_t)CPad__GetHorn_hook);

	CHook::InstallPLT(g_libGTASA + 0x66EB90, (uintptr_t)CPad__ExitVehicleJustDown_hook, (uintptr_t*)& CPad__ExitVehicleJustDown);
	CHook::InstallPLT(g_libGTASA + 0x672440, (uintptr_t)CPad__GetExitVehicle_hook, (uintptr_t*)&CPad__GetExitVehicle);

	CHook::InstallPLT(g_libGTASA + 0x675394, (uintptr_t)CPad__GetDisplayVitalStats_hook, (uintptr_t*)&CPad__GetDisplayVitalStats);
	//����ҧ������ͧ�ѹ��ѧ
	CHook::InstallPLT(g_libGTASA + 0x67063C, (uintptr_t)CPad__GetLookBehindForPed_hook, (uintptr_t*)&CPad__GetLookBehindForPed);

	// WEAPON
	CHook::InstallPLT(g_libGTASA + 0x675260, (uintptr_t)CPad__GetEnterTargeting_hook, (uintptr_t*)&CPad__GetEnterTargeting);
	CHook::InstallPLT(g_libGTASA + 0x672E8C, (uintptr_t)CPad__GetWeapon_hook, (uintptr_t*)&CPad__GetWeapon);
	CHook::InstallPLT(g_libGTASA + 0x6708F0, (uintptr_t)CCamera_IsTargetingActive_hook, (uintptr_t*)&CCamera_IsTargetingActive);
	CHook::InstallPLT(g_libGTASA + 0x66FA0C, (uintptr_t)CPad__CycleWeaponRightJustDown_hook, (uintptr_t*)&CPad__CycleWeaponRightJustDown);
	CHook::InstallPLT(g_libGTASA + 0x66F304, (uintptr_t)CPad__CycleWeaponLeftJustDown_hook, (uintptr_t*)&CPad__CycleWeaponLeftJustDown);

    // nitro
    CHook::InstallPLT(g_libGTASA + 0x66FAF8, (uintptr_t)CPad__GetNitroFired_hook, (uintptr_t*)&CPad__GetNitroFired);

    CHook::InstallPLT(g_libGTASA + 0x67324C, (uintptr_t)CPad__GetLookLeft_hook, (uintptr_t*)&CPad__GetLookLeft);
    CHook::InstallPLT(g_libGTASA + 0x67205C, (uintptr_t)CPad__GetLookRight_hook, (uintptr_t*)&CPad__GetLookRight);

	CHook::InstallPLT(g_libGTASA + 0x674418, (uintptr_t)CPad__GetCarGunLeftRight_hook,(uintptr_t*)&CPad__GetCarGunLeftRight);
	CHook::InstallPLT(g_libGTASA + 0x674240, (uintptr_t)CPad__GetCarGunUpDown_hook, (uintptr_t*)&CPad__GetCarGunUpDown);
	CHook::InstallPLT(g_libGTASA + 0x675ABC, (uintptr_t)CPad__GetCarGunFired_hook, (uintptr_t*)&CPad__GetCarGunFired);

	CHook::InstallPLT(g_libGTASA + 0x671CDC, (uintptr_t)CPad__GetTurretLeft_hook, (uintptr_t*)&CPad__GetTurretLeft);
	CHook::InstallPLT(g_libGTASA + 0x672894, (uintptr_t)CPad__GetTurretRight_hook, (uintptr_t*)&CPad__GetTurretRight);
}