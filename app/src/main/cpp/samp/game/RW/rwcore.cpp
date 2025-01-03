//
// Created by x1y2z on 20.04.2023.
//
#include "rwcore.h"
#include "rpworld.h"
#include "../vendor/armhook/patch.h"
#include <assert.h>

RwFrame* RwFrameUpdateObjects(RwFrame* frame) {
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x001D802C + 1 : 0x26F7B4), frame);
}

RwTexture* RwTextureCreate(RwRaster* raster) {
    return CHook::CallFunction<RwTexture*>(g_libGTASA + (VER_x32 ? 0x001DB7BC + 1 : 0x273F74), raster);
}

RwCamera* RwCameraCreate() {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5EE0 + 1 : 0x26D454));
}

RwFrame* RwFrameCreate() {
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x001D81AC + 1 : 0x26F9E0));
}

RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* colour, RwInt32 clearMode) {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5CF0 + 1 : 0x26D1E8), camera, colour, clearMode);
}

RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip) {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5A38 + 1 : 0x26CF8C), camera, nearClip);
}

RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip) {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5ACC + 1 : 0x26D034), camera, farClip);
}

RwFrame* RwFrameTranslate(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x001D8614 + 1 : 0x270060), frame, v, combine);
}

RwFrame* RwFrameRotate(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine) {
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x001D8728 + 1 : 0x270204), frame, axis, angle, combine);
}

RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow) {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5E04 + 1 : 0x26D330), camera, viewWindow);
}

RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection) {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5D28 + 1 : 0x26D24C), camera, projection);
}

void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x001DCF64 + 1 : 0x275CF0), object, frame);
}

RwMatrix* RwFrameGetLTM(RwFrame* frame) {
    return CHook::CallFunction<RwMatrix*>(g_libGTASA + (VER_x32 ? 0x001D849C + 1 : 0x26FE10), frame);
}

RwCamera* RwCameraEndUpdate(RwCamera* camera) {
    return CHook::CallFunction<RwCamera*>(g_libGTASA + (VER_x32 ? 0x001D5A14 + 1 : 0x26CF48), camera);
}

RwBool RwIm3DEnd() {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001DD03C + 1 : 0x275E20));
}

RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001DD1C4 + 1 : 0x275FD8), primType);
}

RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex* indices, RwInt32 numIndices) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001DD084 + 1 : 0x275E70), primType, indices, numIndices);
}

void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags) {
    return CHook::CallFunction<void*>(g_libGTASA + (VER_x32 ? 0x001DCFAC + 1 : 0x275D54), pVerts, numVerts, ltm, flags);
}

RwTexture* RwTextureRead(const char* name, const char* maskName) {
    return CHook::CallFunction<RwTexture*>(g_libGTASA + (VER_x32 ? 0x001DBA3C + 1 : 0x2742C8), name, maskName);
}

RwFrame* RwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, void* data) {
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x001D8858 + 1 : 0x2703BC), frame, callBack, data);
}

RwBool RwFrameDestroy(RwFrame* frame) {
    assert(frame);
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001D83EC + 1 : 0x26FD08), frame);
}

RwTexture* RwTextureSetRaster(RwTexture* texture, RwRaster* raster) {
    return CHook::CallFunction<RwTexture*>(g_libGTASA + (VER_x32 ? 0x001DB4D4 + 1 : 0x273C28), texture, raster);
}

RwBool RwCameraDestroy(RwCamera* camera) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001D5EA0 + 1 : 0x26D3F8), camera);
}

RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x1DD3B4 + 1 : 0x276238), vert1, vert2);
}

RwTexture* RwTextureSetName(RwTexture* texture, const RwChar* name) {
    return CHook::CallFunction<RwTexture*>(g_libGTASA + (VER_x32 ? 0x1DB820 + 1 : 0x274000), texture, name);
}

RwFrame* RwFrameOrthoNormalize(RwFrame* frame) {
    return CHook::CallFunction<RwFrame*>(g_libGTASA + (VER_x32 ? 0x1D87FC + 1 : 0x27032C), frame);
}

RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x1DB3A4 + 1 : 0x273AB4), callBack);
}

RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x1DB3E0 + 1 : 0x273AFC), callBack);
}