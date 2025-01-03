//
// Created by x1y2z on 03.08.2023.
//

#pragma once

#include "common.h"

enum eAtomicComponentFlag {
    ATOMIC_IS_NOT_PRESENT = 0x0,
    ATOMIC_IS_OK_STATE = 0x1,
    ATOMIC_IS_DAM_STATE = 0x2,
    ATOMIC_IS_LEFT = 0x4,
    ATOMIC_IS_RIGHT = 0x8,
    ATOMIC_IS_FRONT = 0x10,
    ATOMIC_IS_REAR = 0x20,
    ATOMIC_HAS_ALPHA = 0x40,
    ATOMIC_CULL = 0x80,
    ATOMIC_IS_REAR_DOOR = 0x100,
    ATOMIC_IS_FRONT_DOOR = 0x200,
    ATOMIC_RENDER_ALWAYS = 0x400, // enabled for opened components (doors)
    ATOMIC_IS_REPLACEMENT_UPGRADE = 0x800,
    ATOMIC_IS_DOOR_WINDOW_OPENED = 0x1000,
    ATOMIC_DISABLE_REFLECTIONS = 0x2000,
    // ATOMIC_HAS_DYNAMIC_TEXTURES  = 0x2000, // TODO: WTF is this?
    ATOMIC_IS_BLOWN_UP = 0x4000,
    ATOMIC_VEHCOMP_15 = 0x8000
};

class CVisibilityPlugins {
public:
    static inline float ms_pedLodDist{};
    static inline RwV3d* ms_pCameraPosn{};

public:
    static void InjectHooks();

    static void Initialise();
    static void SetRenderWareCamera(RwCamera* camera);
    static float GetDistanceSquaredFromCamera(RwFrame* frame);
    static inline float GetDistanceSquaredFromCamera1(RwFrame* frame) { return GetDistanceSquaredFromCamera(frame); };
    static void RenderReallyDrawLastObjects();

    static float GetDistanceSquaredFromCamera(const CVector* pos);

    static RpAtomic *RenderPedCB(RpAtomic *atomic);

    static int32 GetClumpAlpha(RpClump *clump);
    static uint16 GetAtomicId(RpAtomic* atomic);

    static void RenderAlphaAtomic(RpAtomic *atomic, int32 alpha);
    static void SetupVehicleVariables(RpClump* clump);
};

