//
// Created by x1y2z on 07.03.2023.
//

#pragma once

#include "ClumpModelInfo.h"
#include "game/Enums/eVehicleClass.h"
#include "game/Core/Quaternion.h"
#include "game/Core/Vector.h"
#include "game/RW/rpworld.h"
#include "game/Enums/eVehicleType.h"
#include "game/Core/Pool.h"

enum eVehicleDummy {
    DUMMY_LIGHT_FRONT_MAIN      = 0,
    DUMMY_LIGHT_REAR_MAIN       = 1,

    DUMMY_LIGHT_FRONT_SECONDARY = 2,
    DUMMY_LIGHT_REAR_SECONDARY  = 3,

    DUMMY_SEAT_FRONT            = 4,
    DUMMY_SEAT_REAR             = 5,

    DUMMY_EXHAUST               = 6,
    DUMMY_ENGINE                = 7,
    DUMMY_GAS_CAP               = 8,
    DUMMY_TRAILER_ATTACH        = 9,
    DUMMY_HAND_REST             = 10,
    DUMMY_EXHAUST_SECONDARY     = 11,
    DUMMY_WING_AIR_TRAIL        = 12,
    DUMMY_VEHICLE_GUN           = 13,
};

union tVehicleCompsUnion {
    uint32_t m_nComps;
    struct {
        uint32_t nExtraA_comp1 : 4;
        uint32_t nExtraA_comp2 : 4;
        uint32_t nExtraA_comp3 : 4;
        uint32_t : 4;

        uint32_t nExtraB_comp1 : 4;
        uint32_t nExtraB_comp2 : 4;
        uint32_t nExtraB_comp3 : 4;
        uint32_t : 4;
    };
    struct {
        uint32_t nExtraAComp : 12;
        uint32_t nExtraARule : 4;

        uint32_t nExtraBComp : 12;
        uint32_t nExtraBRule : 4;
    };

    struct {
        uint32_t nExtraA : 16;
        uint32_t nExtraB : 16;
    };
};

struct UpgradePosnDesc {
    CVector         m_vPosition;
    CQuaternion     m_qRotation;
    int32_t         m_nParentComponentId;
};
static_assert(sizeof(UpgradePosnDesc) == 0x20, "Invalid size UpgradePosnDesc");


struct CVehicleModelInfo : public CClumpModelInfo {
public:
    RpMaterial*         m_pPlateMaterial; // 3C
    char                m_szPlateText[9]; // 40
    unsigned char       m_nPlateType;    //  49
    char                m_szGameName[8]; //  4a
    char               _pad3A[2];
    eVehicleType        m_nVehicleType;  //  54
    float               m_fWheelSizeFront;// 58
    float               m_fWheelSizeRear;    //5C
    int16_t             m_nWheelModelIndex;  //60
    int16_t             m_nHandlingId;       // 62
    int8_t              m_nNumDoors;     //64
    eVehicleClass       m_nVehicleClass; //65
    int8_t              m_nFlags;    //66
    int8_t              m_nWheelUpgradeClass;//67
    int8_t              m_nTimesUsed;    //68
    uint8_t             pad_0;
    int16_t             m_freq;
    tVehicleCompsUnion  m_extraComps;    //6c
    float               m_fBikeSteerAngle; // 70

    struct CVehicleStructure {
    public:
        CVehicleStructure();
        ~CVehicleStructure();
        static void* operator new(size_t size);
        static void  operator delete(void* data);

    public:
        static constexpr int32 NUM_DUMMIES = 15;
        static constexpr int32 NUM_UPGRADES = 18;
        static constexpr int32 NUM_EXTRAS = 6;

    public:
        CVector         m_avDummyPos[NUM_DUMMIES];
        UpgradePosnDesc m_aUpgrades[NUM_UPGRADES];
        RpAtomic*       m_apExtras[NUM_EXTRAS];
        RpAtomic*       m_pXtraAtomicLOD[6];
        int8_t          m_nNumExtras;
        uint8           pad0[3];
        uint32_t        m_nMaskComponentsDamagable;

    public:
        static inline CPool<CVehicleStructure>* m_pInfoPool;

