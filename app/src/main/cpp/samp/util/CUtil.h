//
// Created by roman on 11/19/2024.
//

#pragma once
#include "../main.h"
#include "game/SimpleTransform.h"


class CUtil {
public:
    static uintptr_t FindLib(const char *libname);

    // Converts degrees to radians
    // keywords: 0.017453292 flt_8595EC
    constexpr static float DegreesToRadians(float angleInDegrees) {
        return angleInDegrees * PI / 180.0F;
    }

    // Converts radians to degrees
    // 57.295826
    constexpr static float RadiansToDegrees(float angleInRadians) {
        return angleInRadians * 180.0F / PI;
    }

    static RwTexture *LoadTextureFromDB(const char *dbname, const char *texture);

    static RwTexture* GetTexture(const char* name);

    static void __fastcall TransformPoint(RwV3d &result, const CSimpleTransform &t, const RwV3d &v);
};

