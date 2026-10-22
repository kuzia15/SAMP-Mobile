#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../vendor/armhook/patch.h"
#include "World.h"
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
            if(pPlayerPed->IsInJetpackMode()) {
                uint16_t wAccelerate = CPad__GetAccelerate(thiz);
                LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
                return wAccelerate;
            }

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
                CVehicleGTA* pGtaVehicle = pPlayerPed->GetGtaVehicle();
                if (pGtaVehicle)
                {
                    if (pGtaVehicle->m_nVehicleFlags.bEngineOn == 0)
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
                CVehicleGTA* pGtaVehicle = pPlayerPed->GetGtaVehicle();
                if (pGtaVehicle)
                {
                    if (pGtaVehicle->m_nVehicleFlags.bEngineOn == 0)
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
        uint8_t old =   *pbyteCurrentPlayer;
          *pbyteCurrentPlayer = byteCurPlayer;
        uintptr_t result = CPad__GetEnterTargeting(thiz);
        LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result;
          *pbyteCurrentPlayer = old;
        return result;
    }
}

bool bWeaponClicked;

uint32_t (*CPad__CycleWeaponRightJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponRightJustDown_hook(uintptr_t thiz)
{
    if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;

    if(!bWeaponClicked) {
        return 0;
    }
    else {
        bWeaponClicked = false;
        return 1;
    }
    return CPad__CycleWeaponRightJustDown(thiz);
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

bool (*CPad__GetWeapon)(uintptr_t thiz, CPedGTA* pPed);
bool CPad__GetWeapon_hook(uintptr_t thiz, CPedGTA* pPed)
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

uint32_t(*CCamera_IsTargetingActive)(uintptr_t thiz, CPedGTA* pPed);
uint32_t CCamera_IsTargetingActive_hook(uintptr_t thiz, CPedGTA* pPed)
{
    if (pPed != GamePool_FindPlayerPed())
    {
        return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 1 : 0;
    }
    else
    {
        /* CCamera::IsTargetingActive */
        static CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + 0x9F86F8);
        uint32_t bIsTargeting = CHook::CallFunction<bool>(g_libGTASA + 0x46DE58, &TheCamera);

        LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = bIsTargeting;
        return bIsTargeting;
    }
}

uint32_t(*CPad__GetDisplayVitalStats)(uint32_t thiz);
uint32_t CPad__GetDisplayVitalStats_hook(uint32_t thiz)
{
    uint32_t result = CPad__GetDisplayVitalStats(thiz);

    if (pUI) {
//         if (result) pUI->playertablist()->show();
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
      if (*pbyteCurrentPlayer)
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
      if (*pbyteCurrentPlayer)
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
    if (*pbyteCurrentPlayer)
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
      if (*pbyteCurrentPlayer)
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
      if (*pbyteCurrentPlayer)
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
      if (*pbyteCurrentPlayer)
    {
        return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
    }
    else
    {
        LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__GetTurretLeft(thiz);
        return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
    }
}

extern uint8_t byteInternalPlayer;
void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
    auto* pVehicle = (CVehicleGTA*)thiz;
    uintptr_t this_vtable = *(uintptr_t*)pVehicle;
    this_vtable -= g_libGTASA;

    uintptr_t call_addr = 0;

    switch(this_vtable)
    {

        // CAutomobile
        case 0x8304C8:
            call_addr = 0x6A77B8;
            break;

            // CBoat
        case 0x830C78:
            call_addr = 0x6C180C;
            break;

            // CBike
        case 0x8307E8:
            call_addr = 0x6B6954;
            break;

            // CPlane
        case 0x831388:
            call_addr = 0x6CC53C;
            break;

            // CHeli
        case 0x830EB8:
            call_addr = 0x6C79EC;
            break;

            // CBmx
        case 0x830A30:
            call_addr = 0x6BE2A8;
            break;

            // CMonsterTruck
        case 0x831120:
            call_addr = 0x6CB314;
            break;

            // CQuadBike
        case 0x8315F0:
            call_addr = 0x6D1098;
            break;

            // CTrain
        case 0x831AC0:
            call_addr = 0x6D4224;
            break;
    }

    uint8_t saved_focus =   *pbyteCurrentPlayer;
    uint8_t saved_cur_player = byteCurPlayer;

    if (pVehicle && pVehicle->pDriver) {
        byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);
          *pbyteCurrentPlayer = byteCurPlayer;
    } else {
        byteCurPlayer = 0;
          *pbyteCurrentPlayer = 0;
    }

    if(pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 &&
       pVehicle->pDriver != GamePool_FindPlayerPed())
    {
        pVehicle->pDriver->m_nPedType = static_cast<ePedType>(4);
        (( void (*)(uintptr_t))(g_libGTASA + 0x397150))(reinterpret_cast<uintptr_t>(&pVehicle->m_VehicleAudioEntity));
        pVehicle->pDriver->m_nPedType = static_cast<ePedType>(0);
    }
    else
    {
        (( void (*)(uintptr_t))(g_libGTASA + 0x397150))(reinterpret_cast<uintptr_t>(&pVehicle->m_VehicleAudioEntity));
    }

    // Tyre burst fix
    if (pVehicle->pDriver)
    {
        if (pVehicle->m_nVehicleFlags.bTyresDontBurst)
        {
            pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
        }
        if(!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
    }
    else
    {
        if (!pVehicle->m_nVehicleFlags.bTyresDontBurst)
        {
            pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
        }
        if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
    }

    // VEHTYPE::ProcessControl()
    (( void (*)(CVehicleGTA*))(g_libGTASA + call_addr))(pVehicle);

      *pbyteCurrentPlayer = saved_focus;
    byteCurPlayer = saved_cur_player;
}

extern float * pfCameraExtZoom;
void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_hook(uintptr_t thiz)
{
    dwCurPlayerActor = reinterpret_cast<uintptr_t>(thiz);
    byteCurPlayer = FindPlayerNumFromPedPtr(reinterpret_cast<CPedGTA *>(dwCurPlayerActor));

    if (dwCurPlayerActor && (byteCurPlayer != 0))
    {
        // REMOTE PLAYER
        uint16_t byteSavedCameraMode;
        byteSavedCameraMode = *pbyteCameraMode;
        *pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

        // aim switching
        GameStoreLocalPlayerAim();
        GameSetRemotePlayerAim(byteCurPlayer);

        GameStoreLocalPlayerCameraExtZoomAndAspect();
        GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);


        uint16_t wSavedCameraMode2 = *wCameraMode2;
        *wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
        if (*wCameraMode2 == 4)
            *wCameraMode2 = 0;

        // CPed::UpdatePosition nulled from CPed::ProcessControl
        CHook::NOP(g_libGTASA + 0x58AE24, 2);

          *pbyteCurrentPlayer = byteCurPlayer;
        // call original

        CPed__ProcessControl(thiz);

        // restore
        CHook::WriteMemory(g_libGTASA + 0x58AE24, "\xAF\x00\x0A\x94", 4);


          *pbyteCurrentPlayer = 0;
        *pbyteCameraMode = byteSavedCameraMode;

        GameSetLocalPlayerCameraExtZoomAndAspect();
        GameSetLocalPlayerAim();
        *wCameraMode2 = wSavedCameraMode2;
    }
    else
    {
        CPed__ProcessControl(thiz);
    }
}

