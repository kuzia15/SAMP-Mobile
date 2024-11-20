//
// Created by plaka on 07.03.2023.
//

#pragma once

#include "PedModelInfo.h"
#include "AtomicModelInfo.h"
#include "VehicleModelInfo.h"
#include "game/Core/Store.h"
#include "game/constants.h"


class CModelInfo {
public:
    static constexpr int32 NUM_MODEL_INFOS = TOTAL_DFF_MODEL_IDS;
    static inline CBaseModelInfo* ms_modelInfoPtrs[NUM_MODEL_INFOS];

    static void injectHooks();

    static constexpr int32_t NUM_PED_MODEL_INFOS = 350;
    static inline CStore<CPedModelInfo, NUM_PED_MODEL_INFOS> ms_pedModelInfoStore;

    static inline CStore<CAtomicModelInfo, 25000> ms_atomicModelInfoStore;

    static constexpr int32 NUM_VEHICLE_MODEL_INFOS = 370;
    static inline CStore<CVehicleModelInfo, NUM_VEHICLE_MODEL_INFOS> ms_vehicleModelInfoStore;

    static CPedModelInfo *AddPedModel(int index);

    static CAtomicModelInfo *AddAtomicModel(int index);
    static CDamageAtomicModelInfo* AddDamageAtomicModel(int32 index);
    static CVehicleModelInfo *AddVehicleModel(int index);

    static CBaseModelInfo* GetModelInfo(int index) { return ms_modelInfoPtrs[index]; }
    static auto GetPedModelInfo(int32_t index) { return GetModelInfo(index)->AsPedModelInfoPtr(); }
    static auto GetVehicleModelInfo(int32_t index) { return GetModelInfo(index)->AsVehicleModelInfoPtr(); }
    static void SetModelInfo(int index, CBaseModelInfo* pInfo) { ms_modelInfoPtrs[index] = pInfo; }

    static void Initialise();
};

