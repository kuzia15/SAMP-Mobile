//
// Created by x1y2z on 08.11.2023.
//

#include "Pickup.h"
#include "World.h"
#include "../vendor/armhook/patch.h"
#include "Camera.h"
#include "game.h"
#include "net/netgame.h"


// Delete pickup's object (CObject)
void CPickup::GetRidOfObjects() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0031D790 + 1 : 0x3E4A20), this);

    // Need destructor :c
//    if (m_pObject) {
//        CWorld::Remove(m_pObject);
//        delete m_pObject;
//        m_pObject = nullptr;
//    }
}

// Creates an object (CObject) for pickup. slotIndex - object to replace; use -1 (or any negative value) to create a new object
void CPickup::GiveUsAPickUpObject(CObjectGta** obj, int32 slotIndex) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0031D510 + 1 : 0x3E46D4), this, obj, slotIndex);
}

// Is pickup visible (checks if distance between pickup and camera is shorter than 100 units)
bool CPickup::IsVisible() {
    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

    return DistanceBetweenPoints2D(GetPosn2D(), TheCamera.GetPosition()) < 100.0f;
}

#include "game.h"
#include "net/netgame.h"
extern CNetGame *pNetGame;
bool CPickup::Update() {
    if(!pNetGame || !pNetGame->GetPlayerPool())
        return false;
    auto &ped = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_pPed;

    auto oldState = m_bIsPicked;

    auto checkPickup = [&]() {
        if (m_nPickupType == PICKUP_2P) {
            if (!ped->IsInVehicle())
                return false;

            if (DistanceBetweenPoints(GetPosn(), ped->pVehicle->GetPosition()) < 1.0f) {
                Remove();
                return true;
            }
            else
                return false;
        }
        else {
            return DistanceBetweenPoints(GetPosn(), ped->GetPosition()) < 0.9f;
        }
    };

    m_bIsPicked = checkPickup();

    if (oldState == false && m_bIsPicked == true)
        return true;

    return false;
}


void CPickup::Remove() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x002D7DC8 + 1 : 0x3E427C), this);
}
