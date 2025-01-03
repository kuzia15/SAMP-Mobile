//
// Created by x1y2z on 07.03.2023.
//

#pragma once

#include "BaseModelInfo.h"
#include "game/common.h"
#include <RenderWare.h>
#include "game/RwObjectNameIdAssocation.h"


struct CClumpModelInfo : public CBaseModelInfo {
    union {
        char *m_animFileName;
        unsigned int m_dwAnimFileIndex;
    };

public:
    // Added vtable methods
    void CClumpModelInfo__SetClump(RpClump* clump);
    void CClumpModelInfo__DeleteRwObject();

    // static functions
    static RpAtomic* SetAtomicRendererCB(RpAtomic* atomic, void* renderFunc);
    static RpAtomic* AtomicSetupLightingCB(RpAtomic* atomic, void* data);
    static RpAtomic* SetHierarchyForSkinAtomic(RpAtomic* atomic, void* data);
    /* struct tSearchData { char *name; RwFrame *result; };
      returns 0 if we found frame, or last frame if we need to continue searching */
    static RwFrame* FindFrameFromNameCB(RwFrame* frame, void* searchData);
    static RwFrame* FindFrameFromNameWithoutIdCB(RwFrame* frame, void* searchData);
    static RwFrame* FindFrameFromIdCB(RwFrame* frame, void* searchData);
    static RwFrame* FillFrameArrayCB(RwFrame* frame, void* data);
    static RwFrame* GetFrameFromId(RpClump* clump, int32 id);
    static RwFrame* GetFrameFromName(RpClump* clump, const char* name);
    static void FillFrameArray(RpClump* clump, RwFrame** frames);

    // Class functions
    void SetFrameIds(RwObjectNameIdAssocation* data);
};

static_assert(sizeof(CClumpModelInfo) == (VER_x32 ? 0x3c : 0x50));

