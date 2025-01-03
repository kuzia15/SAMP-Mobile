//
// Created by x1y2z on 15.11.2023.
//

#include "Renderer.h"
#include "../vendor/armhook/patch.h"

void CRenderer::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6764D0 : 0x84AA10), &ms_bRenderOutsideTunnels);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x67914C : 0x8502C8), &m_loadingPriority);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x6778EC : 0x84D210), &ms_aVisibleEntityPtrs);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6771F0 : 0x84C428), &ms_nNoOfVisibleEntities);
}
