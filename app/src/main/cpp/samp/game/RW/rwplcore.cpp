//
// Created by x1y2z on 19.04.2023.
//
#include "rwplcore.h"
#include "../../main.h"
#include "../vendor/armhook/patch.h"

RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    matrix->flags &= 0xFFFDFFFC;
    return matrix;
}

RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001E446C + 1 : 0x27F220), mpMat);
}

RwV3d* RwV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix) {
    return CHook::CallFunction<RwV3d*>(g_libGTASA + (VER_x32 ? 0x001E690C + 1 : 0x28231C), pointOut, pointIn, matrix);
}

RwV3d* RwV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix) {
    return CHook::CallFunction<RwV3d*>(g_libGTASA + (VER_x32 ? 0x1E6934 + 1 : 0x28235C), pointsOut, pointsIn, numPoints, matrix);
}

RwMatrix* RwMatrixOrthoNormalize(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    return CHook::CallFunction<RwMatrix*>(g_libGTASA + (VER_x32 ? 0x001E3420 + 1 : 0x27E280), matrixOut, matrixIn);
}

RwUInt32 RwStreamRead(RwStream* stream, void* buffer, RwUInt32 length) {
    return CHook::CallFunction<RwUInt32>(g_libGTASA + (VER_x32 ? 0x001E56D4 + 1 : 0x28091C), stream, buffer, length);
}

RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data) {
    return CHook::CallFunction<RwStream*>(g_libGTASA + (VER_x32 ? 0x001E59F0 + 1 : 0x280E44), type, accessType, data);
}

RwBool RwStreamClose(RwStream* stream, void* data) {
    return CHook::CallFunction<RwBool>(g_libGTASA + (VER_x32 ? 0x001E5958 + 1 : 0x280D24), stream, data);
}

RwMatrix* RwMatrixTransform(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp) {
    return CHook::CallFunction<RwMatrix*>(g_libGTASA + (VER_x32 ? 0x001E402C + 1 : 0x27EDE0), matrix, transform, combineOp);
}

RwMatrix* RwMatrixCreate() {
    return CHook::CallFunction<RwMatrix*>(g_libGTASA + (VER_x32 ? 0x001E4494 + 1 : 0x27F260));
}

RwMatrix* RwMatrixRotate(RwMatrix* pMat, CVector* axis, float angle)
{
    return CHook::CallFunction<RwMatrix*>(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710), pMat, axis, angle, rwCOMBINEPRECONCAT);
}

RwMatrix* RwMatrixTranslate(RwMatrix *matrix, const RwV3d *translation, RwOpCombineType combineOp)
{
    return CHook::CallFunction<RwMatrix*>("_Z17RwMatrixTranslateP11RwMatrixTagPK5RwV3d15RwOpCombineType", matrix, translation, combineOp);
}