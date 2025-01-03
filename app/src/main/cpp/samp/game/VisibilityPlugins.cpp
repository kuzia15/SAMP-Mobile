//
// Created by x1y2z on 03.08.2023.
//

#include "VisibilityPlugins.h"
#include "../vendor/armhook/patch.h"


void CVisibilityPlugins::Initialise() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D446C + 1 : 0x6F954C));
}

void CVisibilityPlugins::SetRenderWareCamera(RwCamera* camera) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D6248 + 1 : 0x6FB5BC), camera);
}

RpAtomic* CVisibilityPlugins::RenderPedCB(RpAtomic* atomic) {
    const float distanceSquared = GetDistanceSquaredFromCamera(RpAtomicGetFrame(atomic));
    if (distanceSquared >= ms_pedLodDist)
        return atomic;

    int32 alpha = GetClumpAlpha(RpAtomicGetClump(atomic));
    if (alpha == 255) {
        AtomicDefaultRenderCallBack(atomic);
        return atomic;
    }
    RenderAlphaAtomic(atomic, alpha);
    return atomic;
}

float CVisibilityPlugins::GetDistanceSquaredFromCamera(RwFrame* frame) {
    RwMatrix* transformMatrix = RwFrameGetLTM(frame);
    CVector distance;
    RwV3dSub(&distance, &transformMatrix->pos, ms_pCameraPosn);
    return distance.SquaredMagnitude();
}


int32 CVisibilityPlugins::GetClumpAlpha(RpClump* clump) {
    return CHook::CallFunction<int32>(g_libGTASA + (VER_x32 ? 0x005D4FEC + 1 : 0x6FA290), clump);
}

void CVisibilityPlugins::RenderAlphaAtomic(RpAtomic* atomic, int32 alpha) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D6D20 + 1 : 0x6FC1A8), atomic, alpha);
}

void CVisibilityPlugins::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676200 : 0x84A480), &CVisibilityPlugins::ms_pedLodDist);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676530 : 0x84AAD0), &CVisibilityPlugins::ms_pCameraPosn);
}

void CVisibilityPlugins::SetupVehicleVariables(RpClump *clump) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D4B90 + 1 : 0x6F9DB0), clump);
}

void CVisibilityPlugins::RenderReallyDrawLastObjects() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D6EC4 + 1 : 0x6FC3DC));
}

// The function name is misleading, it returns the flags
uint16 CVisibilityPlugins::GetAtomicId(RpAtomic* atomic) {
    return CHook::CallFunction<uint16>(g_libGTASA + (VER_x32 ? 0x5D4B54 + 1 : 0x6F9D68), atomic);
}


