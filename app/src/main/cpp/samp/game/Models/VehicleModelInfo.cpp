//
// Created by x1y2z on 11.04.2023.
//

#include "VehicleModelInfo.h"
#include "../vendor/armhook/patch.h"
#include "../RW/rpworld.h"
#include "ModelInfo.h"
#include "game/VisibilityPlugins.h"

void CVehicleModelInfo::CVehicleModelInfo__SetClump(RpClump* clump)
{
    m_pVehicleStruct = new CVehicleStructure();
    CClumpModelInfo__SetClump(clump);
    SetAtomicRenderCallbacks();

    RwObjectNameIdAssocation* (&ms_vehicleDescs)[NUM_VEHICLE_MODEL_DESCS] = *(RwObjectNameIdAssocation*(*)[NUM_VEHICLE_MODEL_DESCS])(g_libGTASA + (VER_x32 ? 0x00687C7C : 0x8614C0));
    CClumpModelInfo::SetFrameIds(ms_vehicleDescs[m_nVehicleType]);
    SetRenderPipelines();
    PreprocessHierarchy();
    ReduceMaterialsInVehicle();
//    m_nCurrentPrimaryColor = 255;
//    m_nCurrentSecondaryColor = 255;
//    m_nCurrentTertiaryColor = 255;
//    m_nCurrentQuaternaryColor = 255;
//    SetCarCustomPlate();
}

void CVehicleModelInfo::CVehicleModelInfo__DeleteRwObject()
{
    delete m_pVehicleStruct;
    m_pVehicleStruct = nullptr;
    CClumpModelInfo__DeleteRwObject();
}

void CVehicleModelInfo::SetAtomicRenderCallbacks()
{
   CHook::CallFunction<void>("_ZN17CVehicleModelInfo24SetAtomicRenderCallbacksEv", this);
}

CVehicleModelInfo::CVehicleStructure::CVehicleStructure() : m_aUpgrades()
{
    for (auto& vecPos : m_avDummyPos)
        vecPos.Set(0.0F, 0.0F, 0.0F);

    for (auto& upgrade : m_aUpgrades)
        upgrade.m_nParentComponentId = -1;

    memset(m_apExtras, 0, sizeof(m_apExtras));
    m_nNumExtras = 0;
    m_nMaskComponentsDamagable = 0;
}

CVehicleModelInfo::CVehicleStructure::~CVehicleStructure()
{
    for (int32 i = 0; i < m_nNumExtras; ++i) {
        auto atomic = m_apExtras[i];
        auto frame = RpAtomicGetFrame(atomic);
        RpAtomicDestroy(atomic);
        RwFrameDestroy(frame);
    }
}

void CVehicleModelInfo::ReduceMaterialsInVehicle()
{
   CHook::CallFunction<void>("_ZN17CVehicleModelInfo24ReduceMaterialsInVehicleEv", this);
}


void CVehicleModelInfo::PreprocessHierarchy()
{
   CHook::CallFunction<void>("_ZN17CVehicleModelInfo19PreprocessHierarchyEv", this);
}

void CVehicleModelInfo::SetRenderPipelines()
{
    CHook::CallFunction<void>("_ZN17CVehicleModelInfo18SetRenderPipelinesEv", this);
}

void* CVehicleModelInfo::CVehicleStructure::operator new(size_t size)
{
    return m_pInfoPool->New();
}

void CVehicleModelInfo::CVehicleStructure::operator delete(void* data)
{
    m_pInfoPool->Delete(reinterpret_cast<CVehicleStructure*>(data));
}

