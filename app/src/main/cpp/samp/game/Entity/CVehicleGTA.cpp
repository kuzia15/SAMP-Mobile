//
// Created by roman on 11/24/2024.
//

#include "CVehicleGTA.h"
#include "patch.h"
#include "game/Enums/ePedState.h"
#include "game/RegisteredCorona.h"
#include "game/Coronas.h"
#include "game/Models/ModelInfo.h"
#include "game/Entity/CPedGTA.h"
#include "game/Entity/CEntityGTA.h"

void CVehicleGTA::RenderDriverAndPassengers() {
    if(IsRCVehicleModelID())
        return;

    if (pDriver && pDriver->m_nPedState == PEDSTATE_DRIVING) {
        CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4A6964 + 1 : 0x59D3B8), pDriver);
        // pDriver->Render();
    }

    for (auto& passenger : m_apPassengers) {
        if (passenger && passenger->m_nPedState == PEDSTATE_DRIVING) {
            CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4A6964 + 1 : 0x59D3B8), passenger);
            // passenger->Render();
        }
    }
}

void CVehicleGTA::SetDriver(CPedGTA* driver) {
    CEntityGTA::ChangeEntityReference(pDriver, driver);

    ApplyTurnForceToOccupantOnEntry(driver);
}

bool CVehicleGTA::AddPassenger(CPedGTA* passenger) {
    ApplyTurnForceToOccupantOnEntry(passenger);

    // Now, find a seat and place them into it
    const auto seats = GetMaxPassengerSeats();

    for(auto & emptySeat : m_apPassengers) {
        emptySeat = passenger;
        CEntityGTA::RegisterReference(emptySeat);
        m_nNumPassengers++;
        return false;
    }

    // No empty seats
    return false;
}

bool CVehicleGTA::AddPassenger(CPedGTA* passenger, uint8 seatIdx) {
    if (m_nVehicleFlags.bIsBus) {
        return AddPassenger(passenger);
    }

    // Check if seat is valid
    if (seatIdx >= m_nMaxPassengers) {
        return false;
    }

    // Check if anyone is already in that seat
    if (m_apPassengers[seatIdx]) {
        return false;
    }

    // Place passenger into seat, and add ref
    m_apPassengers[seatIdx] = passenger;
    CEntityGTA::RegisterReference(m_apPassengers[seatIdx]);
    m_nNumPassengers++;

    return true;
}

void CVehicleGTA::ApplyTurnForceToOccupantOnEntry(CPedGTA* passenger) {
    // Apply some turn force
    switch (m_nVehicleType) {
        case VEHICLE_TYPE_BIKE: {
            ApplyTurnForce(
                    GetUp() * passenger->m_fMass / -50.f,
                    GetForward() / -10.f // Behind the bike
            );
            break;
        }
        default: {
            ApplyTurnForce(
                    CVector{ .0f, .0f, passenger->m_fMass / -5.f },
                    CVector{ CVector2D{passenger->GetPosition() - GetPosition()}, 0.f }
            );
            break;
        }
    }
}

int CVehicleGTA::GetPassengerIndex(const CPedGTA* passenger) {
    for(int i = 0; i <  std::size(m_apPassengers); i++) {
        if(passenger == m_apPassengers[i])
            return i;
    }
    return -1;
}

void CVehicleGTA::AddVehicleUpgrade(int32 modelId) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0058C66C + 1 : 0x6AFF4C), this, modelId);
}

void CVehicleGTA::RemoveVehicleUpgrade(int32 upgradeModelIndex) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x58CC2C + 1 : 0x6B0718), this, upgradeModelIndex);
}

// 0x6D3000
void CVehicleGTA::SetGettingInFlags(uint8 doorId) {
    m_nGettingInFlags |= doorId;
}

// 0x6D3020
void CVehicleGTA::SetGettingOutFlags(uint8 doorId) {
    m_nGettingOutFlags |= doorId;
}

// 0x6D3040
void CVehicleGTA::ClearGettingInFlags(uint8 doorId) {
    m_nGettingInFlags &= ~doorId;
}

// 0x6D3060
void CVehicleGTA::ClearGettingOutFlags(uint8 doorId) {
    m_nGettingOutFlags &= ~doorId;
}

// ----------------------------------- hooks

