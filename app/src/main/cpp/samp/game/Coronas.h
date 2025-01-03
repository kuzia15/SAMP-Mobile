//
// Created by x1y2z on 21.05.2023.
//

#pragma once

#include "common.h"
#include "RegisteredCorona.h"
#include "../Entity/CEntityGTA.h"

class CCoronas {
public:
    static inline float SunScreenX;
    static inline float SunScreenY;
    // are there any obstacles between sun and camera
    static inline bool SunBlockedByClouds;
//    // change coronas brightness immediately
//    static bool& bChangeBrightnessImmediately;
//    // num of registered coronas in frame
//    static uint32& NumCoronas;
//    // coronas intensity multiplier
//    static float& LightsMult;
    // this is used to control moon size when you shooting it with sniper
    static inline uint32 MoonSize = 3;
    // Coronas array. count: MAX_NUM_CORONAS (default: 64)
    static constexpr int32 MAX_NUM_CORONAS = 64;
    static inline CRegisteredCorona aCoronas[MAX_NUM_CORONAS];
//
//    static uint16 (&ms_aEntityLightsOffsets)[8];
//
//    inline static struct { // NOTSA
//        bool DisableWetRoadReflections;
//        bool AlwaysRenderWetRoadReflections; // Ignored if if `DisableReflections == false`
//    } s_DebugSettings{};

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void Update();
    static void Render();
    static void RenderReflections();
    static void RenderSunReflection();
    static void RegisterCorona(uint32 id, CEntityGTA* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector& posn,
                               float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
    static void RegisterCorona(uint32 id, CEntityGTA* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector* posn,
                               float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);

    static void UpdateCoronaCoors(uint32 id, const CVector& posn, float farClip, float angle);
    static void DoSunAndMoon();
};

extern uint32 MAX_CORONAS;
constexpr int32 CORONA_TEXTURES_COUNT = 10;
//static inline RwTexture* gpCoronaTexture[CORONA_TEXTURES_COUNT];

static inline const char* aCoronaSpriteNames[] = {
        "coronastar",
        "coronamoon",
        "coronasun",
        "coronaboom",
        "coronaplace",
        "coronaarc",
        "coronastatus",
        "coronaright",
        "coronalef",
        "corona"
};