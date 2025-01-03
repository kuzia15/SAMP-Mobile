//
// Created by x1y2z on 25.04.2024.
//

#pragma once

#include "../../common.h"

class CCustomBuildingRenderer {
public:
    static void InjectHooks();

    CCustomBuildingRenderer();
    ~CCustomBuildingRenderer();

    static bool Initialise();
    static void Shutdown();
    static bool PluginAttach();
    static void AtomicSetup(RpAtomic* atomic);
    static bool IsCBPCPipelineAttached(RpAtomic* atomic);
    static void UpdateDayNightBalanceParam();
    static void Update();
};