void RenderDriverAndPassengers_hook(CVehicleGTA *thiz)
{
    thiz->RenderDriverAndPassengers();
}

void SetDriver_hook(CVehicleGTA *thiz, CPedGTA *pPed)
{
    thiz->SetDriver(pPed);
}

bool CVehicle__GetVehicleLightsStatus_hook(CVehicleGTA *pVehicle)
{
    return pVehicle->GetLightsStatus();
}

void (*CVehicle__DoVehicleLights)(CVehicleGTA* thiz, CMatrix *matVehicle, uint32 nLightFlags);
void CVehicle__DoVehicleLights_hook(CVehicleGTA* thiz, CMatrix *matVehicle, uint32 nLightFlags)
{
    uint8_t old = thiz->m_nVehicleFlags.bEngineOn;
    thiz->m_nVehicleFlags.bEngineOn = 1;
    CVehicle__DoVehicleLights(thiz, matVehicle, nLightFlags);
    thiz->m_nVehicleFlags.bEngineOn = old;
}

bool CVehicle__DoTailLightEffect(CVehicleGTA* thisVehicle, int32_t lightId, CMatrix* matVehicle, int isRight, int forcedOff, uint32_t nLightFlags, int lightsOn) {

    constexpr int REVERSE_LIGHT_OFFSET = 5;

    auto pModelInfoStart = CModelInfo::GetVehicleModelInfo(thisVehicle->m_nModelIndex);

    CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    auto v = CVector(m_avDummyPos[1]);

    if (!isRight)
        v.x = -v.x;

    uint8_t alpha = (thisVehicle->m_fBreakPedal > 0) ? 200 : 96;
    if (thisVehicle->GetLightsStatus() || (thisVehicle->m_fBreakPedal > 0 && thisVehicle->pDriver)) {
        CCoronas::RegisterCorona(
                (uintptr) &thisVehicle->m_placement.m_vPosn.y + 2 * lightId + isRight,
                thisVehicle,
                100, 0, 0, alpha,
                &v,
                0.65f,
                /*TheCamera.LODDistMultiplier*/ 70.f,
                eCoronaType::CORONATYPE_HEADLIGHT,
                eCoronaFlareType::FLARETYPE_NONE,
                false,
                false,
                0,
                0.0f,
                false,
                0,
                0,
                15.0f,
                false,
                false
        );
    }

    if(thisVehicle->m_nCurrentGear == 0 && thisVehicle->m_pHandlingData->m_transmissionData.m_fCurrentSpeed < -0.01) {
        CCoronas::RegisterCorona(
                (uintptr) &thisVehicle->m_placement.m_vPosn.y + 2 * lightId + isRight + REVERSE_LIGHT_OFFSET,
                thisVehicle,
                255, 255, 255, 200,
                &v,
                0.70f,
                /*TheCamera.LODDistMultiplier*/ 70.f,
                eCoronaType::CORONATYPE_HEADLIGHT,
                eCoronaFlareType::FLARETYPE_NONE,
                false,
                false,
                0,
                0.0f,
                false,
                0,
                0,
                15.0f,
                false,
                false
        );
    }
    return true;
}

void CVehicleGTA::InjectHooks() {
    // var
    CHook::Write(g_libGTASA + (VER_x32 ? 0x675F10 : 0x849EA8), &CVehicleGTA::m_aSpecialColModel);

    CHook::Redirect("_ZN8CVehicle25RenderDriverAndPassengersEv", &RenderDriverAndPassengers_hook);
    CHook::Redirect("_ZN8CVehicle9SetDriverEP4CPed", &SetDriver_hook);

    CHook::Redirect("_ZN8CVehicle17DoTailLightEffectEiR7CMatrixhhjh", &CVehicle__DoTailLightEffect);
    CHook::InlineHook("_ZN8CVehicle15DoVehicleLightsER7CMatrixj", &CVehicle__DoVehicleLights_hook, &CVehicle__DoVehicleLights);
    CHook::Redirect("_ZN8CVehicle22GetVehicleLightsStatusEv", &CVehicle__GetVehicleLightsStatus_hook);
}

bool CVehicleGTA::IsRCVehicleModelID() {
    switch (m_nModelIndex) {
        case 441:
        case 464:
        case 465:
        case 594:
        case 501:
        case 564:
            return true;

        default:
            break;
    }
    return false;
}