uint32_t TaskUseGun(uintptr_t thiz, uintptr_t ped)
{
    dwCurPlayerActor = ped;
    byteCurPlayer = FindPlayerNumFromPedPtr(reinterpret_cast<CPedGTA *>(dwCurPlayerActor));

    uint32_t result = 0;

    if (dwCurPlayerActor &&
        (byteCurPlayer != 0)) // not local player and local player's keys set.
    {
        uint16_t byteSavedCameraMode = *pbyteCameraMode;
        *pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

        uint16_t wSavedCameraMode2 = *wCameraMode2;
        *wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
        if (*wCameraMode2 == 4)* wCameraMode2 = 0;

        // save the camera zoom factor, apply the context
        GameStoreLocalPlayerCameraExtZoomAndAspect();
        GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);

        // aim switching
        GameStoreLocalPlayerAim();
        GameSetRemotePlayerAim(byteCurPlayer);
        *pbyteCurrentPlayer = byteCurPlayer;
        result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x616C0C))(thiz, ped);

        // restore the camera modes, internal id and local player's aim
        *pbyteCameraMode = byteSavedCameraMode;

        // remote the local player's camera zoom factor
        GameSetLocalPlayerCameraExtZoomAndAspect();

        *pbyteCurrentPlayer = 0;
        GameSetLocalPlayerAim();
        *wCameraMode2 = wSavedCameraMode2;
    }
    else
    {
        result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x616C0C))(thiz, ped);
    }

    return result;
}

