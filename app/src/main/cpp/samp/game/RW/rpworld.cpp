//
// Created by plaka on 11.04.2023.
//

#include "RenderWare.h"
#include "game/common.h"
#include "../vendor/armhook/patch.h"

RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* data) {
    return CHook::CallFunction<RpClump*>(g_libGTASA + (VER_x32 ? 0x00213D66 + 1 : 0x2BA020), clump, callback, data);
}

RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* data) {
    return CHook::CallFunction<RpGeometry*>(g_libGTASA + (VER_x32 ? 0x00215F30 + 1 : 0x2BCE78), geometry, fpCallBack, data);
}

RwBool RpClumpDestroy(RpClump* clump) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x0021458C + 1 : 0x2BAAB0), clump);
}

RpClump* RpClumpRender(RpClump* clump) {
    return CHook::CallFunction<RpClump*>("_Z13RpClumpRenderP7RpClump", clump);
}

RpLight* RpLightCreate(RwInt32 type) {
    return CHook::CallFunction<RpLight*>(g_libGTASA + (VER_x32 ? 0x00216DB0 + 1 : 0x2BE078), type);
}

RwBool RpLightDestroy(RpLight* light) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x216EF4 + 1 : 0x2BE210), light);
}

RpWorld* RpWorldCreate(RwBBox* boundingBox) {
    return CHook::CallFunction<RpWorld*>(g_libGTASA + (VER_x32 ? 0x0021D144 + 1 : 0x2C6714), boundingBox);
}

RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera) {
    return CHook::CallFunction<RpWorld*>(g_libGTASA + (VER_x32 ? 0x0021DF84 + 1 : 0x2C78F0), world, camera);
}

RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color) {
    return CHook::CallFunction<RpLight*>(g_libGTASA + (VER_x32 ? 0x00216746 + 1 : 0x2BD930), light, color);
}

RpAtomic* AtomicDefaultRenderCallBack(RpAtomic* atomic) {
    return CHook::CallFunction<RpAtomic*>(g_libGTASA + (VER_x32 ? 0x002138DC + 1 : 0x2B9B08), atomic);
}

RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light) {
    return CHook::CallFunction<RpWorld*>(g_libGTASA + (VER_x32 ? 0x0021E7B0 + 1 : 0x2C8588), world, light);
}

RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light) {
    return CHook::CallFunction<RpWorld*>(g_libGTASA + (VER_x32 ? 0x0021E7F4 + 1 : 0x2C85F4), world, light);
}

RwBool RpAtomicDestroy(RpAtomic* atomic) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x0021416C + 1 : 0x2BA534), atomic);
}

void RpClumpGtaCancelStream() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x5D0BA8 + 1 : 0x6F4E38));
}