//
// Created by x1y2z on 07.03.2023.
//

#include "ModelInfo.h"
#include "../vendor/armhook/patch.h"
#include "log.h"

CVehicleModelInfo* CModelInfo::AddVehicleModel(int index)
{
    auto& pInfo = CModelInfo::ms_vehicleModelInfoStore.AddItem();

    ((void(*)(CVehicleModelInfo*))(g_libGTASA + (VER_x32 ? 0x386E98 + 1 : 0x45DF50)))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    pInfo.vtable = g_libGTASA + (VER_x32 ? 0x006676A8 : 0x82F3B0);

    ((void(*)(CVehicleModelInfo*))(*(uintptr_t*)(pInfo.vtable + (VER_x32 ? 0x1C : 0x1C*2))))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CPedModelInfo* CModelInfo::AddPedModel(int index)
{

    auto& pInfo = CModelInfo::ms_pedModelInfoStore.AddItem();

    ((void(*)(CPedModelInfo*))(g_libGTASA + (VER_x32 ? 0x00384FD8 + 1 : 0x45B3BC)))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    pInfo.vtable = g_libGTASA + (VER_x32 ? 0x00667658 : 0x82F310);

    ((void(*)(CPedModelInfo*))(*(uintptr_t*)(pInfo.vtable + (VER_x32 ? 0x1C : 0x1C*2))))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CDamageAtomicModelInfo* CModelInfo::AddDamageAtomicModel(int32 index)
{
    return CHook::CallFunction<CDamageAtomicModelInfo*>(g_libGTASA + (VER_x32 ? 0x00385F94 + 1 : 0x49BAE8), index);
}

CAtomicModelInfo* CModelInfo::AddAtomicModel(int index)
{
    auto& pInfo = ms_atomicModelInfoStore.AddItem();

    ((void(*)(CAtomicModelInfo*))(g_libGTASA + (VER_x32 ? 0x00384FD8 + 1 : 0x45B3BC)))(&pInfo);

    pInfo.vtable = g_libGTASA + (VER_x32 ? 0x00667444 : 0x82EEE8);

    ((void(*)(CAtomicModelInfo*))(*(uintptr_t*)(pInfo.vtable + (VER_x32 ? 0x1C : 0x1C*2))))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

void CModelInfo::Initialise() {
    memset(ms_modelInfoPtrs, 0, sizeof(ms_modelInfoPtrs));

}

void CModelInfo::injectHooks()
{
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676A34 : 0x84B4C8), &CModelInfo::ms_atomicModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6773CC : 0x84C7E0), &CModelInfo::ms_pedModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678C8C : 0x84F948), &CModelInfo::ms_vehicleModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6796CC : 0x850DB8), &CModelInfo::ms_modelInfoPtrs);

    CHook::Redirect("_ZN10CModelInfo11AddPedModelEi", &CModelInfo::AddPedModel);
    CHook::Redirect("_ZN10CModelInfo15AddVehicleModelEi", &CModelInfo::AddVehicleModel);
    CHook::Redirect("_ZN10CModelInfo14AddAtomicModelEi", &CModelInfo::AddAtomicModel);
}