uint32_t CPad__TaskProcess(uintptr_t thiz, uintptr_t ped, int unk, int unk1)
{
    dwCurPlayerActor = ped;
    byteCurPlayer = FindPlayerNumFromPedPtr(reinterpret_cast<CPedGTA *>(dwCurPlayerActor));
    uint8_t old = *pbyteCurrentPlayer ;
    *pbyteCurrentPlayer  = byteCurPlayer;
    uint32_t result =  ((uint32_t(*)(uintptr_t, uintptr_t, int, int))(g_libGTASA + 0x68BD84))(thiz, ped, unk, unk1);
    *pbyteCurrentPlayer  = old;
    return result;
}
uint32_t (*CPed__GetWeaponSkill)(CPedGTA *ped);
uint32_t CPed__GetWeaponSkill_hook(CPedGTA *ped)
{
    PED_TYPE * thiz = (PED_TYPE *)ped;
    bool bWeaponSkillStored = false;

    dwCurPlayerActor = reinterpret_cast<uintptr_t>(ped);
    byteInternalPlayer = *pbyteCurrentPlayer;
    byteCurPlayer = FindPlayerNumFromPedPtr(reinterpret_cast<CPedGTA *>(dwCurPlayerActor));

    if(dwCurPlayerActor && byteCurPlayer != 0 && byteInternalPlayer == 0)
    {
        GameStoreLocalPlayerSkills();
        GameSetRemotePlayerSkills(byteCurPlayer);
        bWeaponSkillStored = true;
    }
    // CPed::GetWeaponSkill
    uint32_t result = (( uint32_t (*)(CPedGTA *, uint32_t))(g_libGTASA+0x58E358))(ped, ped->m_aWeapons[ped->m_nActiveWeaponSlot].dwType);

    if(bWeaponSkillStored)
    {
        GameSetLocalPlayerSkills();
        bWeaponSkillStored = false;
    }
    return result;
}
void HookCPad()
{
    memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));
  //  CHook::CallFunction<void>(g_libGTASA + 0x54F0DC);
    // CPedSamp::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x825E28, &CPed__ProcessControl_hook, &CPed__ProcessControl);

    // all vehicles ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x830520, &AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x830CD0, &AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x830840, &AllVehicles__ProcessControl_hook); // CBike::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x8313E0, &AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x830F10, &AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x830A88, &AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x831178, &AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x831648, &AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
    CHook::InstallPLT(g_libGTASA + 0x831B18, &AllVehicles__ProcessControl_hook); // CTrain::ProcessControl

    CHook::InstallPLT(g_libGTASA + 0x826A88, &TaskUseGun);
    CHook::InstallPLT(g_libGTASA + 0x82F5C0, &CPad__TaskProcess);

    // lr/ud (onfoot)
    CHook::InlineHook("_ZN4CPad19GetPedWalkLeftRightEv", &CPad__GetPedWalkLeftRight_hook, &CPad__GetPedWalkLeftRight);
    CHook::InlineHook("_ZN4CPad16GetPedWalkUpDownEv", &CPad__GetPedWalkUpDown_hook, &CPad__GetPedWalkUpDown);

    CHook::InlineHook("_ZN4CPad9GetSprintEi", &CPad__GetSprint_hook, &CPad__GetSprint);
    CHook::InlineHook("_ZN4CPad12JumpJustDownEv", &CPad__JumpJustDown_hook, &CPad__JumpJustDown);
    CHook::InlineHook("_ZN4CPad7GetJumpEv", &CPad__GetJump_hook, &CPad__GetJump);
    CHook::InlineHook("_ZN4CPad15GetCarGunUpDownEbP11CAutomobilefb", &CPad__GetCarGunUpDown_hook, &CPad__GetCarGunUpDown);
    CHook::InlineHook("_ZN4CPad12GetAutoClimbEv", &CPad__GetAutoClimb_hook, &CPad__GetAutoClimb);
    CHook::InlineHook("_ZN4CPad13GetAbortClimbEv", &CPad__GetAbortClimb_hook, &CPad__GetAbortClimb);

    // swimm
    CHook::InlineHook("_ZN4CPad12DiveJustDownEv", &CPad__DiveJustDown_hook, &CPad__DiveJustDown);
    CHook::InlineHook("_ZN4CPad16SwimJumpJustDownEv", &CPad__SwimJumpJustDown_hook, &CPad__SwimJumpJustDown);

    CHook::InlineHook("_ZN4CPad19MeleeAttackJustDownEv", &CPad__MeleeAttackJustDown_hook, &CPad__MeleeAttackJustDown);
    CHook::InlineHook("_ZN4CPad12DuckJustDownEP4CPed", &CPad__DuckJustDown_hook, &CPad__DuckJustDown);
    CHook::InlineHook("_ZN4CPad8GetBlockEv", &CPad__GetBlock_hook, &CPad__GetBlock);

    // steering lr/ud (incar)
    CHook::InlineHook("_ZN4CPad20GetSteeringLeftRightEv", &CPad__GetSteeringLeftRight_hook, &CPad__GetSteeringLeftRight);
    CHook::InlineHook("_ZN4CPad17GetSteeringUpDownEv", &CPad__GetSteeringUpDown_hook, &CPad__GetSteeringUpDown);

    CHook::InlineHook("_ZN4CPad13GetAccelerateEv", &CPad__GetAccelerate_hook, &CPad__GetAccelerate);
    CHook::InlineHook("_ZN4CPad8GetBrakeEv", &CPad__GetBrake_hook, &CPad__GetBrake);
    CHook::InlineHook("_ZN4CPad12GetHandBrakeEv", &CPad__GetHandBrake_hook, &CPad__GetHandBrake);
    CHook::InlineHook("_ZN4CPad7GetHornEb", &CPad__GetHorn_hook, &CPad__GetHorn);

    //CHook::InstallPLT(g_libGTASA + 0x66EB90, (uintptr_t)CPad__ExitVehicleJustDown_hook, (uintptr_t*)& CPad__ExitVehicleJustDown);
    //CHook::InstallPLT(g_libGTASA + 0x672440, (uintptr_t)CPad__GetExitVehicle_hook, (uintptr_t*)&CPad__GetExitVehicle);

    //CHook::InstallPLT(g_libGTASA + 0x675394, (uintptr_t)CPad__GetDisplayVitalStats_hook, (uintptr_t*)&CPad__GetDisplayVitalStats);
    //����ҧ������ͧ�ѹ��ѧ
    //CHook::InstallPLT(g_libGTASA + 0x67063C, (uintptr_t)CPad__GetLookBehindForPed_hook, (uintptr_t*)&CPad__GetLookBehindForPed);

    // WEAPON
    CHook::InlineHook("_ZN4CPad17GetEnterTargetingEv", &CPad__GetEnterTargeting_hook, &CPad__GetEnterTargeting);
    CHook::InlineHook("_ZN4CPad9GetWeaponEP4CPedb", &CPad__GetWeapon_hook, &CPad__GetWeapon);
    CHook::InlineHook("_ZN7CCamera17IsTargetingActiveEP10CPlayerPed", &CCamera_IsTargetingActive_hook, &CCamera_IsTargetingActive);
    CHook::InlineHook("_ZN4CPad24CycleWeaponRightJustDownEv", &CPad__CycleWeaponRightJustDown_hook, &CPad__CycleWeaponRightJustDown);
    //CHook::InstallPLT(g_libGTASA + 0x66F304, (uintptr_t)CPad__CycleWeaponLeftJustDown_hook, (uintptr_t*)&CPad__CycleWeaponLeftJustDown);

    //
    CHook::InstallPLT(g_libGTASA + 0x83F190, &CPed__GetWeaponSkill_hook, &CPed__GetWeaponSkill);


    // nitro
    CHook::InlineHook("_ZN4CPad13GetNitroFiredEv", &CPad__GetNitroFired_hook, &CPad__GetNitroFired);

    //CHook::InstallPLT(g_libGTASA + 0x67324C, (uintptr_t)CPad__GetLookLeft_hook, (uintptr_t*)&CPad__GetLookLeft);
    //CHook::InstallPLT(g_libGTASA + 0x67205C, (uintptr_t)CPad__GetLookRight_hook, (uintptr_t*)&CPad__GetLookRight);

    //CHook::InstallPLT(g_libGTASA + 0x674418, (uintptr_t)CPad__GetCarGunLeftRight_hook,(uintptr_t*)&CPad__GetCarGunLeftRight);
    //CHook::InstallPLT(g_libGTASA + 0x675ABC, (uintptr_t)CPad__GetCarGunFired_hook, (uintptr_t*)&CPad__GetCarGunFired);

    CHook::InlineHook("_ZN4CPad13GetTurretLeftEv", &CPad__GetTurretLeft_hook, &CPad__GetTurretLeft);
    CHook::InlineHook("_ZN4CPad14GetTurretRightEv", &CPad__GetTurretRight_hook, &CPad__GetTurretRight);
}