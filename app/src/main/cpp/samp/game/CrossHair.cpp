//
// Created by plaka on 10.11.2023.
//

#include "CrossHair.h"
#include "common.h"
#include "../util/CUtil.h"
#include "../main.h"
#include "../vendor/armhook/patch.h"
#include "Camera.h"
#include "Core/Quaternion.h"
#include "playerped.h"
#include "vehicle.h"
#include "Entity/CVehicleGTA.h"
#include "Entity/CPhysical.h"
#include "object.h"
#include "aimstuff.h"
#include "net/netgame.h"

extern CNetGame *pNetGame;

void CCrossHair::Init()
{
    FLog("CCrossHair::Init");
    pCircleTex = new CSprite2d();
    pCircleTex->m_pTexture = CUtil::LoadTextureFromDB("txd", "siteM16");
    FLog("CCrossHair::Init 2");
}

void CCrossHair::Render()
{
    if(!pNetGame || pNetGame->GetPlayerPool())
        return;

    auto pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();

    int iCamState = GameGetLocalPlayerCameraMode();

    int v1 = iCamState & 0xFFFD;
    if(iCamState == 53 || iCamState == 39 || v1 == 40)
    {
        static float fCHairScreenMultX = (RsGlobal->maximumWidth - (RsGlobal->maximumHeight / 9 * 16)) / 2 + ((RsGlobal->maximumHeight / 9 * 16) * 0.524);
        static float fFixedOffset = RsGlobal->maximumWidth / (RsGlobal->maximumWidth - (RsGlobal->maximumHeight / 9 * 16)) * 2.0;
        auto gunRadius = CHook::CallFunction<float>(g_libGTASA + 0x004C69E8 + 1, pPed->m_pPed);
        static float fCHairScreenMultY = (RsGlobal->maximumHeight / 9 * 16) / 10 * 6 * 0.4 + fFixedOffset;

        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));

        float f1 = ((RsGlobal->maximumHeight / 448.0) * 64.0) * gunRadius;

        float fPosX1 = ((f1 * 0.5) + fCHairScreenMultX) - f1;
        float fPosY1 = ((f1 * 0.5) + fCHairScreenMultY) - f1;

        float fPosX2 = (f1 * 0.5) + fPosX1;
        float fPosY2 = (f1 * 0.5) + fPosY1;

        pCircleTex->Draw(CRect{fPosX1, fPosY1, fPosX2, fPosY2}, CRGBA(255, 255, 255, 255));
        pCircleTex->Draw(CRect{fPosX1 + f1, fPosY1, fPosX2, fPosY2}, CRGBA(255, 255, 255, 255));
        pCircleTex->Draw(CRect{fPosX1, fPosY1 + f1, fPosX2, fPosY2}, CRGBA(255, 255, 255, 255));
        pCircleTex->Draw(CRect{fPosX1 + f1, fPosY1 + f1, fPosX2, fPosY2}, CRGBA(255, 255, 255, 255));
        return;
    }
}