/*RpMaterial* RemoveWindowAlphaCB(RpMaterial* mat, void* data)
{
    if (mat->color.alpha != 255) {
        AddStoredMaterial(RpMaterialGetColor(mat));

        mat->color = {0, 0, 0, 0};
    }
    return mat;
}

RpAtomic* CVehicleModelInfo::SetEditableMaterialsCB(RpAtomic* atomic, void* data)
{
    if (rwObjectTestFlags(atomic, RpAtomicFlag::rpATOMICRENDER) == 0)
        return atomic;

    if (CVisibilityPlugins::GetAtomicId(atomic) & eAtomicComponentFlag::ATOMIC_IS_DOOR_WINDOW_OPENED)
        RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), RemoveWindowAlphaCB, data);

    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetEditableMaterialsCB, data);
    return atomic;
}

RpMaterial* CVehicleModelInfo::SetEditableMaterialsCB(RpMaterial* mat, void* data)
{
    auto pVeh = (CVehicle*)data;
    int color = *(int*)&mat->color;
    
    if(mat->texture) {
        if (RpMaterialGetTexture(mat) == ms_pLightsTexture) {
            int32 iLightIndex = -1;
            if (color == 0xff00afff) // driver side. front 255, 175, 0
                iLightIndex = 0;
            else if (color == 0xffc8ff00) // pass side. front  0, 255, 200
                iLightIndex = 1;
            else if (color == 0xff00ffb9) // driver side. rear 185, 255, 0
                iLightIndex = 2;
            else if (color == 0xff003cff) // pass side. rear 255, 60, 0, 0
                iLightIndex = 3;

            AddStoredMaterial(RpMaterialGetColor(mat));

            RpMaterialGetColor(mat)->red = 255;
            RpMaterialGetColor(mat)->green = 255;
            RpMaterialGetColor(mat)->blue = 255;

            if (iLightIndex != -1 && ms_lightsOn[iLightIndex]) {
                AddStoredMaterial(&mat->texture);
                AddStoredMaterial(RpMaterialGetSurfaceProperties(mat));


                mat->texture = ms_pLightsOnTexture;
                RpMaterialSetSurfaceProperties(mat, &gLightSurfProps);
            }
        }
        if (!strcmp(mat->texture->name, "nomer")) {
            AddStoredMaterial(&mat->texture);
            if (pVeh->pPlateTexture) {
                mat->texture = pVeh->pPlateTexture;
            } else {
                mat->texture = CCustomPlateManager::GetNoPlateTex();
            }
            mat->color.alpha = 255;

            return mat;
        }
    }

    if ( color == 0xff00ff3c ) { // first color 60, 255, 0
        AddStoredMaterial(RpMaterialGetColor(mat));
        if (pVeh->m_pVinylTex) {
            AddStoredMaterial(&mat->texture);
            mat->texture = pVeh->m_pVinylTex;
            mat->color = {255, 255, 255, 255};
            return mat;
        }
        mat->color = pVeh->mainColor;
        return mat;
    }
    if ( color == 0xffaf00ff ) { // second color 255, 0, 175
        AddStoredMaterial(RpMaterialGetColor(mat));
        mat->color = pVeh->secondColor;
        return mat;
    }
    else if (color == 0xff00ffff) { // toner
        AddStoredMaterial(RpMaterialGetColor(mat));

        mat->color = pVeh->tonerColor;
        return mat;
    }
    if ( color == 0xff00647d ) { // wheel
        AddStoredMaterial(RpMaterialGetColor(mat));

        mat->color = pVeh->wheelColor;
        return mat;
    }
    return mat;
}

void CVehicleModelInfo::SetEditableMaterials(RpClump* clump)
{
    if (clump)
    {
        auto vehId = CVehiclePool::FindIDFromRwObject(&clump->object);
        auto pVehicle = CVehiclePool::GetAt(vehId);
        if (pVehicle)
        {
            RpClumpForAllAtomics(clump, SetEditableMaterialsCB, pVehicle);
        }
    }
}

void CVehicleModelInfo::ResetEditableMaterials(RpClump* clump)
{
    for (auto& p : gStoredMaterials)
    {
        *p.first = p.second;
    }
    gStoredMaterials.clear();
}

//  ----------------- hook ----------------------

void CVehicleModelInfo__SetClump_hook(CVehicleModelInfo* thiz, RpClump* clump) {
    thiz->CVehicleModelInfo__SetClump(clump);
}

void CVehicleModelInfo__DeleteRwObject_hook(CVehicleModelInfo* thiz) {
    thiz->CVehicleModelInfo__DeleteRwObject();
}*/

void CVehicleModelInfo::InjectHooks() {
    //CHook::Write(g_libGTASA + (VER_x32 ? 0x679D00 : 0x851A18), &ms_pLightsTexture);
    //CHook::Write(g_libGTASA + (VER_x32 ? 0x67820C : 0x84E440), &ms_pLightsOnTexture);
    //CHook::Write(g_libGTASA + (VER_x32 ? 0x676CD4 : 0x84BA08), &ms_lightsOn);

    //CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66E910 : 0x83DEE0), &SetEditableMaterials);
    //CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x675E30 : 0x849D10), &ResetEditableMaterials);

//    CVehicleStructure::m_pInfoPool = new CPool<CVehicleStructure>(CModelInfo::NUM_VEHICLE_MODEL_INFOS, "VehicleStruct");
//
//    CHook::Redirect("_ZN17CVehicleModelInfo8SetClumpEP7RpClump", &CVehicleModelInfo__SetClump_hook);
//    CHook::Redirect("_ZN17CVehicleModelInfo14DeleteRwObjectEv", &CVehicleModelInfo__DeleteRwObject_hook);
}