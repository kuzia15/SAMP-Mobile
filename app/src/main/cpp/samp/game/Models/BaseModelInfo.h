//
// Created by x1y2z on 07.03.2023.
//

#pragma once

#include <cstdint>

#include "TimeInfo.h"
#include "game/Collision/ColModel.h"
#include "game/Core/KeyGen.h"

enum ModelInfoType : uint8_t {
    MODEL_INFO_ATOMIC = 1,
    MODEL_INFO_TIME = 3,
    MODEL_INFO_WEAPON = 4,
    MODEL_INFO_CLUMP = 5,
    MODEL_INFO_VEHICLE = 6,
    MODEL_INFO_PED = 7,
    MODEL_INFO_LOD = 8
};

enum eModelInfoSpecialType : uint8_t {
    TREE = 1,
    PALM = 2,
    GLASS_TYPE_1 = 4,
    GLASS_TYPE_2 = 5,
    TAG = 6,
    GARAGE_DOOR = 7,
    CRANE = 9,
    UNKNOWN = 10,
    BREAKABLE_STATUE = 11,
};

class CTimeInfo;

class CAtomicModelInfo;
class CClumpModelInfo;
class CDamageAtomicModelInfo;
class CLodAtomicModelInfo;
class CLodTimeModelInfo;
class CPedModelInfo;
class CTimeModelInfo;
class CVehicleModelInfo;
class CWeaponModelInfo;
struct RwObject;


struct CBaseModelInfo {
    uintptr_t 	vtable;
    uint32_t    m_nKey;
    char        m_modelName[21];
    uint8_t     pad_0;
    int16_t     m_nRefCount;
    int16_t     m_nTxdIndex;
    uint8_t     m_nAlpha;
    uint8_t     m_n2dfxCount;
    int16_t     m_n2dEffectIndex;
    int16_t     m_nObjectInfoIndex; // m_dynamicIndex
    union {
        uint16_t m_nFlags;

        struct {
            /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CModelInfoSA.h */
            uint8_t bHasBeenPreRendered : 1; // we use this because we need to apply changes only once
            uint8_t bDrawLast : 1;
            uint8_t bAdditiveRender : 1;
            uint8_t bDontWriteZBuffer : 1;
            uint8_t bDontCastShadowsOn : 1;
            uint8_t bDoWeOwnTheColModel : 1;
            uint8_t bIsBackfaceCulled : 1;
            uint8_t bIsLod : 1;

            // 1st byte
            union {
                struct { // Atomic flags
                    uint8_t bIsRoad : 1;
                    uint8_t bAtomicFlag0x200: 1;
                    uint8_t bDontCollideWithFlyer : 1;
                    uint8_t nSpecialType : 4;
                    uint8_t bWetRoadReflection : 1;
                };
                struct { // Vehicle flags
                    uint8_t bUsesVehDummy : 1;
                    uint8_t : 1;
                    uint8_t nCarmodId : 5;
                    uint8_t bUseCommonVehicleDictionary : 1;
                };
                struct { // Clump flags
                    uint8_t bHasAnimBlend : 1;
                    uint8_t bHasComplexHierarchy : 1;
                    uint8_t bAnimSomething : 1;
                    uint8_t bOwnsCollisionModel : 1;
                    uint8_t : 3;
                    uint8_t bTagDisabled : 1;
                };
            };
        };
    };
    uint8_t     pad_1[2];
    CColModel   *m_pColModel;
    float       m_fDrawDistance;  // m_lodDistance
    union {
        struct RwObject *m_pRwObject;
        struct RwClump  *m_pRwClump;
        struct RpAtomic *m_pRwAtomic;
    };

public:

    // vtable next
    ModelInfoType GetModelType();
    int32 GetAnimFileIndex();
    void DeleteRwObject();

    void SetModelName(const char* modelName) { m_nKey = CKeyGen::GetUppercaseKey(modelName); }
    void SetTexDictionary(const char* txdName, const char *dbName);

    CVehicleModelInfo* AsVehicleModelInfoPtr() { return reinterpret_cast<CVehicleModelInfo*>(this); }
    CPedModelInfo*     AsPedModelInfoPtr()     { return reinterpret_cast<CPedModelInfo*>(this); }
    CWeaponModelInfo*  AsWeaponModelInfoPtr()  { return reinterpret_cast<CWeaponModelInfo*>(this); }

    [[nodiscard]] CColModel* GetColModel() const { return m_pColModel; }

    [[nodiscard]] bool IsSwayInWind1()         const { return nSpecialType == eModelInfoSpecialType::TREE; }               // 0x0800
    [[nodiscard]] bool IsSwayInWind2()         const { return nSpecialType == eModelInfoSpecialType::PALM; }               // 0x1000
    [[nodiscard]] bool SwaysInWind()           const { return IsSwayInWind1() || IsSwayInWind2(); }
    [[nodiscard]] bool IsGlassType1()          const { return nSpecialType == eModelInfoSpecialType::GLASS_TYPE_1; }       // 0x2000
    [[nodiscard]] bool IsGlassType2()          const { return nSpecialType == eModelInfoSpecialType::GLASS_TYPE_2; }       // 0x2800
    [[nodiscard]] bool IsGlass()               const { return IsGlassType1() || IsGlassType2(); }
    [[nodiscard]] bool IsTagModel()            const { return nSpecialType == eModelInfoSpecialType::TAG; }                // 0x3000
    [[nodiscard]] bool IsGarageDoor()          const { return nSpecialType == eModelInfoSpecialType::GARAGE_DOOR; }        // 0x3800
    [[nodiscard]] bool IsBreakableStatuePart() const { return nSpecialType == eModelInfoSpecialType::BREAKABLE_STATUE; }
    [[nodiscard]] bool IsCrane()               const { return nSpecialType == eModelInfoSpecialType::CRANE; }              // 0x4800
};


static_assert(sizeof(CBaseModelInfo) == (VER_x32 ? 0x38 : 0x48), "Invalid size CBaseModelInfo");
