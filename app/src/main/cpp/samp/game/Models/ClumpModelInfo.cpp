//
// Created by x1y2z on 20.04.2023.
//
#include "ClumpModelInfo.h"
#include "../vendor/armhook/patch.h"


void CClumpModelInfo::CClumpModelInfo__SetClump(RpClump* clump)
{
    CHook::CallFunction<void >("_ZN15CClumpModelInfo8SetClumpEP7RpClump", this, clump);
}

void CClumpModelInfo::SetFrameIds(RwObjectNameIdAssocation* data) {
    CHook::CallFunction<void>("_ZN15CClumpModelInfo11SetFrameIdsEP24RwObjectNameIdAssocation", this, data);
}

void CClumpModelInfo::CClumpModelInfo__DeleteRwObject()
{
   CHook::CallFunction<void>("_ZN15CClumpModelInfo14DeleteRwObjectEv", this);
}

RwFrame* CClumpModelInfo::GetFrameFromName(RpClump* clump, const char* name)
{
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x003856F4 + 1 : 0x45BE78), clump, name);
//    auto searchInfo = tCompSearchStructByName(name, nullptr);
//    RwFrameForAllChildren(RpClumpGetFrame(clump), FindFrameFromNameCB, &searchInfo);
//    return searchInfo.m_pFrame;
}