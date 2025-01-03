//
// Created by x1y2z on 20.04.2023.
//
#pragma once
#include "RenderWare.h"

RwObject* GetFirstObjectCallback(RwObject* object, void* data);
RwObject* GetFirstObject(RwFrame* frame);
RpAtomic* GetFirstAtomicCallback(RpAtomic* atomic, void* data);
RpAtomic* GetFirstAtomic(RpClump* clump);
RpHAnimHierarchy* GetAnimHierarchyFromSkinClump(RpClump* clump);
RpAtomic* SkinAtomicGetHAnimHierarchCB(RpAtomic* atomic, void* data);
RpAtomic* AtomicRemoveAnimFromSkinCB(RpAtomic* atomic, void* data);
RpClump* RpClumpGetBoundingSphere(RpClump* clump, RwSphere* sphere, bool bUseLTM);