//
// Created by x1y2z on 28.04.2024.
//

#pragma once

#include "RenderWare.h"
#include "pipe/p2core.h"

struct RxOpenGLMeshInstanceData
{
    RwUInt32    primType;
    RwUInt32    vertexDesc;
    RwUInt32    vertexStride;
    RwUInt32    numVertices;
    RwUInt32    vertexDataSize;
    uintptr_t   emuArrayRef;
    RwUInt32    minVertexIdx;
    uintptr_t   vertexBufferRef;
    uintptr_t   indexBufferRef;
    RwUInt32    indexOffset;
    RwUInt32    numIndices;
    RwUInt16    *indexData;
    RpMaterial  *material;
    RwBool      vertexAlpha;
};

typedef RwBool (*RxOpenGLAllInOneInstanceCallBack)(void *, RxOpenGLMeshInstanceData*, const RwBool, const RwBool);
typedef void (*RxOpenGLAllInOneRenderCallBack)(RwResEntry *, void *, const RwUInt8, const RwUInt32);

RxNodeDefinition* RxNodeDefinitionGetOpenGLAtomicAllInOne();
void RxOpenGLAllInOneSetInstanceCallBack(RxPipelineNode *node, RxOpenGLAllInOneInstanceCallBack instanceCB);
void RxOpenGLAllInOneSetRenderCallBack(RxPipelineNode* node, RxOpenGLAllInOneRenderCallBack renderCB);
void _rxPipelineDestroy(RxPipeline *Pipeline);