    public: // Helpers
        [[nodiscard]] bool IsDummyActive(eVehicleDummy dummy) const {
            return m_avDummyPos[dummy] != 0.0F;
        }

        [[nodiscard]] bool IsComponentDamageable(int32 nodeIndex) const {
            return m_nMaskComponentsDamagable & (1 << nodeIndex);
        }

    } * m_pVehicleStruct;
    VALIDATE_SIZE(CVehicleStructure, (VER_x32 ? 0x32C : 0x360));

    RpMaterial* m_firstColour[49];
    RpMaterial* m_secondColour[33];
    RpMaterial* m_thirdColour[17];
    RpMaterial* m_fourthColour[17];
    RpMaterial* m_apDirtMaterials[64];
    uint8_t     m_possibleColours[4][8];
    uint8_t     m_numPossibleColours;
    uint8_t     m_lastColUsed;
    uint8_t     m_lastCol[4];
    int16_t     m_anUpgrades[18];
    uint8_t     pad_1[2];
    const char* m_remaps[4];

    union {
        uintptr_t* m_pAnimBlock;
        char* m_animBlockFileName;
        unsigned int m_dwAnimBlockIndex;
    };

    // vehicle components description tables
    // static RwObjectNameIdAssocation ms_vehicleDescs[12];
    static constexpr int32 NUM_VEHICLE_MODEL_DESCS = 12;
   // static inline RwObjectNameIdAssocation* ms_vehicleDescs[NUM_VEHICLE_MODEL_DESCS]; need initializer

    // vehiclelights128 texture
    static inline RwTexture* ms_pLightsTexture;
    // vehiclelightson128 texture
    static inline RwTexture* ms_pLightsOnTexture;

    // Light states for currently rendered car
    static constexpr int32 NUM_LIGHTS = 4;
    static inline uint8 ms_lightsOn[NUM_LIGHTS];

public:
    static void InjectHooks();

    // vt
    void CVehicleModelInfo__DeleteRwObject();
    void CVehicleModelInfo__SetClump(RpClump* clump);

    // setup model render callbacks
    void SetAtomicRenderCallbacks();
    // set component flags
    void SetVehicleComponentFlags(RwFrame* component, uint32 flags);
    // setups rendering pipelines for atomics in model (CCustomCarEnvMapPipeline::CustomPipeAtomicSetup)
    void SetRenderPipelines();
    // get position of dummy in model-space
    CVector* GetModelDummyPosition(eVehicleDummy dummy) const { return &m_pVehicleStruct->m_avDummyPos[dummy]; } // NOTSA
    // setup vehicle model components
    void PreprocessHierarchy();
    // remove some unused materials in model?
    void ReduceMaterialsInVehicle();

    // change colors and settings of material according to vehicle color and lights states.  Data
    // contains pointer to restore entries
    static RpMaterial* SetEditableMaterialsCB(RpMaterial* material, void* data);
    // execute SetEditableMaterialsCB(RpMaterial *, void *) for atomic materials and also remove
    // vehicle window if needed. Data contains pointer to restore entries
    static RpAtomic* SetEditableMaterialsCB(RpAtomic* atomic, void* data);
    // execute SetEditableMaterialsCB(RpAtomic *, void *) for atomics in clump. This one is called
    // before vehicle rendering
    static void SetEditableMaterials(RpClump* clump);
    // reset materials settings. This one is called after vehicle rendering
    static void ResetEditableMaterials(RpClump* clump);
};

static inline RwSurfaceProperties gLightSurfProps {5.0, 0.0, 1.0};
static inline RwSurfaceProperties gLightOffSurfProps {0.0, 0.0, 1.0};


static inline std::list<std::pair<uint32*, uint32>> gStoredMaterials;
template <typename Addr>
static inline void AddStoredMaterial(Addr addr)
{
    gStoredMaterials.emplace_back(reinterpret_cast<uint32*>(addr), *reinterpret_cast<uint32*>(addr));
}


static_assert(sizeof(CVehicleModelInfo) == (VER_x32 ? 0x3a8 : 0x6B0), "Invalid size cTransmission");